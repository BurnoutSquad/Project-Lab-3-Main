//
//  Tank_Test.c
//  
//
//  Created by Jeremiah McCutcheon on 2/11/18.
//
// MSP430 tank control test

/*
 
 HW I/O assignments:
 P1.0    not used
 P1.1    not used
 P1.2    not used
 P1.3    PUSHBUTTON  (input from Pushbutton on LaunchPad board
 P1.4    ENABLE B
 P1.5    IN 1
 P1.6    not used
 P1.7    ENABLE A
 
 P2.0    IN 2
 P2.1    IN 3
 P2.2    IN 4
 P2.3    not used
 P2.4    not used
 P2.5    not used
 P2.6    not used
 P2.7    not used
 
 */

#include  <msp430g2553.h>



#define PB  (0x08u) // (%00001000)

#define ENA (0x01u) // (%00000001)
#define ENB (0x08u) // (%00001000)
#define IN1 (0x04u) // (%00000100)
#define IN2 (0x80u) // (%10000000)
#define IN3 (0x40u) // (%01000000)
#define IN4 (0x20u) // (%00100000)

void FWD();

void main(void){
    
    WDTCTL = WDTPW + WDTHOLD; // Stop Watchdog Timer
    
    P1OUT =  PB;   // Pullup for PB
    P1REN |= PB;   // Resistor enable for PB position
    P1IE |=  PB;    // PB interrupt enabled
    P1IES |= PB;   // PB Hi/lo edge
    P1IFG &= ~0xFF;        // clear ALL the Interrupt Flags on Port 1
    
    
    P1DIR  = ENA;      // setup output port
    P1DIR |= ENB;      // setup output port
    P1DIR |= IN1;      // setup output port
    P2DIR  = IN2;      // setup output port
    P2DIR |= IN3;      // setup output port
    P2DIR |= IN4;      // setup output port
    
    _BIS_SR(GIE);       // enable general interrupts
    
    P1OUT  = ENA;        // Turn on Right Track
    P1OUT |= ENB;        // Turn on Left Track
    
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    FWD();
    P1IFG &= ~0xFF;  // clear ALL the Interrupt Flag bits on Port 1
}


    
void FWD(){
    P1OUT ^= IN1;
    P2OUT ^= IN3;
    __delay_cycles(18);
    P1OUT ^= IN1;
    P2OUT ^= IN3;
    __delay_cycles(2);
}
    
void REV(){
    P2OUT ^= IN2;
    P2OUT ^= IN4;
    __delay_cycles(18);
    P2OUT ^= IN2;
    P2OUT ^= IN4;
    __delay_cycles(2);
}

    
void Left(){
    P1OUT ^= IN1;
    P2OUT ^= IN4;
    __delay_cycles(18);
    P1OUT ^= IN1;
    P2OUT ^= IN4;
    __delay_cycles(2);
}

void Right(){
    P2OUT ^= IN2;
    P2OUT ^= IN3;
    __delay_cycles(18);
    P2OUT ^= IN2;
    P2OUT ^= IN3;
    __delay_cycles(2);
}

