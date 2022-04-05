/*
Software PWM mit atmegea32u4 interupts - Single tread save
 */

#include <avr/io.h>
#include <avr/interrupt.h>

//Globale Variablen - Multi tread save
unsigned int Inter = 0x00;


ISR(TIMER0_COMPA_vect)
{
    Inter = Inter + 0x01;
}

ISR(TIMER1_COMPA_vect)
{
    PORTB = 0xFF;
}

ISR(TIMER1_COMPB_vect)
{
    PORTB = 0x00;
}

int main(void)
{
    // Ports Setup
    DDRB = 0xFF;
    PORTB = 0xFF;

    // CLK Setup
    CLKPR = 0x80;
    CLKPR = 0x00;

    // Timer 0 setup
    TCCR0A = 0x02;
    TCCR0B = 0x09; // Timer Takt
    OCR0A = 0xFA;
    TIMSK0 = 0x02;

    // Timer 1 Setup
    OCR1A = 0x03E8;
    TIMSK1 = 0x06;
    TCCR1B = 0x09;

    TCNT0 = 0x00;
    TCNT1 = 0x0000;

    sei();

    unsigned int timing = 0;

    while(1)
    {



    if (Inter >= 120)
    {
        timing += 1;
        Inter = 0;
        OCR1B = timing;
    }
    if (timing >= 1000)
    {
        timing = 0;
    }





    }

    return 0;
}
