#include "serial.h"

/**
 * Theory of operation:
 *
 * STC15F100 series doesn't have a hardware UART port, so in order
 *   to perform serial communication, a software-emulated serial
 *   port must be implemented.
 * Serial transfers are asynchronous, meaning data must be written/
 *   sampled at a given frequency, known as baudrate. Timer 1 is used
 *   to generate this baudrate, by interrupting at a given freqeuency.
 * Note that the official STC sample code sets timer frequency 4 times
 *   of the required baudrate, presumably to enhance a quasi-full-duplex
 *   transfer. For receive-only applications, timer frequency can be
 *   set to match the exact baudrate.
 * This implementation is a receive-only routine, emulated RxD is on P3.2.
 *   When a new byte is sent, start bit triggers the INT0 interrupt, which
 *   then 
 */

uint8_t g_bit_cnt;
uint8_t g_recv_buf;
void (*g_proc)(uint8_t) = 0;

void serial_init(void)
{
    /** Receive control variables */
    g_bit_cnt = 8;
    g_recv_buf = 0;

    /** Configure IO port */
    P3M0 &= ~Pin2;              ///< P3.2 to input mode
    P3M1 |= Pin2;

    /** Configure external interrupt */
    IE |= IE_EA | IE_EX0;       ///< Enable global and external interrupt 0
    TCON |= TCON_IT0;           ///< INT0 triggers only by falling edge

    /** Timer related registers */
    TMOD = 0x00;                ///< Timer 1 in auto-reload mode
    AUXR = AUXR_T1X12;          ///< Timer 1 in 1T mode
    TL1 = BAUD_L_1ST;           ///< Set Timer1 reload value
    TH1 = BAUD_H_1ST;
    IE |= IE_ET1;               ///< Enable Timer 1 interrupt

    /** Until now, MCU is configured to listen to falling edge on P3.2 */
}

void serial_on_byte_receive(void (*proc)(uint8_t))
{
    g_proc = proc;
}

void serial_receive_begin(void)
{
    IE &= ~IE_EX0;          ///< Disable INT0 interrupt during this transfer
    TCON |= TCON_TR1;       ///< Start timer 1 to record serial data
}

void serial_receive_end(void)
{
    g_proc(g_recv_buf);      ///< Invoke callback to send the received byte out

    TCON &= ~TCON_TR1;      ///< Stop Timer 1
    IE |= IE_EX0;           ///< Re-enable INT0 to wait for the next transfer

#ifdef DELAY_ON_FIRST
    TL1 = BAUD_L_1ST;       ///< Reset timer counter to 1.5*baudrate
    TH1 = BAUD_H_1ST;
#endif

    g_bit_cnt = 8;           ///< Reset bit counter
    g_recv_buf = 0;          ///< Clear receive buffer
}

void serial_proc_irq(void)
{
#ifdef DELAY_ON_FIRST
    TL1 = BAUD_L;           ///< Change timer reload value to match normal baudrate
    TH1 = BAUD_H;
#endif

    if (P3 & Pin2)
    {
        g_recv_buf |= 0x80;  ///< Read in the bit
    }

    if (--g_bit_cnt == 0)
    {
        /** This is the last bit of this transfer */

         serial_receive_end();
    }
    else
    {
        g_recv_buf >>= 1;
    }
}

#if 0

uint8_t Tbuf, Rbuf;     ///< I/O buffer (always intact)
uint8_t Tdat, Rdat;     ///< I/O register (will be shifted)
uint8_t Tcnt, Rcnt;     ///< Baudrate generating prescalar
uint8_t Tbit, Rbit;     ///< Bit counter
uint8_t Tstatus;        ///< [ - - - - Ting, Ring, Tend, Rend]

uint8_t r, t;
uint8_t Rarray[16];     ///< Buffer

#define TING            (Tstatus & 0x08)
#define RING            (Tstatus & 0x04)
#define TEND            (Tstatus & 0x02)
#define REND            (Tstatus & 0x01)
#define SET_TING        (Tstatus |= 0x08)
#define CLR_TING        (Tstatus &= ~0x08)
#define SET_RING        (Tstatus |= 0x04)
#define CLR_RING        (Tstatus &= ~0x04)
#define SET_TEND        (Tstatus |= 0x02)
#define CLR_TEND        (Tstatus &= ~0x02)
#define SET_REND        (Tstatus |= 0x01)
#define CLR_REND        (Tstatus &= ~0x01)

void serial_init()
{
    /** Transfer control variables */
    Tstatus = 0x02;
    Tcnt = 0;
    Rcnt = 0;
    r = 0;
    t = 0;

    /** Timer related registers */
    TMOD = 0x00;        ///< Timer 1 in auto-reload mode
    AUXR = 0x40;        ///< Timer 1 in 1T mode
    TL1 = BAUD_L;       ///< Set reload value
    TH0 = BAUD_H;
    ET1 = 1;            ///< Enable Timer 1 interrupt
    PT1 = 1;            ///< Improve Timer1 interrupt priority
    EA = 1;             ///< Enable global interrupt
    TR1 = 1;            ///< Timer 1 start running
}

void serial_proc_io()
{
    if (REND)
    {
        /** Upon a finished reciving */

        CLR_REND;
        Rarray[r++ & 0x0F] = Rbuf;
    }

    if (TEND)
    {
        if (t != r)
        {
            CLR_TEND;
            Tbuf = Rarray[t++ & 0x0F];
            SET_TING;
        }
    }
}

void serial_proc_irq()
{
    if (RING)
    {
        /** During receiving */

        if (--Rcnt == 0)
        {
            Rcnt = 3;
            if (--Rbit == 0)
            {
                Rbuf = Rdat;
                CLR_RING;           ///< Stop receiving
                SET_REND;
            }
            else
            {
                Rdat >>= 1;
                if (P3_0)
                {
                    Rdat |= 0x80;   ///< Read 1 bit
                }
            }
        }
    }
    else if (!P3_0)
    {
        /** Not receiving but low detected on P3.0 (RX), start receiving */

        SET_RING;                   ///< Set receiving flag
        Rcnt = 4;                   ///< Baudrate counter (prescalar)
        Rbit = 9;                   ///< Bits to receive (8 data + 1 stop)
    }

    if (--Tcnt == 0)
    {
        Tcnt = 3;
        if (TING)
        {
            if (Tbit == 0)
            {
                /** It's a new transfer */

                P3_1 = 0;           ///< Send start bit
                Tdat = Tbuf;        ///< Send data to output buffer
                Tbit = 9;           ///< Bits to send (8 data + 1 stop)
            }
            else
            {
                /** During previous transfer */

                Tdat >>= 1;         ///< Shifted bit is in CY
                if (--Tbit == 0)
                {
                    P3_1 = 1;       ///< Stop bit
                    CLR_TING;
                    SET_TEND;
                }
                else
                {
                    P3_1 = PSW_CY;
                }
            }
        }
    }
}

#endif
