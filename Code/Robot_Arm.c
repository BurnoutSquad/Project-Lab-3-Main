#include <msp430g2553.h>

void main(void)
{
        WDTCTL = WDTPW|WDTHOLD;                 // stop the watchdog timer

        P2DIR |= BIT1;                // TA1CCR1 on P2.1 & P2.2
        P2SEL |= BIT1;                // TA1CCR1 on P2.1 & p2.2
        P2OUT &= ~(BIT1);


        P1DIR |= BIT6+BIT2;                        //use LED to indicate duty cycle has toggled
        P1REN |= BIT3;
        P1OUT |= BIT3;

        TA1CTL |= TASSEL_2|MC_1;           // configure for SMCLK
        TA1CCTL0 |= CCIE;                // enable CC interrupts
        TA1CCTL1 |= OUTMOD_7|CCIE;        // set TACCTL1 to Set / Reset mode//enable CC interrupts
        TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
        TA1CCR0 = 1000000/46;                // set signal period to 9999 clock cycles (~10 millisecond)

        TA0CTL |= TASSEL_2|MC_1;           // configure for SMCLK
        TA0CCTL0 |= CCIE;
        TA0CCTL1 |= OUTMOD_7|CCIE;        // set TACCTL1 to Set / Reset mode//enable CC interrupts
        TA0CCTL1 &= ~CCIFG;
        TA0CCR0= 2500;

        TA1CCR1 = 1400; //Default Positions for Both Servos
           TA0CCR1 =0;
        _enable_interrupt();

while(1)
{
          while(P1IN & BIT3);
          {
              //TA0CCR1 = 0;               // Drops forearm
             TA1CCR1 = 1200;
              __delay_cycles(1000000);
              TA1CCR1 = 1400;
               __delay_cycles(1000000);
              TA0CCR1 = 1400;               // Returns forearm into position
              TA1CCR1 = 1400;                // Turns arm around
              __delay_cycles(1150000);
              TA0CCR1 = 0;                  // Keeps arm position
              TA1CCR1 = 1200;              // Lowers forearm for drop
              __delay_cycles(1700000);

              TA0CCR1 = 700;                //Rotates arm to default position
              TA1CCR1 = 1400;
              __delay_cycles(1150000);
              TA0CCR1 = 0;
              TA1CCR1 = 1400;
              __delay_cycles(1000000);
          }
          TA1CCR1 = 1400;
}

}



#pragma vector = TIMER1_A0_VECTOR
__interrupt void captureCompareInt (void) {
    P1OUT |= BIT6;
    P2OUT |= BIT1;//Turn on LED
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void captureCompareInt2 (void) {
   P1OUT &= ~BIT6;
    P2OUT &= ~BIT1;//Turn off LED
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void captureCompareInt3 (void) {
    P1OUT |= BIT2;
   // P2OUT |= BIT4;//Turn on LED
    TA0CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void captureCompareInt4 (void) {
    P1OUT &= ~BIT2;
   // P2OUT &= ~BIT4;//Turn off LED
    TA0CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

/////////////////////////////////////////////////////////////////











