#include "stc15f100.h"
#include "serial.h"
#include "led.h"

void ext0_irq(void) __interrupt(0)
{
    /**
     * When code hits here, INT0 line (P3.2), aka emulated serial Rx,
     *   must be receiving a transfer start bit.
     */

    TCON &= ~TCON_IE0;      ///< Clear INT0 interrupt flag

    serial_receive_begin();
}

void timer0_irq(void) __interrupt(1)
{
    TCON &= ~TCON_TF0;      ///< Clear TIM0 interrupt flag

    led_proc_irq();
}

void timer1_irq(void) __interrupt(3)
{
    TCON &= ~TCON_TF1;      ///< Clear TIM1 interrupt flag

    serial_proc_irq();
}