#include <msp430g2553.h>

unsigned char RX_BUFF;
void main(void)
{
 WDTCTL = WDTPW|WDTHOLD;                 // stop the watchdog timer

 DCOCTL=0;
     DCOCTL = CALDCO_1MHZ;

     BCSCTL1 = CALBC1_1MHZ;

        P2DIR |= BIT6;                // TA1CCR1 on P2.6
        P2SEL |= BIT6;                // TA1CCR1 on P2.6
        P2SEL2 &= ~(BIT6+BIT7);
        P2SEL &= ~BIT7;

        P2DIR |= BIT1;                // TA1CCR1 on P2.1
        P2SEL |= BIT1;                // TA1CCR1 on P2.1
        P2OUT &= ~(BIT1);


        P1DIR |= BIT7;
        P1SEL |= BIT7;
        P1OUT &= ~(BIT7);

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


        TA1CCR1 = 1800; //Default Positions for Both Servos
        TA0CCR1 =0;
        //////////////////////////////////////////////
        P1SEL = BIT1 + BIT2 + BIT4;    // Secondary Peripheral Module function selected
        P1SEL2 = BIT1 + BIT2 + BIT4;

        ////////////////////////////////////////////////
        UCA0CTL1 = UCSWRST;                       // Reset USCI
        UCA0CTL0 |= UCCKPL + UCMSB + UCSYNC;      // Setting up SPI Slave
        UCA0CTL1 &= ~UCSWRST;                     // Start USCI
        IE2 |= UCA0RXIE;                          // Enable RX interrupt

        ///////////////////////////////////////////////
         __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts

}



#pragma vector = TIMER1_A0_VECTOR
__interrupt void captureCompareInt (void) {
   // P1OUT |= BIT6;
    P2OUT |= BIT1;//Turn on LED
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void captureCompareInt2 (void) {
  // P1OUT &= ~BIT6;
    P2OUT &= ~BIT1;//Turn off LED
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void captureCompareInt3 (void) {
    //P1OUT |= BIT2;
    P2OUT &= ~BIT6;//Turn on LED
    TA0CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void captureCompareInt4 (void) {
    //P1OUT &= ~BIT2;
   P2OUT |= BIT6;//Turn off LED
    TA0CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

/////////////////////////////////////////////////////////////////



#pragma vector=USCIAB0RX_VECTOR
interrupt void USCI0RX_ISR (void)
{
    if(P1IN & BIT5)
    {
        RX_BUFF=UCA0RXBUF;

              if (RX_BUFF=='1')
                {
                    //TA0CCR1 = 0;               // Drops forearm
                    TA1CCR1 = 1800;
                    P1OUT |= BIT7;
                    __delay_cycles(400000);
                    TA1CCR1 = 1700;
                    P1OUT |= BIT7;
                    __delay_cycles(400000);
                    P1OUT |= BIT7;
                    TA1CCR1 = 1600;
                    __delay_cycles(400000);
                    P1OUT |= BIT7;
                    TA1CCR1 = 1500;
                    __delay_cycles(400000);
                    TA1CCR1 = 1400;
                    P1OUT |= BIT7;
                    __delay_cycles(400000);
                    P1OUT |= BIT7;
                    TA1CCR1 = 1500;
                    __delay_cycles(400000);
                    TA1CCR1 = 1600;         //Raises forearm
                    P1OUT |= BIT7;
                    __delay_cycles(400000);
                    P1OUT |= BIT7;
                    TA1CCR1 = 1700;
                    __delay_cycles(400000);
                    P1OUT |= BIT7;
                    TA1CCR1 = 2100;        //Forearm reaches apex
                    __delay_cycles(400000);
                    P1OUT |= BIT7;
                    TA0CCR1 = 1400;
                    TA1CCR1 = 2100;                // Turns arm around
                    __delay_cycles(750000);
                    P1OUT |= BIT7;
                    TA0CCR1 = 0;                  // Keeps arm position
                    TA1CCR1 = 2100;              /
                    __delay_cycles(700000);
                    P1OUT &= ~BIT7;
                    TA0CCR1 = 0;
                    TA1CCR1 = 1600;              // Lowers forearm for drop
                    __delay_cycles(700000);
                    P1OUT &= ~BIT7;
                    TA0CCR1 = 0;
                    TA1CCR1 = 2100;              // Returns forearm to apex
                    __delay_cycles(700000);
                    P1OUT &= ~BIT7;
                    TA0CCR1 = 600;                //Returns entire arm to front position
                    TA1CCR1 = 1800;
                    __delay_cycles(750000);
                    P1OUT &= ~BIT7;
                    TA0CCR1 = 0;
                    TA1CCR1 = 1900;
                    __delay_cycles(1000000);


                 }


    }


    else
    {
        TA1CCR1 = 1800; //Default Positions for Both Servos
         TA0CCR1 =0;
    }


}





