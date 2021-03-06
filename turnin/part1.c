/*	Author: klai022
 *  Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab 10  Exercise 1
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




unsigned char x, y;
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;


enum pauseButtonSM_States{pauseButton_wait, pauseButton_press, pauseButton_release};
enum toggleLED0_States{toggleLED0_wait, toggleLED0_blink};
enum toggleLED1_States{toggleLED1_wait, toggleLED1_blink};
enum display_States {display_display};
enum IsItPressedStates{Start, Wait, Press};

int IsItPressedSM(int state)
{
	switch(state)
	{
		case Start:
			state = Wait;
			break;

		case Wait:
			if(y != '\0')
			{
				
			
				state = Wait;
				break;
			}
			else
			{
			//PORTB = PORTB - 0x80;
			state = Press;
			break;
			}
		case Press:
			if(y != '\0')
			{
				PORTB = PORTB + 0x80;
				//state = Press;
				//break;
			}
			//else if(y == '\0')
			//{
			//	PORTB = PORTB + 0x80;
			//	state = Wait;
			//	break;
			//}
			/*	state = Press;
				break;
			}	
			else
			{
				PORTB = PORTB - 0x80;
				state = Wait;
				break;
			}*/
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
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;
    
    static task task1, task2, task3, task4;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;
    
    task1.state = Start;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &IsItPressedSM;
    
   
    
    TimerSet(100);/*GCD*/
    TimerOn();
    
    unsigned short i;
    
    

    while(1)
    {
        x = GetKeypadKey();
	y = x;
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


        }

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
