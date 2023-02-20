/*
 * uart.h
 *
 *  Created on: Jan 31, 2023
 *      Author: armstrjj
 *
 *      Uses the USB Serial output port.
 *      An over-engineered serial port implementation.
 *      TX and RX buffers are a wrap-around array queue implementation.
 *
 */

#ifndef UART_H_
#define UART_H_
#include "msp.h"


#define DEFAULT_BUFFER_SIZE 256

static int uartWaiting = 1;

volatile char* OutBuffer; // To TX
unsigned short TXLength;
unsigned short TXSize;
unsigned short TXIndex;

volatile char* InBuffer; // To RX
volatile unsigned short RXLength;
volatile unsigned short RXSize;
volatile unsigned short RXIndex;
volatile unsigned short RXTempLength;

int writeMessage(const char* message, int msgLength);
int printMessage(const char* message, int msgLength);
int getString(char* buffer, int length);
void fillBuffer(char* buffer, int length);

void initSerial();

void flushSerial();

#endif /* UART_H_ */
