#include <stdlib.h>
#include "uart.h"
#include "string.h"

#ifdef USB
    #define UARTPORT P1
    #define EUSCIMOD EUSCI_A0
    #define EUSCI_IRQ EUSCIA0_IRQn
#else
    #define UARTPORT P3
    #define EUSCIMOD EUSCI_A2
    #define EUSCI_IRQ EUSCIA2_IRQn
#endif


void initSerial() {

    // init BRCLK
    /* Configure MCLK/SMCLK source to DCO, with DCO = 12MHz */
    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses

    /*
     * Initialize the queue state.
    */

    InBuffer = (char*) malloc(sizeof(char)*DEFAULT_BUFFER_SIZE); // Allocate input buffer.
    RXSize = DEFAULT_BUFFER_SIZE;
    RXLength = 0;
    RXIndex = 0;

    OutBuffer = (char*) malloc(sizeof(char)*DEFAULT_BUFFER_SIZE); //Allocate out (tx) buffer.
    TXSize = DEFAULT_BUFFER_SIZE;
    TXLength = 0;
    TXIndex = 0;

    /* Configure UART pins */
    UARTPORT->SEL0 |= BIT2 | BIT3;                // set 2-UART pins as secondary function
    UARTPORT->SEL1 &= ~(BIT2 | BIT3);

    /* Configure UART
     *  Asynchronous UART mode, 8O1 (8-bit data, odd parity, 1 stop bit),
     *  LSB first, SMCLK clock source
     */
    EUSCIMOD->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    //EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST ; // Remain eUSCI in reset
    // complete configuration of UART in eUSCI_A0 control register
    EUSCIMOD->CTLW0 |= EUSCI_A_CTLW0_PEN + EUSCI_A_CTLW0_PAR + EUSCI_A_CTLW0_SSEL__SMCLK;

    /* Baud Rate calculation
     * Refer to Section 24.3.10 of Technical Reference manual
     * BRCLK = 12000000, Baud rate = 38400 TODO Set baud rate to around 110kHz
     *
     * calculate N and determine values for UCBRx, UCBRFx, and UCBRSx
     *          values used in next two TODOs
     *
     * N = 12e6/38400
     */
    // set clock prescaler in eUSCI_AX baud rate control register
    EUSCIMOD->BRW = 19;

    //  configure baud clock modulation in eUSCI_A0 modulation control register
    EUSCIMOD->MCTLW |= (0x65<<EUSCI_A_MCTLW_BRS_OFS);
    EUSCIMOD->MCTLW |= (8<<EUSCI_A_MCTLW_BRF_OFS);
    EUSCIMOD->MCTLW |= BIT0;


    EUSCIMOD->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;    // Initialize eUSCI
    EUSCIMOD->IFG &= ~EUSCI_A_IFG_RXIFG;        // Clear eUSCI RX interrupt flag
    EUSCIMOD->IE |= EUSCI_A_IE_RXIE + EUSCI_A_IE_TXIE;            // Enable USCI_A0 RX interrupt

    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = (1 << EUSCI_IRQ);
}

// Only fills until truncated
int writeMessage(const char* message, int msgLength) {
    int idx;
    for(idx = 0; (idx < msgLength) && (idx < TXSize); idx++) {
        OutBuffer[(TXIndex + idx) % TXSize] = message[idx];
        if(message[idx] == '\0') break;
    }

    TXLength = TXLength + idx > TXSize ? TXSize : TXLength + idx;

    if(uartWaiting) {
        uartWaiting = 0;
        EUSCIMOD->TXBUF = OutBuffer[TXIndex];
        TXIndex = (TXIndex + 1) % TXSize;
        TXLength--;
    }
    return msgLength - idx;
}

/**
 * Get a null-terminated string of length N
 *
 * Also writes to serial buffer for user input purposes.
 *
 * char* buf: Input buffer to write to.
 * int length: length of buffer to fill to.
 *
 * returns number of filled chars.
 */
int getString(char* buf, int length) {
    int i = 0;
    for(; (i < length); i++) {
        while(RXLength == 0){__no_operation();} // Wait for length

        buf[i] = InBuffer[RXIndex];
        RXIndex = RXIndex + 1 % RXSize; // Walk up buffer.
        RXLength--;

        switch(InBuffer[RXIndex]) {
        case 8: //Handle Backspace
            break;
        case '\r':
            break;
        default://TODO Reimplement string

        }

        if(buf[i] == '\0') break;

    }
    i++;
    buf[i] = '\0';
    return i;
}

/**
 * Get an N-Length string of length N
 * Warning: Blocking
 *
 * char* buf: Input buffer to write to.
 * int length: length of buffer to fill to.
 *
 */
void fillBuffer(char* buf, int length) {
    int i = 0;
    for(; (i < length); i++) {
        while(RXLength == 0){__no_operation();} // Wait for length

        buf[i] = InBuffer[RXIndex];
        RXIndex = RXIndex + 1 % RXSize; // Walk up buffer.
        RXLength--;

    }
    i++;
}

/**
 * Blocking flushes transmit buffer.
 */
void flushSerial() {
    while(TXLength != 0) {
        __no_operation(); // Lazy operation.
    }
}

/**
 * Blocking writes a message to the serial port.
 */
int printMessage(const char* message, int msgLength) {
    int i = writeMessage(message, msgLength);
    flushSerial();
    return i;
}

// UART interrupt service routine
#ifdef USB
void EUSCIA0_IRQHandler(void)
#else
void EUSCIA2_IRQHandler(void)
#endif
{
    // Check if receive flag is set (value ready in RX buffer)
    if (EUSCIMOD->IFG & EUSCI_A_IFG_RXIFG)
    {
        // Note that reading RX buffer clears the flag
        int digit = EUSCIMOD->RXBUF;
        //int rxIdx = (RXIndex + RXTempLength) % RXSize;
        int rxIdx = (RXIndex) % RXSize;
        InBuffer[rxIdx] = (char) digit;

        RXLength++;


    }
    // Check if transmit flag is set (value ready in RX buffer)
    if (EUSCIMOD->IFG & EUSCI_A_IFG_TXIFG)
    {
        if(TXLength > 0) {
            EUSCIMOD->TXBUF = (OutBuffer[TXIndex] & EUSCI_A_TXBUF_TXBUF_MASK);
            TXIndex = (TXIndex + 1) % TXSize;
            TXLength--;
        } else {
            uartWaiting = 1;
            EUSCIMOD->IFG &= ~EUSCI_A_IFG_TXIFG;
        }
    }

}
