/*
 * keypad.h
 *
 *  Created on: 2023/02/14
 *      Author: ziemereg
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#define ROWS 4
#define COLS 4
#define KEYPAD_PORT P4

void initKeypad();
char getKey();



#endif /* KEYPAD_H_ */
