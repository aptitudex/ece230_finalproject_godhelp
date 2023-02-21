#include "msp.h"
#include "uart.h"
#include "sha1.h"
#include "keypad.h"
#include "rgbLED.h"
#include "csLFXT.h"
//#include "interrupt.h"
//#include "sysctl.h"
#include <string.h>
//#include "cs.h"

//#define USB

/**
 * ATTRIBUTION
 *
 * sha1 clib written by Steve Reid
 * 'Never Gonna Give You Up' lyrics by Stock Aitken Waterman
 *
 */

// UART stuff:

// TODO: Delete??
#define INVALID_VALUE           -1
uint8_t digitsReceived = 0;
char recievedhash[20];
char otp[4];
/**
 * main.c
 */
SHA1_CTX sha;
char results[20];
char buf[19];
char* recbuf;
int n;
uint16_t seconds; uint16_t minutes; uint16_t hour; uint16_t dayofweek; uint16_t month; uint16_t day; uint16_t year;
char secondshigh; char secondslow;
char minhigh; char minlow;
char yearcenturyhigh; char yearcenturylow; char yeardecade; char yearlowest;
char hourhigh; char hourlow;
char dayhigh; char daylow;
char monthhigh; char monthlow;
char dow; // day of week

int count = 0;

// LE EPIC ENCRYPTION KEEEEEEEEEYYYYYYYYYYYYYYYYYYYY
char* key;
char* keyhash[20];

#define TRUE                    1
#define FALSE                   0
//
//const RTC_C_Calendar currentTime =
//{
//        0x00,
//        0x00,
//        0x00,
//        0x00,
//        0x00,
//        0x00
//};

uint8_t hashTime = FALSE;

void otpGen() {
    int i = 0;
    for (i = 0; i < 4; i++) {
        otp[i] = results[i] % 10 + 48;
    }
}

void configAESAccel(void) {
     key =   "[Intro]"
             "Desert you"
             "Ooh-ooh-ooh-ooh"
             "Hurt you"
             "\n"
             "[Verse 1]"
             "We're no strangers to love"
             "You know the rules and so do I"
             "A full commitment's what I'm thinking of"
             "You wouldn't get this from any other guy"
             "[Pre-Chorus]"
             "I just wanna tell you how I'm feeling"
             "Gotta make you understand"
             "\n"
             "[Chorus]"
             "Never gonna give you up"
             "Never gonna let you down"
             "Never gonna run around and desert you"
             "Never gonna make you cry"
             "Never gonna say goodbye"
             "Never gonna tell a lie and hurt you"
             "\n"
             "[Verse 2]"
             "We've known each other for so long"
             "Your heart's been aching, but you're too shy to say it"
             "Inside, we both know what's been going on"
             "We know the game, and we're gonna play it"
             "\n"
             "[Pre-Chorus]"
             "And if you ask me how I'm feeling"
             "Don't tell me you're too blind to see"
             "\n"
             "[Chorus]"
             "Never gonna give you up"
             "Never gonna let you down"
             "Never gonna run around and desert you"
             "Never gonna make you cry"
             "Never gonna say goodbye"
             "Never gonna tell a lie and hurt you"
             "Never gonna give you up"
             "Never gonna let you down"
             "Never gonna run around and desert you"
             "Never gonna make you cry"
             "Never gonna say goodbye"
             "Never gonna tell a lie and hurt you"
             "You might also like"
             "Whopper Whopper"
             "Burger King"
             "Did you know that there�fs a tunnel under Ocean Blvd"
             "Lana Del Rey"
             "Creepin�f"
             "Metro Boomin, The Weeknd & 21 Savage"
             "[Post-Chorus]"
             "Ooh (Give you up)"
             "Ooh-ooh (Give you up)"
             "Ooh-ooh"
             "Never gonna give, never gonna give (Give you up)"
             "Ooh-ooh"
             "Never gonna give, never gonna give (Give you up)"
             "\n"
             "[Bridge]"
             "We've known each other for so long"
             "Your heart's been aching, but you're too shy to say it"
             "Inside, we both know what's been going on"
             "We know the game, and we're gonna play it"
             "\n"
             "[Pre-Chorus]"
             "I just wanna tell you how I'm feeling"
             "Gotta make you understand"
             "\n"
             "[Chorus]"
             "Never gonna give you up"
             "Never gonna let you down"
             "Never gonna run around and desert you"
             "Never gonna make you cry"
             "Never gonna say goodbye"
             "Never gonna tell a lie and hurt you"
             "Never gonna give you up"
             "Never gonna let you down"
             "Never gonna run around and desert you"
             "Never gonna make you cry"
             "Never gonna say goodbye"
             "Never gonna tell a lie and hurt you"
             "Never gonna give you up"
             "Never gonna let you down"
             "Never gonna run around and desert you"
             "Never gonna make you cry"
             "Never gonna say goodbye"
             "Never gonna tell a lie and hurt you";
//     n = strlen(key);
//     SHA1Init(&sha);
//     SHA1Update(&sha, key, n);
//     SHA1Final(keyhash, &sha);

     AES256->CTL0 |= AES256_CTL0_KL0; // select 128 key length
     AES256->CTL0 |= AES256_CTL0_CM1; // select cbc mode


}

