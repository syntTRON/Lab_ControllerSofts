/*
Pin chance intrerupts lab build
Bild 2
 */

#include <avr/io.h>
#include <avr/interrupt.h>


ISR(PCINT0_vect)
{
    PORTD = ~PORTD;
}

int main(void)
{

    PCICR = 0x01;
    PCMSK0 = 0x01;

    DDRD = DDRD | (1<<PD7);
    PORTD = PORTD | (1<<PD7);

    sei();


    while(1)
    {

    }

    return 0;
}
