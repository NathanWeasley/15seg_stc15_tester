#pragma once

#include "stc15f100.h"
#include "stdint.h"

// #define DELAY_ON_FIRST

#define INS_PER_CLK (1)
#define SYSCLK      (22118400UL)
#define BAUDRATE    (9600UL)
#define CLKDIV      (SYSCLK/BAUDRATE/INS_PER_CLK)
#define BAUD        (65536L - CLKDIV)
#define BAUD_H      ((uint8_t)(BAUD >> 8))
#define BAUD_L      ((uint8_t)BAUD)
#ifdef DELAY_ON_FIRST
#define CLKDIV_1ST  (CLKDIV*3/2)
#define BAUD_H_1ST  ((uint8_t)((65536L-CLKDIV_1ST) >> 8))
#define BAUD_L_1ST  ((uint8_t)(65536L-CLKDIV_1ST))
#else
#define CLKDIV_1ST  BAUD
#define BAUD_H_1ST  BAUD_H
#define BAUD_L_1ST  BAUD_L
#endif

extern uint8_t Tstatus;

void serial_init(void);

void serial_receive_begin(void);
void serial_receive_end(void);

void serial_proc_irq(void);

void serial_on_byte_receive(void (*proc)(uint8_t byte));
