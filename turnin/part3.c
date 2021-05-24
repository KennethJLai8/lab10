/*	Author: klai022
 *  Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab 10  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include "keypad.h"
#include "timer.h"
#include "scheduler.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif











void ADC_init()

{

    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);

}


void set_PWM(double frequency) { //call this to set frequency of speaker

    static double current_frequency;


    if (frequency != current_frequency) {

        if(!frequency)

            TCCR3B &= 0x08;

        else

            TCCR3B |= 0x03;


        if(frequency < 0.954)

            OCR3A = 0xFFFF;

        else if (frequency > 31250)

            OCR3A = 0x0000;

        else

            OCR3A = (short) (8000000 / (128 * frequency)) - 1;

        

        TCNT3 = 0;

        current_frequency = frequency;

    }

}


void PWM_on() {//enables PWM

    TCCR3A = (1 << COM3A0);

    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

    set_PWM(0);

}


void PWM_off() {

    TCCR3A = 0x00;

    TCCR3B = 0x00;

}





unsigned char x, y;
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;

enum DoorBellStates{DoorBellStart, DoorBellWait, Note1, Note2, Note3, WaitLow};
double array[3] = {349.23, 329.63, 261.63};
unsigned char i = 0;
unsigned char j = 0;


enum pauseButtonSM_States{pauseButton_wait, pauseButton_press, pauseButton_release};
enum toggleLED0_States{toggleLED0_wait, toggleLED0_blink};
enum toggleLED1_States{toggleLED1_wait, toggleLED1_blink};
enum display_States {display_display};
//enum IsItPressedStates{Start, Wait, Press};
enum LockStates {LockStart, PoundPrePress, PoundRelease, OnePrePress, OneRelease, TwoPrePress, TwoRelease, ThreePrePress, ThreeRelease, FourPrePress, FourRelease, FivePrePress, FiveRelease, Unlock};      



int DoorbellSM(int state)
{
    switch(state)
    {
        case DoorBellStart:
            state = DoorBellWait;
            break;
            
        case DoorBellWait:
            if((~PINA & 0x80) == 0x80)
            {
                state = Note1;
                break;
            }
            else
            {
                state = DoorBellWait;
                break;
            }
            
        case Note1:
            if(i < 5)
            {
                set_PWM(array[j]);
                i++;
                break;
            }
            else
            {
                i = 0;
                j++;
                state = Note2;
                break;
            }
            
        case Note2:
            if(i < 5)
            {
                set_PWM(array[j]);
                i++;
                break;
            }
            else
            {
                i = 0;
                j++;
                state = Note3;
                break;
            }
            
            
        case Note3:
            if(i < 5)
            {
                set_PWM(array[j]);
                i++;
                break;
            }
            else
            {
                i = 0;
                j = 0;
		set_PWM(0.00);
                state = WaitLow;
                break;
            }

	case WaitLow:
	    if((~PINB & 0x80) == 0x80)
	    {
		    state = WaitLow;
	    }
	    else
	    {
		    state = DoorBellWait;
	    }
            
           
            
            
        
    }
    return state;

}




int LockSM(int state)
{

switch(state)
{
    case LockStart:
        state = PoundPrePress;
        break;
        
    case PoundPrePress:
        PORTB = 0x00;
	
	if(y == '\0')
	{
		state = PoundPrePress;
		break;
	}
	else if(y != '#')
        {
            
            state = PoundPrePress;
            break;
            
        }
        else if(y == '#')
        {
		//PORTB = 0x01;//debug
            state = PoundRelease;
            break;
        }
        
    case PoundRelease:
        if(y == '#')
        {
            state = PoundRelease;
            break;
        }
        else
        {
            state = OnePrePress;
            break;
        }
        
    case OnePrePress:
	if(y == '\0')
	{
		state = OnePrePress;
		break;
	}
	else if(y != '1')
        {
            state = PoundPrePress;
            break;
            
        }
        else if(y == '1')
        {
            state = OneRelease;
            break;
        }
        
    case OneRelease:
        if(y == '1')
        {
		//PORTB = 0x01;
            state = OneRelease;
            break;
        }
        else
        {
            state = TwoPrePress;
            break;
        }
        
    case TwoPrePress:
	if(y == '\0')
	{
		state = TwoPrePress;
		break;
	}
	else if(y != '2')
        {
            state = PoundPrePress;
            break;
            
        }
        else if(y == '2')
        {
            state = TwoRelease;
            break;
        }
        
    case TwoRelease:
        if(y == '2')
        {
		//PORTB = 0x01;
            state = TwoRelease;
            break;
        }
        else
        {
            state = ThreePrePress;
            break;
        }
        
    case ThreePrePress:

	if(y == '\0')
	{
		state = ThreePrePress;
		break;
	}
	else if(y != '3')
        {
            state = PoundPrePress;
            break;
            
        }
        else if(y == '3')
        {
            state = ThreeRelease;
            break;
        }
        
    case ThreeRelease:
        if(y == '3')
        {
		//PORTB = 0x01;
            state = ThreeRelease;
            break;
        }
        else
        {
            state = FourPrePress;
            break;
        }
        
    case FourPrePress:
	if(y == '\0')
	{
		state = FourPrePress;
		break;

	}
	else if(y != '4')
        {
		//PORTB = 0x01; success
            state = PoundPrePress;
            break;
            
        }
        else if(y == '4')
        {
		//PORTB == 0x01;
            state = FourRelease;
            break;
        }
        
    case FourRelease:
        if(y == '4')
        {
		//PORTB = 0x01;

            state = FourRelease;
            break;
        }
        else
        {
            state = FivePrePress;
            break;
        }
        
    case FivePrePress:
	if(y == '\0')
	{
		state = FivePrePress;
		break;
	}
        if(y != '5')
        {
            state = PoundPrePress;
            break;
            
        }
        else if(y == '5')
        {
            state = FiveRelease;
            break;
        }
        
    case FiveRelease:
        if(y == '5')
        {
            state = FiveRelease;
            break;
        }
        else
        {
            state = Unlock;
            break;
        }
        
    case Unlock:
        PORTB = 0x01;
        
        if((~PINB & 0x80) == 0x80)
        {
            PORTB = 0x00;
            state = PoundPrePress;
        break;
        }
        else
        {
            state = Unlock;
            break;
        }
        
        
        
}
return state;
}



int displaySMTick(int state)
{
    unsigned char output;

    switch(state)
    {
        case display_display: state = display_display; break;
        default: state = display_display; break;
    }

    switch(state)
    {
        case display_display:
            output = led0_output | led1_output << 1;

            break;
    }

    PORTB = output;
    return state;
}

int toggleLED0SMTick(int state)
{
    switch(state)
    {
        case toggleLED0_wait: state = !pause? toggleLED0_blink: toggleLED0_wait; break;
        case toggleLED0_blink: state = pause? toggleLED0_wait: toggleLED0_blink; break;
        default: state = toggleLED0_wait; break;
    }
    switch(state)
    {
        case toggleLED0_wait: break;
        case toggleLED0_blink:
            led0_output = (led0_output == 0x00) ? 0x01 : 0x00;
            break;
    }
    return state;
}

int toggleLED1SMTick(int state)
{
    switch(state)
    {
        case toggleLED1_wait: state = !pause? toggleLED1_blink: toggleLED0_wait; break;
        case toggleLED1_blink: state = pause? toggleLED1_wait: toggleLED0_blink; break;
        default: state = toggleLED1_wait; break;
    }
    switch(state)
    {
        case toggleLED1_wait: break;
        case toggleLED1_blink:
            led1_output = (led1_output == 0x00) ? 0x01 : 0x00;
            break;
    }
    return state;
}

int pauseButtonSMTick(int state)
{
    unsigned char press = ~PINA & 0x01;

    switch(state)
    {
        case pauseButton_wait:
            state = press == 0x01 ? pauseButton_press: pauseButton_wait; break;
        case pauseButton_press:
            state = pauseButton_release; break;
        case pauseButton_release:
            state = press == 0x00? pauseButton_wait: pauseButton_press; break;
        default: state = pauseButton_wait; break;
    }
    switch(state)
    {
        case pauseButton_wait: break;
        case pauseButton_press:
            pause = (pause == 0) ? 1 : 0;
            break;

        case pauseButton_release: break;
    }

    return state;
}

typedef struct _task{
	// Tasks should have members that include: state, period,
	//a measurement of elapsed time, and a function pointer.
	signed 	 char state; 		//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function

} task;




int main(void)
{

    unsigned char pause = 0;
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x7B; PORTB = 0x80;
    DDRC = 0xF0; PORTC = 0x0F;
    
    static task task1, task2, task3, task4;
    task *tasks[] = {&task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;
    
    task1.state = DoorBellStart;
    task1.period = 200;
    task1.elapsedTime = task1.period;
    task1.TickFct = &DoorbellSM;

    task2.state = LockStart;
    task2.period = 100;
    task2.elapsedTime = task2.period;
    task2.TickFct = &LockSM;
    
   
    
    TimerSet(100);/*GCD*/
    TimerOn();
    PWM_on();
    
    unsigned short i;
    
    

    while(1)
    {
        x = GetKeypadKey();
	y = x;
	/*
        switch(x)
        {
            case '\0': PORTB = 0x1F; break;
            case '1': PORTB = 0x01; break;
            case '2': PORTB = 0x02; break;
            case '3': PORTB = 0x03; break;
            case '4': PORTB = 0x04; break;
            case '5': PORTB = 0x05; break;
            case '6': PORTB = 0x06; break;
            case '7': PORTB = 0x07; break;
            case '8': PORTB = 0x08; break;
            case '9': PORTB = 0x09; break;
            case 'A': PORTB = 0x0A; break;
            case 'B': PORTB = 0x0B; break;
            case 'C': PORTB = 0x0C; break;
            case 'D': PORTB = 0x0D; break;
            case '*': PORTB = 0x0E; break;
            case '0': PORTB = 0x00; break;
            case '#': PORTB = 0x0F; break;
            default: PORTB = 0x1B; break;
        }*/

	for(i = 0; i < numTasks; i++)
        {
            if(tasks[i]->elapsedTime == tasks[i]->period)
            {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 100; /*GCD*/
        }
        
        while(!TimerFlag);
        TimerFlag = 0;

    }
}
