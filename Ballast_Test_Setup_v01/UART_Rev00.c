
//******************************************************************************
//   MSP430F552x Demo - USCI_A0, 115200 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1048576hz = 1048576/115200 = ~9.1 (009h|01h)
//   ACLK = REFO = ~32768Hz, MCLK = SMCLK = default DCO = 32 x ACLK = 1048576Hz
//   See User Guide for baud rate divider table
//
//                 MSP430F552x
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |     P3.3/UCA0TXD|------------>
//            |                 | 115200 - 8N1
//            |     P3.4/UCA0RXD|<------------
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCSv4 and IAR Embedded Workbench Version: 4.21
//******************************************************************************
#include <msp430.h>
void uart_init (void){
    P3SEL |= BIT3+BIT4;                       // P3.3,4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    //UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
    //UCA0CTL0 = UCPEN + UCPAR;
    //UCA0CTL0 = UCMSB;
    /*
     * For fBRCLK=1MHz, BR=19200: N=1000000/19200 = 51,020408163265306122448979591837
     * UCBRx = INT(N) = 51
     * UCBRSx = round (0,020408163265306122448979591837 * 8) = round (0,16326530612244897959183673469388) = 0
     *
     */
    //UCA0BR0 = 9;                                  // 1MHz 115200 (see User's Guide)
    //UCA0BR0 = 54;                                 // 1MHz 19200 (see User's Guide) Baud rate divider with 1048576hz = 1048576/19200 =
    //UCA0BR1 = 0;                                  // 1MHz 115200
    UCA0BR0 = 0x03;                               // 32kHz/9600=3.41 (see User's Guide)
    //UCA0BR0 = 0x01;                                 // 32kHz/19200=1.71 (see User's Guide)
    UCA0BR1 = 0x00;                                 //
    //UCA0MCTL |= UCBRS_1 + UCBRF_0;                // Modulation UCBRSx=1, UCBRFx=0
    UCA0MCTL = UCBRS_3+UCBRF_0;                     // 9600 Modulation UCBRSx=3, UCBRFx=0  9600
    //UCA0MCTL = 0x00;                                // 19200
    UCA0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                               // Enable USCI_A0 RX interrupt

    //__bis_SR_register(LPM0_bits + GIE);           // Enter LPM0, interrupts enabled
    __bis_SR_register(GIE);                         // Enter LPM0, interrupts enabled
    __no_operation();
    return;
}

void uart_init_no_int (void){

    P3SEL |= BIT3+BIT4;                         // P3.3,4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                        // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                       // SMCLK
    UCA0BR0 = 9;                                // 1MHz 115200 (see User's Guide)
    //UCA0BR0 = 54;                             // 1MHz 19200 (see User's Guide) Baud rate divider with 1048576hz = 1048576/19200 =
    UCA0BR1 = 0;                                // 1MHz 115200
    UCA0MCTL |= UCBRS_1 + UCBRF_0;              // Modulation UCBRSx=1, UCBRFx=0
    //UCA0MCTL = 0xb5;
    UCA0CTL1 &= ~UCSWRST;                       // **Initialize USCI state machine**
    return;
}

void UART_TX(char * tx_data) // Define a function which accepts a character pointer to an array
{
    unsigned int i=0;
    while(tx_data[i]) // Increment through array, look for null pointer (0) at end of string
    {
        while ((UCA0STAT & UCBUSY)); // Wait if line TX/RX module is busy with data
        UCA0TXBUF = tx_data[i]; // Send out element i of tx_data array on UART bus
        i++; // Increment variable for array address
    }
    return;
}

void uart_init_19200_8MHz(void){
    P3SEL |= BIT3+BIT4;                             // P3.3,4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                            // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL__SMCLK;                      // SMCLK 8MHz
    UCA0BR0 = 0xB4;                                 //MSBs  436
    UCA0BR1 = 0x01;                                 //LSBs
    //UCA0MCTL = 0xDB;                              //
    UCA0MCTL = 0x00;
    UCA0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                               // Enable USCI_A0 RX interrupt

    //__bis_SR_register(LPM0_bits + GIE);           // Enter LPM0, interrupts enabled
    __bis_SR_register(GIE);                         // Enter LPM0, interrupts enabled
    __no_operation();
    return;
}

void uart_init_19200_8MHz_8E1(void){
    P3SEL |= BIT3+BIT4;                             // P3.3,4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                            // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL__SMCLK;                      // SMCLK 8MHz
    UCA0CTL0 = UCPEN + UCPAR;
    //UCA0CTL0 = UCMSB;
    UCA0BR0 = 0xB4;                                 //MSBs  436
    UCA0BR1 = 0x01;                                 //LSBs
    //UCA0MCTL = 0xDB;                              //
    UCA0MCTL = 0x00;
    UCA0CTL1 &= ~UCSWRST;                           // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                               // Enable USCI_A0 RX interrupt
    __no_operation();
    return;
}
