/*
 */

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect)
{
    PORTB = ~PORTB;
}

int main(void)
{

    CLKPR = 0x80;
    CLKPR = 0x00;

    DDRB = 0xFF;
    PORTB = 0x00;

    // Timer 1 Setup
    //TCCR1A = TCCR1A | (1<<WGM10);
    TCCR1B = TCCR1B | (1<<WGM13);
    TCCR1B = TCCR1B | (1<<CS11);

    TIMSK1 = 0x02;
    sei();

    ICR1 = 0x03E8;
    OCR1A = 0x0064;

    while(1)
    {

    }

    return 0;
}

