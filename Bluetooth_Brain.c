////////////////////////////////////////
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
 P1.3    PUSHBUTTON  // used for testing
 P1.4    not used
 P1.5    not used
 P1.6    not used
 P1.7    not used

 P2.0    ENA
 P2.1    IN1
 P2.2    IN2
 P2.3    IN3
 P2.4    IN4
 P2.5    ENB
 P2.6    not used
 P2.7    not used

 */

#include  <msp430g2553.h>

//unsigned int button = 0;
unsigned char Rx_Data=0;

void FWD();
void Left();
void Right();
void REV();
void Stop();

void main(void){

    /*** Set-up system clocks ***/
    WDTCTL = WDTPW + WDTHOLD;               // Stop WDT
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;                  // Set DCO
    DCOCTL = CALDCO_1MHZ;
    /*** Set-up GPIO ***/
    P1SEL = BIT1 + BIT2;                    // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;                   // P1.1 = RXD, P1.2=TXD

    P2OUT = 0x00u;
    P2DIR |= BIT1;  // set P1.1 as output (IN1) right forward
    P2DIR |= BIT2;  // set P1.2 as output (IN2) right reverse
    P2DIR |= BIT3;  // set P1.3 as output (IN3) left reverse
    P2DIR |= BIT4;  // set P1.4 as output (IN4) left forward

    /*** Set-up USCI A ***/
    UCA0CTL1 |= UCSSEL_2;                       // 1MHz 9600
    UCA0BR1 = 0;                            // 1MHz 9600
    UCA0MCTL = UCBRS0;                      // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;           // SMCLK
    UCA0BR0 = 104;                              // Initialize USCI state machine
    TA1CCR0 = 9999;
    //enable all timers
    TA1CCTL0 |= CCIE;                // enable CC interrupts
    TA1CCTL1 |= OUTMOD_4|CCIE;        // set TACCTL1 to Set / Reset mode//enable CC interrupts
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
    IE2 |= UCA0RXIE;                  // Enable USCI_A0 RX interrupt
    TA1CTL |= TASSEL_2 + MC_1;

    P2DIR |= BIT0 + BIT5;                // TA1CCR1 on P2.1
    P2SEL |= BIT0 + BIT5;                // TA1CCR1 on P2.1
    P2SEL &= ~(BIT0 +BIT5);

  __bis_SR_register(LPM0_bits + GIE);  // interrupts enabled
    while(1)
    {
        if(Rx_Data == 0xAA)
        {
            FWD();
        }
        else if(Rx_Data == 0xBB)
        {
            REV();
        }
        else if(Rx_Data ==0xCC)
        {
            Left();

        }

        else if(Rx_Data == 0xDD)
        {
            Right();

        }
        else
            Stop();


    }



}

void FWD()
{

    P2OUT |= BIT1;      // rt forw
    P2OUT &= ~BIT2;      // rt forw
    P2OUT &= ~BIT3;      // rt forw
    P2OUT |= BIT4;  // lft forw

    TA1CCR1 = 7000;

}

void REV()
{
P2OUT &= ~BIT1;      // rt forw
P2OUT |= BIT2;      // rt forw
P2OUT |= BIT3;      // rt forw
P2OUT &= ~BIT4;  // lft forw
TA1CCR1 = 7000;
}


void Left()
{
P2OUT &= ~BIT1;      // rt forw
P2OUT |= BIT2;      // rt forw
P2OUT &= ~BIT3;      // rt forw
P2OUT |= BIT4;  // lft forw
TA1CCR1 = 7000;
}

void Right()
{
P2OUT |= BIT1;      // rt forw
P2OUT &= ~BIT2;      // rt forw
P2OUT |= BIT3;      // rt forw
P2OUT &= ~BIT4;  // lft forw
TA1CCR1 = 7000;
}


void Stop()
{
P2OUT |= BIT1;      // rt forw
P2OUT |= BIT2;      // rt forw
P2OUT |= BIT3;      // rt forw
P2OUT |= BIT4;  // lft forw
TA1CCR1 = 7000;

}

#pragma vector=USCIAB0RX_VECTOR  // Receive UART interrupt
__interrupt void USCI0RX_ISR(void)
{
    Rx_Data = UCA0RXBUF; // Assign received byte to Rx_Data
    __bic_SR_register_on_exit(LPM0_bits);   // Wake-up CPU
}



#pragma vector = TIMER1_A0_VECTOR
__interrupt void captureCompareInt1 (void)
{
    P2OUT |= BIT5 + BIT0;                        //Turn on ENABLES
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void captureCompareInt2 (void)
{
    P2OUT &= ~(BIT5 + BIT0);                        //Turn off ENABLES
    TA1CCTL1 &= ~CCIFG;                //clear capture compare interrupt flag
}