void configRTC(void) {

    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;
    BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS) = 0;
    BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;

    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;

    BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS) = 0;

    RTC_C->TIM0 = (20<<RTC_C_TIM0_MIN_OFS) | 42;
    RTC_C->TIM1 = (2<<RTC_C_TIM1_DOW_OFS) | 4;
    RTC_C->DATE = (4<<RTC_C_DATE_MON_OFS) | 20;
    RTC_C->YEAR = 1969;

    BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
    //    RTC_C->CTL0 |= RTC_C_CTL0_RDYIE; // ready interrupt enable
    //  real-time clock BCD select
    // RTC_C->CTL13 |= RTC_C_CTL13_BCD;



   configLFXT();
   configHFXT(); // smclk = 48Mhz/128

   TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK + TIMER_A_CTL_ID__8 + TIMER_A_CTL_IE;
   TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE;
   TIMER_A0->CCR[0] = 65535; // Repeat every 1 sec.
   //Purposely not turning on Timer a0 because we don't need it to check hash.
   NVIC_EnableIRQ(TA0_0_IRQn);

}


void doRTC() {
    while (!RTC_C_CTL0_RDYIFG) {__no_operation();}

    minutes = (RTC_C->TIM0 & RTC_C_TIM0_MIN_MASK)>>RTC_C_TIM0_MIN_OFS;
    seconds = RTC_C->TIM0 & RTC_C_TIM0_SEC_MASK;
    hour = RTC_C->TIM1 & RTC_C_TIM1_HOUR_MASK;
    day = RTC_C->DATE & RTC_C_DATE_DAY_MASK;
    month = (RTC_C->DATE & RTC_C_DATE_MON_MASK)>>RTC_C_DATE_MON_OFS;
    year = RTC_C->YEAR;


    n = sprintf(buf, "%02d,%02d,%02d,%02d,%02d,%02d", seconds, minutes, hour, day, month, year);
}

void hashRTC()
{
    doRTC();
    buf[1] = '0';
    SHA1Init(&sha);
    SHA1Update(&sha, (uint8_t *)buf, n);
    SHA1Final(results, &sha);
    otpGen();
}

void recieveHash() {
    fillBuffer(recbuf, 19);
}

void sendHash() {
    writeMessage(results, 20);
}


char request[7];
char reqotp[6];
char timereq[7] = "timerq";
char rqotp[7] = "reqotp";

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    configRTC(); // config RTC
    initSerial();
    initKeypad();
    RGBLED_init();
    RGBLED_setColor(PURPLE);

    __enable_interrupts();
   while (1) {
       fillBuffer(request, 6);
       int succ = 1;
       int i = 0;
       for(; (i < 7) && (succ); i++) {
           succ = request[i] == timereq[i];
       }
       if (succ) {
           RGBLED_setColor(CYAN);
           doRTC();
           printMessage(buf, n);
           continue;
       }
       i = 0;
       int test = 1;

       for(; (i < 7) && (test); i++) {
           test = request[i] == rqotp[i];
       }
       if (test) {
           RGBLED_setColor(WHITE);
           break;
       }

   }

   // HashRTC call (sub-calls OTPgen)
   hashRTC();
   RGBLED_setColor(YELLOW);
   int i = 0;
   TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
   while (1) {
       char curKeyInput = getKey();
       if (i < 4) {
           if(otp[i] == curKeyInput) {
               RGBLED_setColor(WHITE);
               i++;
           } else if ((curKeyInput != ' ') && (i != 0)) {
               RGBLED_setColor(RED);
               i = 0;
           }
       } else if (i == 4) {
          break;
       }
       int k = 0;
       for (k = 0; k < 200000; k++);

   }
   TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
   char authmsg[6] = "auth\n";
   printMessage(authmsg, 6);
   RGBLED_setColor(GREEN);

   char reccolor[2];
   while(1) {
       fillBuffer(reccolor, 1);
       if (reccolor[0] == '1') {
           RGBLED_setColor(RED);
       } else if (reccolor[0] == '2') {
           RGBLED_setColor(GREEN);
       } else if (reccolor[0] == '3') {
           RGBLED_setColor(YELLOW);
       } else if (reccolor[0] == '4') {
           RGBLED_setColor(BLUE);
       } else if (reccolor[0] == '5') {
           RGBLED_setColor(PURPLE);
       } else if (reccolor[0] == '6') {
           RGBLED_setColor(CYAN);
       } else {
           RGBLED_setColor(WHITE);
       }
   }

   while (1) {
       // don't touch this
       __no_operation();
   }
}


void TA0_0_IRQHandler(void) {
    if(count == 0){
        hashRTC();
    }
    count = (count + 1) % 5;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

}
