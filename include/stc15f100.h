#pragma once

#include <8051.h>
#include <compiler.h>

#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80

SFR(PSW,        0xD0);
    // SBIT(PSW_CY,    0xD0, 7);
    // SBIT(PSW_AC,    0xD0, 6);
    // SBIT(PSW_F0,    0xD0, 5);
    // SBIT(PSW_RS1,   0xD0, 4);
    // SBIT(PSW_RS0,   0xD0, 3);
    // SBIT(PSW_OV,    0xD0, 2);
    // SBIT(PSW_P,     0xD0, 0);
#define PSW_CY      BIT7
#define PSW_AC      BIT6
#define PSW_F0      BIT5
#define PSW_RS1     BIT4
#define PSW_RS0     BIT3
#define PSW_OV      BIT2
#define PSW_P       BIT0

SFR(WDT_CONR,   0xC1);
SFR(IAP_DATA,   0xC2);
SFR(IAP_ADDRH,  0xC3);
SFR(IAP_ADDRL,  0xC4);
SFR(IAP_CMD,    0xC5);
SFR(IAP_TRIG,   0xC6);
SFR(IAP_CONTR,  0xC7);

SFR(IP,         0xB8);
    // SBIT(IP_PLVD,   0xB8, 6);
    // SBIT(IP_PT1,    0xB8, 3);
    // SBIT(IP_PX1,    0xB8, 2);
    // SBIT(IP_PT0,    0xB8, 1);
    // SBIT(IP_PX0,    0xB8, 0);
SFR(IRC_CLKO,   0xBB);
#define IP_PLVD     BIT6
#define IP_PT1      BIT3
#define IP_PX1      BIT2
#define IP_PT0      BIT1
#define IP_PX0      BIT0

SFR(P3,         0xB0);
    // SBIT(P3_5,      0xB0, 5);
    // SBIT(P3_4,      0xB0, 4);
    // SBIT(P3_3,      0xB0, 3);
    // SBIT(P3_2,      0xB0, 2);
    // SBIT(P3_1,      0xB0, 1);
    // SBIT(P3_0,      0xB0, 0);
SFR(P3M1,       0xB1);
SFR(P3M0,       0xB2);
#define Pin5        BIT5
#define Pin4        BIT4
#define Pin3        BIT3
#define Pin2        BIT2
#define Pin1        BIT1
#define Pin0        BIT0

SFR(IE,         0xA8);
    // SBIT(IE_EA,     0xA8, 7);
    // SBIT(IE_ELVD,   0xA8, 6);
    // SBIT(IE_ET1,    0xA8, 3);
    // SBIT(IE_EX1,    0xA8, 2);
    // SBIT(IE_ET0,    0xA8, 1);
    // SBIT(IE_EX0,    0xA8, 0);
#define IE_EA       BIT7
#define IE_ELVD     BIT6
#define IE_ET1      BIT3
#define IE_EX1      BIT2
#define IE_ET0      BIT1
#define IE_EX0      BIT0

SFR(CLK_DIV,    0x97);

SFR(TCON,       0x88);
    // SBIT(TCON_TF1,  0x88, 7);
    // SBIT(TCON_TR1,  0x88, 6);
    // SBIT(TCON_TF0,  0x88, 5);
    // SBIT(TCON_TR0,  0x88, 4);
    // SBIT(TCON_IE1,  0x88, 3);
    // SBIT(TCON_IT1,  0x88, 2);
    // SBIT(TCON_IE0,  0x88, 1);
    // SBIT(TCON_IT0,  0x88, 0);
#define TCON_TF1    BIT7
#define TCON_TR1    BIT6
#define TCON_TF0    BIT5
#define TCON_TR0    BIT4
#define TCON_IE1    BIT3
#define TCON_IT1    BIT2
#define TCON_IE0    BIT1
#define TCON_IT0    BIT0

SFR(TMOD,       0x89);
#define TMOD_GATE1  BIT7
#define TMOD_CT1    BIT6
#define TMOD_M11    BIT5
#define TMOD_M01    BIT4
#define TMOD_GATE0  BIT3
#define TMOD_CT0    BIT2
#define TMOD_M10    BIT1
#define TMOD_M00    BIT0

SFR(TL0,        0x8A);
SFR(TL1,        0x8B);
SFR(TH0,        0x8C);
SFR(TH1,        0x8D);

SFR(AUXR,       0x8E);
#define AUXR_T0X12  BIT7
#define AUXR_T1X12  BIT6

SFR(INT_CLKO,   0x8F);
#define INTCLK_EX4     BIT6
#define INTCLK_EX3     BIT5
#define INTCLK_EX2     BIT4
#define INTCLK_T1CLKO  BIT1
#define INTCLK_T0CLKO  BIT0

SFR(SP,         0x81);
SFR(DPL,        0x82);
SFR(DPH,        0x83);

SFR(PCON,       0x87);
