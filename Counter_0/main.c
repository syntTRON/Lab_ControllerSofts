/*
Lab Counter 0
 */

#include <avr/io.h>

unsigned char hvel;

int main(void)
{

    // Timer 0 Setup
    TCCR0B = 0x06;


    DDRB = 0xFF;
    PORTB = 0xFF;

    while(1)
    {
        if(TCNT0 >= 0x08)
        {
            TCNT0 = 0x00;
        }
        else
        {
            hvel = TCNT0;
            hvel = hvel << 5;
            PORTB= hvel >> 5;

        }



    }

    return 0;
}
