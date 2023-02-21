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

//char keypad_values[ROWS][COLS] = {
//    {'1', '2', '3', 'A'},
//    {'4', '5', '6', 'B'},
//    {'7', '8', '9', 'C'},
//    {'*', '0', '#', 'D'}
//};
char keypad_values[16] = {
    'D', 'C', 'B', 'A', '#', '9', '6', '3', '0', '8','5','2','*','7','4','1'
};

void initKeypad() {
//    KEYPAD_PORT->SEL0 &= ~0xFF;
//    KEYPAD_PORT->SEL1 &= ~0xFF;
//    KEYPAD_PORT->DIR &= ~0xFF;
//    KEYPAD_PORT->REN |= 0xFF;
//    KEYPAD_PORT->OUT |= 0xFF;
    KEYPAD_PORT->DIR = 0;
    KEYPAD_PORT->REN = 0xF0;
    KEYPAD_PORT->OUT = 0xF0;
}

char getKey() {
//    int col;
//    int row;
//    for (col = 0; col < COLS; col++) {
//        KEYPAD_PORT->DIR |= (1 << col);
//        KEYPAD_PORT->OUT &= ~(1 << col);
//        for (row = 0; row < ROWS; row++) {
//            if(~(KEYPAD_PORT->IN & (1 << (row + COLS)))) {
//                KEYPAD_PORT->DIR &= ~(1 << col);
//                KEYPAD_PORT->OUT |= (1 << col);
//                return keypad_values[row][col];
//            }
//        }
//        KEYPAD_PORT->DIR &= ~(1 << col);
//        KEYPAD_PORT->OUT |= (1 << col);
//    }
//
//    return ' '; // is there a better way to do this?
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08}; /* one row is active */

    /* check to see any key pressed */
    KEYPAD_PORT->DIR |= 0x0F;            /* make all row pins output */
    KEYPAD_PORT->OUT &= ~0x0F;           /* drive all row pins low */
    col = KEYPAD_PORT->IN & 0xF0;        /* read all column pins */
    KEYPAD_PORT->OUT |= 0x0F;            /* drive all rows high before disable them */
    KEYPAD_PORT->DIR &= ~0x0F;           /* disable all row pins drive */
    if (col == 0xF0)            /* if all columns are high */
        return ' ';               /* no key pressed */

    /* If a key is pressed, it gets here to find out which key.
     * It activates one row at a time and read the input to see
     * which column is active. */
    for (row = 0; row < 4; row++) {
        KEYPAD_PORT->DIR &= 0x0F;                /* disable all rows */
        KEYPAD_PORT->DIR |= row_select[row];     /* enable one row at a time */
        KEYPAD_PORT->OUT &= ~row_select[row];    /* drive the active row low */
        col = KEYPAD_PORT->IN & 0xF0;            /* read all columns */
        KEYPAD_PORT->OUT |= row_select[row];     /* drive the active row high */
        if (col != 0xF0) break;         /* if one of the input is low, some key is pressed. */
    }
    KEYPAD_PORT->OUT |= 0x0F;                    /* drive all rows high before disable them */
    KEYPAD_PORT->DIR &= 0x0F;                    /* disable all rows */
    if (row == 4)
        return ' ';                       /* if we get here, no key is pressed */

    /* gets here when one of the rows has key pressed, check which column it is */
//    if (col == 0xE0) return keypad_values[0][row];
//    if (col == 0xE0) return keypad_values[1][row];
//    if (col == 0xE0) return keypad_values[2][row];
//    if (col == 0xE0) return keypad_values[3][row];
    int result = 0;
    if (col == 0xE0) result = row * 4 + 1;    /* key in column 0 */
    if (col == 0xD0) result = row * 4 + 2;    /* key in column 1 */
    if (col == 0xB0) result = row * 4 + 3;    /* key in column 2 */
    if (col == 0x70) result = row * 4 + 4;    /* key in column 3 */

    return keypad_values[result-1];   /* just to be safe */
}
