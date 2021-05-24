/*	Author: klai022
 *  Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab 10  Exercise 2
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

enum KeypadStates{KeyStart, KeyPress};
enum pauseButtonSM_States{pauseButton_wait, pauseButton_press, pauseButton_release};
enum toggleLED0_States{toggleLED0_wait, toggleLED0_blink};
enum toggleLED1_States{toggleLED1_wait, toggleLED1_blink};
enum display_States {display_display};
//enum IsItPressedStates{Start, Wait, Press};
enum LockStates {Start, PoundPrePress, PoundRelease, OnePrePress, OneRelease, TwoPrePress, TwoRelease, ThreePrePress, ThreeRelease, FourPrePress, FourRelease, FivePrePress, FiveRelease, Unlock};      




int KeypadSM(int state)
{
    

   switch(state)
    {
        case KeyStart:
            state = KeyPress;
            break;
        
        case KeyPress:
        
        x = GetKeypadKey();
         y = x;

    
	
            
    }
   return state;
}







int LockSM(int state)
{

switch(state)
{
    case Start:
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
		//PORTB = 0x01;debug
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
    DDRB = 0x7B; PORTB = 0x80;
    DDRC = 0xF0; PORTC = 0x0F;
    
    static task task1, task2, task3, task4;
    task *tasks[] = {&task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    task1.state = KeyStart;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &KeypadSM;


    
    task2.state = Start;
    task2.period = 100;
    task2.elapsedTime = task2.period;
    task2.TickFct = &LockSM;
    
   
    
    TimerSet(100);/*GCD*/
    TimerOn();
    
    unsigned short i;
    
    

    while(1)
    {

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
