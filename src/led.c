#include "led.h"
#include "stc15f100.h"
#include "seg15_ascii.h"

#define LED_OE              Pin5
#define LED_DOUT            Pin0
#define LED_SCLK            Pin3
#define LED_RCLK            Pin4

#define OE_H                (P3 |= LED_OE)
#define OE_L                (P3 &= ~LED_OE)
#define DOUT_H              (P3 |= LED_DOUT)
#define DOUT_L              (P3 &= ~LED_DOUT)
#define SCLK_H              (P3 |= LED_SCLK)
#define SCLK_L              (P3 &= ~LED_SCLK)
#define RCLK_H              (P3 |= LED_RCLK)
#define RCLK_L              (P3 &= ~LED_RCLK)

#define LED_CMD_HEADER1     (0x5A)
#define LED_CMD_HEADER2     (0xA5)
#define LED_CNT             (8)
#define LED_DATALEN         (LED_CNT * 2)

#define LED_REFRESH_RATE    (100)
#define LED_TIMER_PRESC     (2)
#define LED_COMM_RATE       (LED_REFRESH_RATE*LED_CNT)
#define LED_TIMER_VAL       (65536L - 22118400UL / LED_TIMER_PRESC / LED_COMM_RATE)
#define LED_TIMER_H         ((uint8_t)(LED_TIMER_VAL>>8))
#define LED_TIMER_L         ((uint8_t)LED_TIMER_VAL)

/** Flags */
uint8_t g_led_refresh;
uint8_t g_1sec;

/** Prescalers */
uint8_t g_led_timer_presc;
uint16_t g_led_1sec_presc;

/** Command */
uint8_t g_led_cmd_id;

/** Display buffer */
uint16_t g_led_array[LED_CNT];
uint8_t g_led_digit;
const uint8_t * g_led_digit_ptr;

#if 1
const uint8_t g_led_mask[LED_CNT] = 
{
    0xFE,
    0xFD,
    0xFB,
    0xF7,
    0xEF,
    0xDF,
    0xBF,
    0x7F
};
#else
const uint8_t g_led_mask[LED_CNT] = 
{
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80
};
#endif

void led_init(void)
{
    uint8_t i;

    /** Variable init */
    for (i = 0; i < LED_CNT; ++i)
    {
        g_led_array[i] = g_seg15_ascii[i+1];
    }
    g_led_refresh = 0;
    g_1sec = 0;

    g_led_1sec_presc = 0;
    g_led_timer_presc = 0;

    g_led_digit = LED_CNT;
    g_led_digit_ptr = (const uint8_t *)g_led_array;

    g_led_cmd_id = 0;

    /** IO init */
    P3M0 |= LED_OE | LED_DOUT | LED_SCLK | LED_RCLK;    ///< All driving ports are set to push-pull
    P3M1 &= ~(LED_OE | LED_DOUT | LED_SCLK | LED_RCLK);
    OE_H;
    SCLK_L;
    DOUT_L;
    RCLK_L;

    /** Timer 0 init */
    TMOD &= ~(TMOD_GATE0 | TMOD_CT0 | TMOD_M10 | TMOD_M00);
    AUXR |= AUXR_T0X12;                                 ///< Timer0 in 1T mode
    TL0 = LED_TIMER_L;
    TH0 = LED_TIMER_H;
    IE |= IE_EA | IE_ET0;
    TCON |= TCON_TR0;                                   ///< Timer0 start running
}

void led_set_refresh_flag(uint8_t value)
{
    g_led_refresh = value;
}

uint8_t led_need_refresh(void)
{
    return g_led_refresh;
}

void spi_send(uint8_t byte)
{
    /** Assuming CS signal is already lowed */

    P3_0 = byte & 0x80;
    SCLK_H;
    SCLK_L;
    P3_0 = byte & 0x40;
    SCLK_H;
    SCLK_L;
    P3_0 = byte & 0x20;
    SCLK_H;
    SCLK_L;
    P3_0 = byte & 0x10;
    SCLK_H;
    SCLK_L;
    P3_0 = byte & 0x08;
    SCLK_H;
    SCLK_L;
    P3_0 = byte & 0x04;
    SCLK_H;
    SCLK_L;
    P3_0 = byte & 0x02;
    SCLK_H;
    SCLK_L;
    P3_0 = byte & 0x01;
    SCLK_H;
    SCLK_L;
}

void led_refresh(void)
{
    uint8_t i;
    uint8_t datah, datal, digit;

    /** Prepare data */
    // i = g_led_digit;
    // g_led_digit_ptr = (const uint8_t *)(&g_led_array[g_led_digit]);
    datah = *g_led_digit_ptr++;
    datal = *g_led_digit_ptr++;
    digit = g_led_mask[g_led_digit-1];

    /** Emulate SPI */
    OE_H;
    spi_send(datal);        ///< Amendment for SDCC endianness (little-endian)
    spi_send(datah);
    spi_send(digit);
    RCLK_H;
    RCLK_L;
    OE_L;

    /** Change digit */
    if (--g_led_digit == 0)
    {
        g_led_digit = LED_CNT;
        g_led_digit_ptr = (const uint8_t *)g_led_array;     ///< Reset buffer ptr
    }

    /** Clear flag */
    g_led_refresh = 0;
}

void led_command_recv(uint8_t byte)
{
    if (g_led_cmd_id == 0)
    {
        /** Expects header1 0x55 */
        if (byte == 0x55)
        {
            g_led_cmd_id = 1;
        }
    }
    else if (g_led_cmd_id == 1)
    {
        /** Expects header2 0xAA */
        if (byte == 0xAA)
        {
            g_led_cmd_id = 2;
        }
        else
        {
            g_led_cmd_id = 0;       ///< Dump if not followed
        }
    }
    else if (g_led_cmd_id < LED_DATALEN + 2)
    {
        /** Expects display data */
        g_led_array[g_led_cmd_id - 2] = byte;
        ++g_led_cmd_id;
    }
    else
    {
        g_led_cmd_id = 0;
    }
}

void led_proc_irq(void)
{
    if (++g_led_timer_presc == LED_TIMER_PRESC)
    {
        g_led_timer_presc = 0;
        led_set_refresh_flag(1);    ///< Set flag to trigger a new refresh
    }

    if (++g_led_1sec_presc == LED_TIMER_PRESC * LED_REFRESH_RATE * LED_CNT)
    {
        g_led_1sec_presc = 0;
        g_1sec = 1;
    }
}

void led_set_display(uint8_t word)
{
    uint8_t i;

    for (i = 0;i < LED_DATALEN; ++i)
    {
        g_led_array[i] = word;
    }
}

uint8_t led_1sec(void)
{
    uint8_t temp = g_1sec;

    g_1sec = 0;
    return temp;
}
