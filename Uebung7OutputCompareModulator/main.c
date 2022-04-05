/*
Uebung 7 Output compare modulator
Timer 1 and Timer0
Fc = 100Hz
Fmod = 1KHz
CLK_IO = 8MHz
Compare modulator output PB7
 */

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{

    DDRB = 0x80;

    // Timer 0 Setup
    TCCR0A = 0x42;
    TCCR0B = 0x01;
    OCR0A = 15;

    //Timer 1 Setup
    TCCR1A = 0x04;
    TCCR1B = 0x1C;
    ICR1 = 312; //Top wert
    OCR1C = 312; //Trigger Value for timing


    while(1)
    {

    }

    return 0;
}
