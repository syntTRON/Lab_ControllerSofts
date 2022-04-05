/*
Übung 6 Labor Timer 3 Capture
Single tread only
Arthur Maximilian Scheibenpflug

4,19424ks
 */

#include <avr/io.h>
#include <avr/interrupt.h>

// Global Vars
uint8_t ovr = 0x00, mesurement = 0x00;

ISR(TIMER3_CAPT_vect)
{
    uint8_t lbyte, hbyte;
    hbyte = ICR3H;
    lbyte = ICR3L;
    if(ovr == 0xFF)
    {
        hbyte = 0xFF;
        lbyte = 0xFF;
    }

    //PORTB = lbyte;
    //PORTD = hbyte;
    PORTB = 0xFF;
    mesurement = 0x00;

}
ISR(TIMER3_OVR_vect)
{
    ovr = 0xFF;
    mesurement = 0x00;
    PORTB = 0xFF;

}

int main(void)
{
    //Port C Setup
    DDRC = 0x00;
    PORTC = 0x80;

    //Port D&B Setup
    DDRB = 0xFF;
    DDRD = 0xFF;
    PORTB = 0xFF;


    //ClkIO setup 16mHz
    CLKPR = 0x80;
    CLKPR = 0x00;

    // Timer 3 Capture setup
    TCCR3B = 0x41;//0x47
    TIMSK3 = 0x21;

    sei();


    while(1)
    {
        if(!PINC == 0x80 && mesurement == 0x00)
        {
            TCNT3 = 0x0000;
            ovr = 0x00;
            mesurement = 0xFF;
            PORTB = 0x00;
        }
    }

    return 0;
}
