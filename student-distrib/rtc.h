/* rtc.h - RTC init and interrupt
 */

#ifndef _RTC_H
#define _RTC_H
#include "types.h"


#define A_turn_on 0x20 //control register A turn on osciallator
#define A_2_Hz 0x0F //control register A 2 Hz setting
#define A_4_Hz 0x0E //so on and so forth
#define A_8_Hz 0x0D
#define A_16_Hz 0x0C
#define A_32_Hz 0x0B
#define A_64_Hz 0x0A
#define A_128_Hz 0x09
#define A_256_Hz 0x08
#define A_512_Hz 0x07
#define A_1024_Hz 0x06//max frequency allowed 
//allowed frequency inputs
#define f_2_Hz 2
#define f_4_Hz 4
#define f_8_Hz 8
#define f_16_Hz 16
#define f_32_Hz 32
#define f_64_Hz 64
#define f_128_Hz 128
#define f_256_Hz 256
#define f_512_Hz 512
#define f_1024_Hz 1024

#define NUM_BYTES	4

extern void init_rtc(void);

extern void rtc_interrupt(void);

extern int32_t rtc_open(const uint8_t* filename);

extern int32_t rtc_close(int32_t fd);

extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
#endif
