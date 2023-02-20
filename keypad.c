/*
 * keypad.c
 *
 *  Created on: 2023/02/13
 *      Author: ziemereg
 */
#include "msp.h"
#define ROWS 4
#define COLS 4
#define KEYPAD_PORT             P4

char pressedKey = '\0';
int rowPins[ROWS] = {1, 2, 3, 4};
int colPins[COLS] = {5, 6, 7, 8};

char keypad_values[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void initKeypad() {
    KEYPAD_PORT->SEL0 &= ~0xFF;
    KEYPAD_PORT->SEL1 &= ~0xFF;
    KEYPAD_PORT->DIR &= ~0xFF;
    KEYPAD_PORT->REN |= 0xFF;
    KEYPAD_PORT->OUT |= 0xFF;
}

char getKey() {
    int col;
    int row;
    for (col = 0; col < COLS; col++) {
        KEYPAD_PORT->DIR |= (1 << col);
        KEYPAD_PORT->OUT &= ~(1 << col);
        for (row = 0; row < ROWS; row++) {
            if(~(KEYPAD_PORT->IN & (1 << (row + COLS)))) {
                KEYPAD_PORT->DIR &= ~(1 << col);
                KEYPAD_PORT->OUT |= (1 << col);
                return keypad_values[row][col];
            }
        }
        KEYPAD_PORT->DIR &= ~(1 << col);
        KEYPAD_PORT->OUT |= (1 << col);
    }

    return ' '; // is there a better way to do this?
}
