/*
Uebung 8
Bot PWM Drive Timer 4
with a plus of automatic Battery Voltage detection and safety self Shutof
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define PWM_STOPP_RIGHT	300
#define PWM_STOPP_LEFT	302
#define PWM_MAX			400
#define PWM_MIN			200

// PORTB
#define LED_RV			(1<<0)
#define LED_RH			(1<<1)
#define LED_LV			(1<<2)
#define LED_LH			(1<<3)
#define LED_GRUEN		(1<<4)
#define LED_ROT			(1<<5)
#define MOT_RIGHT		(1<<6)

// PORTD
// PD0 -> I2C SCL
// PD1 -> I2C SDA
#define RECEIVER		(1<<2)
#define TRANSMITTER		(1<<3)
#define WHEEL_RIGHT		(1<<4)
#define DEBUG1			(1<<5)
#define LF_EMITTER		(1<<6)
#define MOT_LEFT		(1<<7)

//PORTF
#define MEASURE_UB		(1<<0)
#define ADC_UB			0
#define LF_LEFT			(1<<1)
#define ADC_LF_LEFT		1
#define LF_RIGHT		(1<<4)
#define ADC_LF_RIGHT	4
#define CHOOSE_LR		(1<<5)
#define CHOOSE_FB		(1<<6)
#define DEBUG2			(1<<7)

//PORTC
#define IO_RESET		(1<<6)
#define WHEEL_LEFT		(1<<7)

//PORTE
#define IO_INT			(1<<6)

//Motorick
#define PWM_R_VOR_MAX	125				//2 ms
#define PWM_R_RET_MAX	62				//0,992 ms
#define PWM_R_STOPP		94				//1,504 ms

#define PWM_L_VOR_MAX	62				//0,992 ms
#define PWM_L_RET_MAX	125				//2 ms
#define PWM_L_STOPP		94				//1,504 ms


unsigned char ADC_Mesure_Compleated = 0x00;
unsigned char ISR_Occured=0x00;
uint16_t ADC_Val=0x0000;

ISR(TIMER0_OVF_vect)
{
    ISR_Occured=0xFF;
}

ISR(ADC_vect)
{
    ADC_Mesure_Compleated=0xFF;
}

void add_to_enable_hibbernetes(void)
{

}

void add_to_disable_hibbernetes(void)
{

}

void add_to_main(void)
{

}

void adc_measure_init(unsigned char channel)
{
	//unsigned int result=0;

	ADMUX = 0;
	ADMUX &= ~(1<<REFS1)&~(1<<REFS0);			//ext. AREF = 5V
	ADMUX &= ~(1<<ADLAR);						//Right Adjusted 10bit

	ADCSRB &= ~(1<<MUX5);
	ADMUX &= ~(1<<MUX4)&~(1<<MUX3)&~(1<<MUX2)&~(1<<MUX1);
	ADMUX |= channel;

	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADIE)|(1<<ADIF);	//ADC Enable, Prescaler 128 -> 125kHz Samplingfrequenzy

	ADCSRA |= (1<<ADSC);						//start
	//ADCSRA=0xC8;
	//while(ADCSRA&(1<<ADSC));					//wate till completion -- is remouved

}

void adc_val_update(void)
{
    	uint8_t adcl_tmp=0x00, adch_tmp=0x00;
        adcl_tmp=ADCL;
        adch_tmp=ADCH;
        ADC_Val=(adch_tmp<<8)|adcl_tmp;
}

int main(void)
{

    // Clk
    CLKPR=0x80;
    CLKPR=0x00;
    //Disable Jtag
    MCUCR |= (1<<JTD);
	MCUCR |= (1<<JTD);

    //Timer 0 Setup
    TCCR0A=0x00;
    TCCR0B=0x04;
    TIMSK0=0x01;
//0x3F
    DDRB=0xFF;
    PORTB=0x1F;
//0x40
    DDRD=0xFF;
    PORTD=0x00;

    //Timer4 HardwarePWM Setup
    TCCR4A = TCCR4A | (1<<PWM4B);
	TCCR4C = TCCR4C | (1<<PWM4D);
	TCCR4D = TCCR4D &~(1<<WGM41);
	TCCR4D = TCCR4D &~(1<<WGM40);		//Fast PWM on OC4B and OC4D

	TCCR4A = TCCR4A &~(1<<COM4B0);
	TCCR4A = TCCR4A | (1<<COM4B1);		//COM4B1:0=2

	TCCR4C = TCCR4C &~(1<<COM4D0);
	TCCR4C = TCCR4C | (1<<COM4D1);		//COM4D1:0=2

	TC4H = 0x03;
	OCR4C = 0xE8;						//f_PWM = f_CLK_T4/(1+OCR4C) = 62,5kHz/1000 = 62,5 Hz
	TC4H = 0x00;
	OCR4B = PWM_R_STOPP;				//Duty cycle on OC4B-Pin (PB6), PWM_right
	OCR4D = PWM_L_STOPP;				//Duty cycle on OC4D-Pin (PD7), PWM_left

	TCCR4B = TCCR4B | (1<<CS43);
	TCCR4B = TCCR4B &~(1<<CS42);
	TCCR4B = TCCR4B &~(1<<CS41);		//f_CLK_T4 = CLK_IO/Prescaler = 16MHz/256 = 62,5kHz
	TCCR4B = TCCR4B | (1<<CS40);		//Timer4 Prescaler = 1, Start PWM

	//Enable interupts
    sei();

    uint8_t ISR_Count=0x00;
    ADC_Val=1000;

    uint8_t Hibbernetes=0x00;
    uint8_t HibberISRcount=0x00;

    uint8_t Engenus=94;
    uint8_t Engine_Clk=0x00;
    uint8_t Engine_Rut=0x00;

    //Engines
    //reight
    OCR4B=PWM_R_STOPP;
    //left
    OCR4D=PWM_L_STOPP;

    unsigned char blnk_ind=0x00;

    while(1)
    {
        if(ISR_Occured==0xFF)
        {
            ISR_Occured=0x00;
            ISR_Count=ISR_Count+1;
        }

        if(ISR_Count==0xFF)
        {
            adc_measure_init(0);
            ISR_Count=0x00;
            Engine_Clk=0xFF;
        }

        if(ADC_Mesure_Compleated==0xFF)
        {
            ADC_Mesure_Compleated=0x00;

            adc_val_update();
        }


        if(ADC_Val>=874)
        {
           PORTB=0x1F;
        }
        else if(ADC_Val>=759&&ADC_Val<=873)
        {
            //PORTB=0x2F;
            if(blnk_ind==1)
            {
                PORTB=0x21;
                blnk_ind=blnk_ind+1;
            }
            else if(blnk_ind==2)
            {
                PORTB=0x22;
                blnk_ind=blnk_ind+1;
            }
            else if(blnk_ind==3)
            {
                PORTB=0x24;
                blnk_ind=blnk_ind+1;
            }
            else if(blnk_ind==4)
            {
                PORTB=0x28;
                blnk_ind=blnk_ind+1;
            }
            else{PORTB=0x2F;blnk_ind=1;}
        }
        else if(ADC_Val<=758)
        {
            Hibbernetes=1;
            PORTB=0x00;
            PORTB=0x20;
            //Engines Stop
            OCR4B=PWM_R_STOPP;
            OCR4D=PWM_L_STOPP;
            TCNT4H = 0;
            add_to_enable_hibbernetes();
            while(Hibbernetes==1)
            {
                if(ISR_Occured==0xFF)
                {
                    ISR_Occured=0x00;
                    HibberISRcount=HibberISRcount+1;
                }
                else if(HibberISRcount>=64+64)
                {
                    if(PORTB==0x20)
                    {
                        PORTB=0x00;
                    }
                    else{PORTB=0x20;}
                    adc_measure_init(0);
                }
                else if(ADC_Mesure_Compleated==0xFF)
                {
                    ADC_Mesure_Compleated=0x00;
                    adc_val_update();
                }
                else if(ADC_Val>=874){Hibbernetes=0;}
            }
            add_to_disable_hibbernetes();
        }
        //---------------------Main Prog under This Text
        add_to_main();

        //Engine Drive
        if(Engine_Clk==0xFF)
        {
            Engine_Clk=0x00;
            if(Engine_Rut==0xFF)
            {
                if(Engenus>=125)
                {
                    Engine_Rut=0x00;
                }
                else{Engenus=Engenus+1;}
            }
            else if(Engine_Rut==0x00)
            {
                if(Engenus<=62)
                {
                    Engine_Rut=0xFF;
                }
                else{Engenus=Engenus-1;}
            }
            else{Engine_Rut=0xFF;}

            TCNT4H=0;
            OCR4B=Engenus;
            TCNT4H=0;
            OCR4D=Engenus;
        }

    }

    return 0;
}
