#include "stc15f100.h"
#include "serial.h"
#include "led.h"

/**
 * Configuration:
 *
 * P3.2 -> Rx (Receiving display command)
 *
 * P3.3 -> SCLK
 * P3.4 -> RCLK
 * P3.5 -> OE
 * P3.0 -> DOUT
 */

int main(void)
{
    SP = 0x60;      ///< Set stack pointer to 0x60

    serial_on_byte_receive(led_command_recv);
    serial_init();
    led_init();

    // P3M0 |= Pin1;
    // P3M1 &= ~Pin1;

    while (1)
    {
        if (led_1sec())
        {
            led_set_display();
        }

        if (led_need_refresh())
        {
            led_refresh();
        }
    }

    return 0;
}

extern void ext0_irq(void) __interrupt(IE0_VECTOR);
extern void timer0_irq(void) __interrupt(TF0_VECTOR);
extern void timer1_irq(void) __interrupt(TF1_VECTOR);
