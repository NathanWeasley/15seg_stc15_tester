#include "led.h"
#include "stc15f100.h"

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

#define LED_REFRESH_RATE    (25)
#define LED_TIMER_PRESC     (2)
#define LED_COMM_RATE       (LED_REFRESH_RATE*LED_CNT)
#define LED_TIMER_VAL       (65536L - 22118400UL / LED_TIMER_PRESC / LED_COMM_RATE)
#define LED_TIMER_H         ((uint8_t)(LED_TIMER_VAL>>8))
#define LED_TIMER_L         ((uint8_t)LED_TIMER_VAL)

uint8_t g_led_refresh;
uint8_t g_led_timer_presc;
uint8_t g_led_digit;

uint8_t g_led_array[LED_DATALEN];

void led_init(void)
{
    uint8_t i;

    /** Variable init */
    for (i = 0; i < LED_DATALEN; ++i)
    {
        g_led_array[i] = 0xFF;
    }
    g_led_refresh = 0;
    g_led_timer_presc = LED_TIMER_PRESC;
    g_led_digit = LED_CNT;

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

void led_refresh(void)
{
    uint8_t i, buf, dsel = 0x80;

    /** Emulate SPI */
    OE_H;

}

void led_command_recv(uint8_t byte)
{
    ;
}

void led_proc_irq(void)
{
    ;
}
