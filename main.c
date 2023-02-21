#include "msp.h"
#include "uart.h"
#include "sha1.h"
#include "keypad.h"
#include "rgbLED.h"
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
char buf[23];
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
     n = strlen(key);
     SHA1Init(&sha);
     SHA1Update(&sha, key, n);
     SHA1Final(keyhash, &sha);
}

void configRTC(void) {
//   //  real-time clock BCD select
//    RTC_C->CTL13 |= RTC_C_CTL13_BCD;
//    //RTC_C->CTL13 |= RTC_C_CTL13_CALF0; //for cal if necessary later
//    // real-time clock ready interrupt enable
//    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;
//    RTC_C->CTL13 &= ~RTC_C_CTL13_HOLD;
//    //RTC_C->CTL0 &= ~RTC_C_KEY;

    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;
    BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS) = 0;
    BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;

    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;

    BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS) = 1;

    RTC_C->TIM0 = (20<<RTC_C_TIM0_MIN_OFS) | 42;
    RTC_C->TIM1 = (2<<RTC_C_TIM1_DOW_OFS) | 4;
    RTC_C->DATE = (4<<RTC_C_DATE_MON_OFS) | 20;
    RTC_C->YEAR = 1969;

    BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
    //    RTC_C->CTL0 |= RTC_C_CTL0_RDYIE; // ready interrupt enable
    //  real-time clock BCD select
    // RTC_C->CTL13 |= RTC_C_CTL13_BCD;

    ROM_CS_startLFXT(0);
}


void doRTC() {
    // EVERYTHING IN BCD FORMAT
    // assign to vars
    // TODO: Uhhh... it's possible some of these are flipped around... hope not, though.
    while (!RTC_C_CTL0_RDYIFG) {__no_operation();}

//    seconds = RTC_C_BCD -> TIM0 & 0b11111111;
//    //round to nearest 10
//    //seconds = log10(seconds);
//    float secondsfloat = pow(10, seconds);
//    seconds = ceil(seconds/secondsfloat)*secondsfloat;
//    minutes = RTC_C_BCD -> TIM0 & 0b1111111100000000;
//    minutes = minutes >> 8;
//    dayofweek = RTC_C_BCD -> TIM1 & 0b111;
//    hour = RTC_C_BCD -> TIM1 & 0b1111111100000000;
//    hour = hour >> 8;
//    day = RTC_C_BCD -> DATE & 0b11111111;
//    month = RTC_C_BCD -> DATE & 0b1111111100000000;
//    month = month >> 8;
//    year = RTC_C_BCD -> YEAR;
//
//    // reformat
//    secondshigh = seconds >> 4;
//    secondslow = seconds & 0b1111;
//
//    minhigh = minutes >> 4;
//    minlow = minutes & 0b1111;
//
//    hourhigh = hour >> 4;
//    hourlow = hour & 0b1111;
//
//    dayhigh = day >> 4;
//    daylow = day & 0b1111;
//    monthhigh = day >> 4;
//    monthlow = day & 0b1111;
//
//    yearcenturyhigh = year >> 12;
//    yearcenturylow = (year >> 8) & 0b1111;
//    yeardecade = (year >> 4) & 0b1111;
//    yearlowest = year & 0b1111;
    minutes = (RTC_C->TIM0 & RTC_C_TIM0_MIN_MASK)>>RTC_C_TIM0_MIN_OFS;
    seconds = RTC_C->TIM0 & RTC_C_TIM0_SEC_MASK;
    hour = RTC_C->TIM1 & RTC_C_TIM1_HOUR_MASK;
    day = RTC_C->DATE & RTC_C_DATE_DAY_MASK;
    month = (RTC_C->DATE & RTC_C_DATE_MON_MASK)>>RTC_C_DATE_MON_OFS;
    year = RTC_C->YEAR;


    n = sprintf(buf, "%02d,%02d,%02d,%02d,%02d,%02d", minutes, seconds, hour, day, month, year);
}

void hashRTC()
{
    doRTC();
    // seconds,minutes,hours,dow,day,mon,year
    //n = sprintf(buf, "%02d%02d,%02d%02d,%02d%02d,%02d%02d,%02d%02d,%02d%02d%02d%02d", secondshigh, "0", minhigh, minlow, hourhigh, hourlow, dayhigh, daylow, monthhigh, monthlow, yearcenturyhigh, yearcenturylow, yeardecade, yearlowest) + 1;
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
   char authmsg[6] = "auth\n";
   printMessage(authmsg, 6);
   RGBLED_setColor(GREEN);

   while (1) {
       // don't touch this
       __no_operation();
   }
}
//
//void RTCRDY_IRQHandler(void)
//{
//    if (hashTime == TRUE) {
//        hashRTC();
//    }
//}


//void TA0_0_IRQHandler(void) {

//}
