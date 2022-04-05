/*
Lab timer 0 uebung 3
 */

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned int zsave = 0x0000;

ISR(TIMER0_OVF_vect)
{
    zsave = zsave + 1;
}

int main(void)
{

    CLKPR = 0x80;
    CLKPR = 0x00;

    TIMSK0 = 0x01;
    TCCR0B = 0x01;

    DDRB = 0x01;


    sei();

    while(1)
    {
        if(zsave > 31250)
        {
            PORTB = PORTB ^ 0x01;
            zsave = 0;
        }


    }

    return 0;
}
