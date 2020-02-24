#include <stdint.h>
#include "msp.h"

void ta3dummy(uint16_t t){};       // dummy function
void (*CaptureTask0)(uint16_t time) = ta3dummy;// user function
void (*CaptureTask2)(uint16_t time) = ta3dummy;// user function

//------------TimerA3Capture_Init------------
// Initialize Timer A3 in edge time mode to request interrupts on
// the rising edges of P10.4 (TA3CCP0) and P8.2 (TA3CCP2).  The
// interrupt service routines acknowledge the interrupt and call
// a user function.
// Input: task0 is a pointer to a user function called when P10.4 (TA3CCP0) edge occurs
//              parameter is 16-bit up-counting timer value when P10.4 (TA3CCP0) edge occurred (units of 0.083 usec)
//        task2 is a pointer to a user function called when P8.2 (TA3CCP2) edge occurs
//              parameter is 16-bit up-counting timer value when P8.2 (TA3CCP2) edge occurred (units of 0.083 usec)
// Output: none
// Assumes: low-speed subsystem master clock is 12 MHz
// P8.2 -> TA3.CCI2A; P10.4 -> TA3.CCI0A

void TimerA3Capture_Init(void(*task0)(uint16_t time), void(*task2)(uint16_t time)){
  // write this as part of lab 4
    CaptureTask0 = task0;              // user function
    CaptureTask2 = task2;              // user function
    // initialize P10.4 and make it input (P10.4 TA3CCR0)
    P10->SEL0 |= 0x10;
    P10->SEL1 &= ~0x10;                 // configure P10.4 as TA3CCR0
    P10->DIR &= ~0x10;                  // make P10.4 input direction
    // initialize P8.2 and make it input (P8.2 TA3CCR2)
    P8->SEL0 |= 0x04;
    P8->SEL1 &= ~0x04;                 // configure P8.2 as TA3CCR2
    P8->DIR &= ~0x04;                  // make P8.2 input direction

    TIMER_A3->CTL &= ~0x0030;               // halt Timer A3 TAxCTL
    // bits15-10=XXXXXX, reserved
    // bits9-8=10,       clock source to SMCLK
    // bits7-6=00,       input clock divider /1
    // bits5-4=00,       stop mode
    // bit3=X,           reserved
    // bit2=0,           set this bit to clear
    // bit1=0,           interrupt disable
    // bit0=0,           clear interrupt pending
    TIMER_A3->CTL = 0x0200;
    // bits15-14=01,     capture on rising edge
    // bits13-12=00,     capture/compare input on CCI0A
    // bit11=1,          synchronous capture source
    // bit10=X,          synchronized capture/compare input
    // bit9=X,           reserved
    // bit8=1,           capture mode
    // bits7-5=XXX,      output mode
    // bit4=1,           enable capture/compare interrupt
    // bit3=X,           read capture/compare input from here
    // bit2=X,           output this value in output mode 0
    // bit1=X,           capture overflow status
    // bit0=0,           clear capture/compare interrupt pending
    TIMER_A3->CCTL[0] = 0x4910; // capture on rising edge, CCIxA, synchronous capture, capture mode, OUT bit value, interrupt enabled
    TIMER_A3->CCTL[2] = 0x4910; // capture on rising edge, CCIxA, synchronous capture, capture mode, OUT bit value, interrupt enabled
    TIMER_A3->EX0 &= ~0x0007;       // configure for input clock divider /1
    NVIC->IP[3] = (NVIC->IP[3]&0xFFFFFF00)|0x00000040; // priority 2
  // interrupts enabled in the main program after all devices initialized
    NVIC->ISER[0] = 0x0000C000; // enable interrupt 8 in NVIC
    TIMER_A3->CTL |= 0x0024;        // reset and start Timer A3 in continuous up mode
    // bits15-10=XXXXXX, reserved
    // bits9-8=10,       clock source to SMCLK
    // bits7-6=00,       input clock divider /1
    // bits5-4=10,       continuous count up mode
    // bit3=X,           reserved
    // bit2=1,           set this bit to clear
    // bit1=0,           interrupt disable (no interrupt on rollover)
    // bit0=0,           clear interrupt pending

}

void TA3_0_IRQHandler(void){
  // write this as part of lab 4
    TIMER_A3->CCTL[0] &= ~0x0001;             // acknowledge capture/compare interrupt 0
    (*CaptureTask0)(TIMER_A3->CCR[0]);         // execute user task
}

void TA3_N_IRQHandler(void){
  // write this as part of lab 4
    TIMER_A3->CCTL[2] &= ~0x0001;             // acknowledge capture/compare interrupt 0
    (*CaptureTask2)(TIMER_A3->CCR[2]);         // execute user task
}
