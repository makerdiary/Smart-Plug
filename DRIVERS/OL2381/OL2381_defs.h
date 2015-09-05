// -----------------------------------------------------------------------------
//
// OL2381_defs.h
//
// -----------------------------------------------------------------------------
//  (c) NXP B.V. 2010. All rights reserved.
//  Disclaimer
//  1.   The NXP Software/Source Code is provided to Licensee "AS IS"
//  without any warranties of any kind. NXP makes no warranties to Licensee
//  and shall not indemnify Licensee or hold it harmless for any reason
//  related to the NXP Software/Source Code or otherwise be liable to the
//  NXP customer. The NXP customer acknowledges and agrees that the NXP
//  Software/Source Code is provided AS-IS and accepts all risks of
//  utilizing the NXP Software under the conditions set forth according to
//  this disclaimer.
//
//  2.   NXP EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
//  OF INTELLECTUAL PROPERTY RIGHTS. NXP SHALL HAVE NO LIABILITY TO THE NXP
//  CUSTOMER, OR ITS SUBSIDIARIES, AFFILIATES, OR ANY OTHER THIRD PARTY FOR
//  ANY DAMAGES, INCLUDING WITHOUT LIMITATION, DAMAGES RESULTING OR ALLEGDED
//  TO HAVE RESULTED FROM ANY DEFECT, ERROR OR OMMISSION IN THE NXP
//  SOFTWARE/SOURCE CODE, THIRD PARTY APPLICATION SOFTWARE AND/OR
//  DOCUMENTATION, OR AS A RESULT OF ANY INFRINGEMENT OF ANY INTELLECTUAL
//  PROPERTY RIGHT OF ANY THIRD PARTY. IN NO EVENT SHALL NXP BE LIABLE FOR
//  ANY INCIDENTAL, INDIRECT, SPECIAL, EXEMPLARY, PUNITIVE, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING LOST PROFITS) SUFFERED BY NXP CUSTOMER OR ITS
//  SUBSIDIARIES, AFFILIATES, OR ANY OTHER THIRD PARTY ARISING OUT OF OR
//  RELATED TO THE NXP SOFTWARE/SOURCE CODE EVEN IF NXP HAS BEEN ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGES.
// -----------------------------------------------------------------------------

#ifndef _OL2381_DEFS_H_
#define _OL2381_DEFS_H_

#include "rf_cfg.h"

#define OL2381_BASE_ADDR  0x00

// registers accessible via bank 0 and bank 1
#define OL2381_FC0L_REG         (OL2381_BASE_ADDR + 0x00)
#define OL2381_FC0M_REG         (OL2381_BASE_ADDR + 0x01)
#define OL2381_FC0H_REG         (OL2381_BASE_ADDR + 0x02)
#define OL2381_FC1L_REG         (OL2381_BASE_ADDR + 0x03)
#define OL2381_FC1M_REG         (OL2381_BASE_ADDR + 0x04)
#define OL2381_FC1H_REG         (OL2381_BASE_ADDR + 0x05)
#define OL2381_FC2L_REG         (OL2381_BASE_ADDR + 0x06)
#define OL2381_FC2M_REG         (OL2381_BASE_ADDR + 0x07)
#define OL2381_FC2H_REG         (OL2381_BASE_ADDR + 0x08)
#define OL2381_FC3L_REG         (OL2381_BASE_ADDR + 0x09)
#define OL2381_FC3M_REG         (OL2381_BASE_ADDR + 0x0A)
#define OL2381_FC3H_REG         (OL2381_BASE_ADDR + 0x0B)
#define OL2381_VCOCON_REG       (OL2381_BASE_ADDR + 0x0C)
#define OL2381_LOCON_REG        (OL2381_BASE_ADDR + 0x0D)
#define OL2381_TIMING0_REG      (OL2381_BASE_ADDR + 0x0E)
#define OL2381_TIMING1_REG      (OL2381_BASE_ADDR + 0x0F)
#define OL2381_PORTCON0_REG     (OL2381_BASE_ADDR + 0x10)
#define OL2381_PORTCON1_REG     (OL2381_BASE_ADDR + 0x11)
#define OL2381_PORTCON2_REG     (OL2381_BASE_ADDR + 0x12)
#define OL2381_PWRMODE_REG      (OL2381_BASE_ADDR + 0x13)
#define OL2381_IEN_REG          (OL2381_BASE_ADDR + 0x14)
#define OL2381_IFLAG_REG        (OL2381_BASE_ADDR + 0x15)
#define OL2381_POLLWUPTIME_REG  (OL2381_BASE_ADDR + 0x16)
#define OL2381_POLLACTION_REG   (OL2381_BASE_ADDR + 0x17)
#define OL2381_CLOCKCON_REG     (OL2381_BASE_ADDR + 0x18)
#define OL2381_DEVSTATUS_REG    (OL2381_BASE_ADDR + 0x19)
#define OL2381_FDEV_REG         (OL2381_BASE_ADDR + 0x1A)
#define OL2381_FRMP_REG         (OL2381_BASE_ADDR + 0x1B)
#define OL2381_ACON0_REG        (OL2381_BASE_ADDR + 0x1C)
#define OL2381_ACON1_REG        (OL2381_BASE_ADDR + 0x1D)
#define OL2381_ACON2_REG        (OL2381_BASE_ADDR + 0x1E)
#define OL2381_ARMP_REG         (OL2381_BASE_ADDR + 0x1F)
#define OL2381_TXCON_REG        (OL2381_BASE_ADDR + 0x20)
#define OL2381_RXGAIN_REG       (OL2381_BASE_ADDR + 0x21)
#define OL2381_RXBW_REG         (OL2381_BASE_ADDR + 0x22)
#define OL2381_GAINSTEP_REG     (OL2381_BASE_ADDR + 0x23)
#define OL2381_HIGAINLIM_REG    (OL2381_BASE_ADDR + 0x24)
#define OL2381_UPPERRSSITH_REG  (OL2381_BASE_ADDR + 0x25)
#define OL2381_LOWERRSSITH_REG  (OL2381_BASE_ADDR + 0x26)
#define OL2381_RXBBCON_REG      (OL2381_BASE_ADDR + 0x27)
#define OL2381_UMODAMPTH_REG    (OL2381_BASE_ADDR + 0x28)
#define OL2381_LMODAMPTH_REG    (OL2381_BASE_ADDR + 0x29)
#define OL2381_EMODAMPTH_REG    (OL2381_BASE_ADDR + 0x2A)
#define OL2381_RXDCON0_REG      (OL2381_BASE_ADDR + 0x2B)
#define OL2381_RXDCON1_REG      (OL2381_BASE_ADDR + 0x2C)
#define OL2381_RXDCON2_REG      (OL2381_BASE_ADDR + 0x2D)
#define OL2381_BANKSEL_REG      (OL2381_BASE_ADDR + 0x3F)

// registers accessible via bank 0 only
#define OL2381_SIGMON0_REG      (OL2381_BASE_ADDR + 0x2E)
#define OL2381_SIGMON1_REG      (OL2381_BASE_ADDR + 0x2F)
#define OL2381_SIGMON2_REG      (OL2381_BASE_ADDR + 0x30)
#define OL2381_WUPSTO_REG       (OL2381_BASE_ADDR + 0x31)
#define OL2381_SLICERINITL_REG  (OL2381_BASE_ADDR + 0x32)
#define OL2381_SLICERINITH_REG  (OL2381_BASE_ADDR + 0x33)
#define OL2381_TIMINGCHK_REG    (OL2381_BASE_ADDR + 0x34)
#define OL2381_RXCON_REG        (OL2381_BASE_ADDR + 0x35)
#define OL2381_RXFOLLOWUP_REG   (OL2381_BASE_ADDR + 0x36)
#define OL2381_SIGMONSTATUS_REG (OL2381_BASE_ADDR + 0x37)
#define OL2381_SIGMONERROR_REG  (OL2381_BASE_ADDR + 0x38)
#define OL2381_RSSILEVEL_REG    (OL2381_BASE_ADDR + 0x39)
#define OL2381_PREACON_REG      (OL2381_BASE_ADDR + 0x3A)
#define OL2381_PREA0_REG        (OL2381_BASE_ADDR + 0x3B)
#define OL2381_PREA1_REG        (OL2381_BASE_ADDR + 0x3C)
#define OL2381_PREA2_REG        (OL2381_BASE_ADDR + 0x3D)
#define OL2381_PREA3_REG        (OL2381_BASE_ADDR + 0x3E)

// registers accessible via bank 1 only
#define OL2381_EXTRXSTATUS_REG  (OL2381_BASE_ADDR + 0x2E + 0x40)
#define OL2381_CFRCCAL_REG      (OL2381_BASE_ADDR + 0x2F + 0x40)
#define OL2381_CFIQCAL_REG      (OL2381_BASE_ADDR + 0x30 + 0x40)
#define OL2381_EXPERT0_REG      (OL2381_BASE_ADDR + 0x31 + 0x40)
#define OL2381_EXPERT1_REG      (OL2381_BASE_ADDR + 0x32 + 0x40)
#define OL2381_EXPERT2_REG      (OL2381_BASE_ADDR + 0x33 + 0x40)
#define OL2381_EXPERT3_REG      (OL2381_BASE_ADDR + 0x34 + 0x40)
#define OL2381_TEST0_REG        (OL2381_BASE_ADDR + 0x35 + 0x40)
#define OL2381_TEST1_REG        (OL2381_BASE_ADDR + 0x36 + 0x40)
#define OL2381_TEST2_REG        (OL2381_BASE_ADDR + 0x37 + 0x40)
#define OL2381_TEST3_REG        (OL2381_BASE_ADDR + 0x38 + 0x40)
#define OL2381_TEST4_REG        (OL2381_BASE_ADDR + 0x39 + 0x40)
#define OL2381_TEST5_REG        (OL2381_BASE_ADDR + 0x3A + 0x40)
#define OL2381_RFU_REG          (OL2381_BASE_ADDR + 0x3B + 0x40)
#define OL2381_TCBEN0_REG       (OL2381_BASE_ADDR + 0x3C + 0x40)
#define OL2381_TCBR_REG         (OL2381_BASE_ADDR + 0x3D + 0x40)
#define OL2381_TCBEN1_REG       (OL2381_BASE_ADDR + 0x3E + 0x40)

// the next defines can be used for the IEN and IFLAG registers of OL2381
#define IRQ_POR                  0x01
#define IRQ_BROWNOUT             0x02
#define IRQ_WATCHDOG             0x04
#define IRQ_POLLTIM              0x08
#define IRQ_WUPS                 0x10
#define IRQ_PREA                 0x20
#define IRQ_EOF                  0x40
#define IRQ_TXRX_RDY             0x80

#define OL2381_ASK_UMODAMPTH_VAL 0x9C
#define OL2381_ASK_LMODAMPTH_VAL 0x88
#define OL2381_ASK_EMODAMPTH_VAL 0x88

#define OL2381_FSK_UMODAMPTH_VAL 0xBC
#define OL2381_FSK_LMODAMPTH_VAL 0xA8
#define OL2381_FSK_EMODAMPTH_VAL 0xA8


/*----------------------------------------------------------------------------*/
/* Variables declaration                                                      */
/*----------------------------------------------------------------------------*/

// OL2381 configuration values (16.00 MHz XTAL freq)
static volatile char sfr_buffer_bank0[64] =
{
    ((RF_CENTER_FREQ_1 >>  0) & 0xFF),   // FC0L
    ((RF_CENTER_FREQ_1 >>  8) & 0xFF),   // FC0M
    ((RF_CENTER_FREQ_1 >> 16) & 0xFF),   // FC0H
    ((RF_CENTER_FREQ_2 >>  0) & 0xFF),   // FC1L
    ((RF_CENTER_FREQ_2 >>  8) & 0xFF),   // FC1M
    ((RF_CENTER_FREQ_2 >> 16) & 0xFF),   // FC1H
    ((RF_CENTER_FREQ_3 >>  0) & 0xFF),   // FC2L
    ((RF_CENTER_FREQ_3 >>  8) & 0xFF),   // FC2M
    ((RF_CENTER_FREQ_3 >> 16) & 0xFF),   // FC2H
    ((RF_CENTER_FREQ_4 >>  0) & 0xFF),   // FC3L
    ((RF_CENTER_FREQ_4 >>  8) & 0xFF),   // FC3M
    ((RF_CENTER_FREQ_4 >> 16) & 0xFF),   // FC3H
    0x00,   // VCOCON
    0x00,   // LOCON
               /* bit2   LOCK_DET_ON=0b, only monitor PLL after VCO calibration */
               /* bit1   VCO_BAND=0b,  1 for freq. bands below 400MHz, 0 for higher freq. */
               /* bit0   RF_LO_DIV=0b, 1 for 315;434MHz and 0 for 868;915MHz */
    0x63,   // TIMING0
               /* bit7-0 MAINSCL=99, will be set during OL2381_tx_setup */
    0x08,   // TIMING1
               /* bit7-6 WTCHDOG=00b */
               /* bit5-3 PRESC=001b, will be set during OL2381_tx_setup */
               /* bit2-0 MAINSCH=000b, will be set during OL2381_tx_setup */
    0x28,   // PORTCON0
               /* bit7-4 P11C=0010b, P11/INT is used as interrupt request line */
               /* bit3   P11INV=1b,  P11/INT signal inverted */
               /* bit2-1 P10C=00b,   not relevant */
               /* bit0   P10INV=0b,  P10/DATA signal not inverted */
    0x00,   // PORTCON1
               /* bit7-5 P13C=000b, P13/SDO tristate */
               /* bit4   P13INV=0b, P13/SDO signal not inverted */
               /* bit3-1 P12C=000b, P12/CLOCK tristate */
               /* bit0   P12INV=0b, P12/CLOCK signal not inverted */
    0x06,   // PORTCON2
               /* bit7   SEP_SDO=0b,      P13 not used for SPI */
               /* bit6   SEP_RX_OUT=0b,   SDIO is used for RX data in receive mode */
               /* bit5   SEP_TX_LINES=0b, SDIO is used for TX data in transmit mode */
               /* bit4-3 RFU=00b,         reserved for future use */
               /* bit2-1 P14C=11b,        P14/PIND is 0 in RX mode, 1 in TX mode, else 0 */
               /* bit0   P14INV=0b,       P14/PIND signal not inverted */
    0x00,   // PWRMODE
               /* bit7-5 always 000b */
               /* bit4   POLLTIM_EN=0b (1=poll-timer enabled, 0=poll-timer disabled) */
               /* bit3-2 DEV_MODE=00b (idle) */
               /* bit1   PD=0b (set to 1 to power down) */
               /* bit0   RESET=0b (set to 1 to reset) */
    0x00,   // IEN
               /* bit7-1 0000000b (all interrupts disabled) */
               /* bit0   always 0b */
    0x00,   // IFLAG
               /* bit7-0 00000000b (read-only register) */
    0xFF,   // POLLWUPTIME
               /* bit7-0 POLLWUPTIME=255 (no polling enabled, see PWRMODE.POLLTIM_EN) */
    0xC7,   // POLLACTION
               /* bit7-6 POLL_MODE=11b (fully powered, auto generate rx command) */
               /* bit5-4 RX_FREQ=00b (select frequency regs FCx; corresponds to bits AB of rx command) */
               /* bit3   RX_CMD=0b (0=WUPS, 1=preamble-detect) */
               /* bit2-1 RX-GAIN=11b (high gain) */
               /* bit0   SET_RX_FLAGS=1b (1=use the settings from POLLACTION, 0=use current settings) */
    0x00,   // CLOCKCON
               /* bit7   MAN-PTCAL=0b */
               /* bit6   read only */
               /* bit5   EXT-POLL=0b */
               /* bit4-2 CLKSRCSEL=000b, not used */
               /* bit1   EXT_CLK_BUF_EN=0b */
               /* bit0   XODIS=0b */
    0x00,   // DEVSTATUS
               /* bit7-0 00000000b (read-only register) */
//
// FSK deviation freq = (+/-) Fref * (FDEV / 65536)
//    or
// FDEV = (FSKdev * 65536) / Fref
// FDEV = (50 kHz * 65536) / 16000 kHz = 204.8 = 204 = 26 * 8 = 26 * 2^3
// so FDEV_EXP = 3 and FDEV_MANT = 26 or 0x1A or 11010b
//
    0x7A,   // FDEV
               /* bit7-5 FDEV_EXP=011b */
               /* bit4-0 FDEV_MANT=11010b */
    0x04,   // FRMP
               /* Relative ramp duration 10.6 % of chip */
    0x9F,   // ACON0
               /* bit7   ASK0=1b, (1=ASK mode, 0=FSK mode) */
               /* bit6   always 0b */
               /* bit5   AMH0X=0b, only used in test mode */
               /* bit4-0 AMH0=11111b, set maximum output power level for this PAM setting */
    0x1F,   // ACON1
			   /* bit7   ASK1=0b, (1=ASK mode, 0=FSK mode) */
			   /* bit6-5 always 00b */
			   /* bit4-0 AMH1=11111b, set maximum output power level for this PAM setting */
    0x00,   // ACON2
			   /* bit7-5 always 000b */
			   /* bit4-0 AML=00000b, min power */
    0x05,   // ARMP
               /* bit7   always 0b */
               /* bit6-5 ARMP_EXP=00b */
               /* bit4-0 ARMP_MANT=00101b */
    0x00,   // TXCON
               /* bit7   DOUBLE_SD_RESULT=0b, */
               /* bit6   INV_TX_DATA=0b, will be set during OL2381_tx_setup */
               /* bit5   TXCLKSEL=0b, will be set during OL2381_tx_setup */
               /* bit4-3 TXCLKOUTSEL=00b, P12/CLOCK is CLKtx (baudrate) */
               /* bit2   always 0b */
               /* bit1-0 PAM=00b, recommended value is 00b*/
    0xB1,   // RXGAIN
               /* bit7-4 RX_HI_GAIN=1011b */
               /* bit3-0 RX_LOW_GAIN=0001b */
    0x12,   // RXBW
               /* bit7   DEMOD_ASK=0b, will be set during OL2381_rx_setup */
               /* bit6-4 CF_BW=001b, bandwidth of channel filter is 200kHz */
               /* bit3-0 RSSI_FILTER_FC=0010b, */
    0x15,   // GAINSTEP
               /* bit7   always 0b */
               /* bit6-0 RSSI_GAIN_STEP_ADJ=0010101b */
    0xC8,   // HIGAINLIM
               /* bit7-0 HI_GAIN_LIMIT=0xC8 */
    0xFF,   // UPPERRSSITH
               /* bit7-0 UPPER_RSSI_TH=0xFF */
    0x00,   // LOWERRSSITH
               /* bit7-0 LOWER_RSSI_TH=0x00 */
    0x10,   // RXBBCON
               /* bit7-6 DEGLITCHER_WINDOW_LEN=00b */
               /* bit5-4 BASBAND_SETTL_TIME=01b */
               /* bit3-0 BASEBAND_FILTER_FC=0000b, 115.45kHz */
    OL2381_FSK_UMODAMPTH_VAL,   // UMODAMPTH
               /* bit7-4 UPPER_MODAMP_TH_EXP=0000b, will be set during OL2381_rx_setup */
               /* bit3-0 UPPER_MODAMP_TH_MANT=0000b, will be set during OL2381_rx_setup */
    OL2381_FSK_LMODAMPTH_VAL,   // LMODAMPTH
               /* bit7-4 LOWER_MODAMP_TH_EXP=0000b, will be set during OL2381_rx_setup */
               /* bit3-0 LOWER_MODAMP_TH_MANT=0000b, will be set during OL2381_rx_setup */
    OL2381_FSK_EMODAMPTH_VAL,   // EMODAMPTH
               /* bit7-4 EDGE_MODAMP_TH_EXP=0000b, will be set during OL2381_rx_setup */
               /* bit3-0 EDGE_MODAMP_TH_MANT=0000b, will be set during OL2381_rx_setup */
    0xD4,   // RXDCON0
               /* bit7-6 NUM_MODAMP_GAPS_W=11b */
               /* bit5-4 SLICERSEL_W=01b */
               /* bit3-2 SLICERINITSEL_W=01b */
               /* bit1-0 INIT_ACQ_BITS_W=00b */
    0xF8,   // RXDCON1
               /* bit7-6 NUM_MODAMP_GAPS_P=11b */
               /* bit5-4 SLICERSEL_P=11b */
               /* bit3-2 SLICERINITSEL_PD=10b */
               /* bit1-0 INIT_ACQ_BITS_PD=00b */
    0xFF,   // RXDCON2
               /* bit7-6 NUM_MODAMP_GAPS_D=11b */
               /* bit5-4 SLICERSEL_D=11b */
               /* bit3-2 CODINGRESTR_W=11b */
               /* bit1   CODINGRESTR_P=1b */
               /* bit0   CODINGRESTR_D=1b */
    0x00,   // SIGMON0
               /* bit7   WUPS_MODE=0b, pessimistic wakeup search */
               /* bit6-1 SIGMON_EN_W=000000b, nothing enabled for now */
               /* bit0   always 0b */
    0x01,   // SIGMON1
               /* bit7   EN_PREADET_TIMEOUT=0b, no timeout for now */
               /* bit6-1 SIGMON_EN_P=000000b, nothing enabled for now */
               /* bit0   ACCU_SIG_FAILS_P=1b */
    0x01,   // SIGMON2
               /* bit7   always 0b */
               /* bit6-1 SIGMON_EN_D=000000b, nothing enabled for now */
               /* bit0   ACCU_SIG_FAILS_D=1b */
    0x00,   // WUPSTO
               /* bit7-6 WUPSTIMEOUTPRESC=00b, not used for now */
               /* bit5-0 WUPSTIMEOUT=000000b, not used for now */
    0x00,   // SLICERINITL
               /* bit7-0 SLICERINITTHR_LO=0x00 */
    0x04,   // SLICERINITH
               /* bit7   always 0b */
               /* bit6-0 SLICERINITTHR_HI=0x04 */
    0x18,   // TIMINGCHK
               /* bit7   always 0b */
               /* bit6-5 BROBS_LENGTH=00b */
               /* bit4-2 SUMBITTMGERRTH=110b */
               /* bit1-0 SGLBITTMGERRTH=00b */
    0x58,   // RXCON
               /* bit7   STATAUTOSAMPLE=0b, */
               /* bit6   AUTOSAMPLEMANUAL=1b, no auto sample */
               /* bit5   INV_RX_DATA=0b, do not invert the received data */
               /* bit4-3 CLOCK_RECOV_TC=11b, 31 chips settling time */
               /* bit2   RX_MANCHESTER=0b, will be set during OL2381_rx_setup */
               /* bit1   RX_CLOCK_TRANSP=0b, clock output is always bit clock */
               /* bit0   RX_DATA_TRANSP=0b, */
    0x88,   // RXFOLLOWUP
               /* bit7   PREA_FU_TF=1b, power down */
               /* bit6   PREA_FU_CF=0b, stop */
               /* bit5-4 WUPS_FU_TS=00b, stop */
               /* bit3   WUPS_FU_TF=1b, power down */
               /* bit2-1 WUPS_FU_CS=00b, stop */
               /* bit0   WUPS_FU_CF=0b, stop */
    0x00,   // SIGMONSTATUS
               /* bit7-0 00000000b (read-only register) */
    0x00,   // SIGMONERROR
               /* bit7-0 00000000b (read-only register) */
    0x00,   // RSSILEVEL
               /* bit7-0 00000000b (read-only register) */
    0x00,   // PREACON
               /* bit7   always 0b */
               /* bit6-5 PREA_TOL=00b, preamble detection bit rate error 0 */
               /* bit4-0 PREA_LEN=00000b, will be set during OL2381_rx_setup */
    0x00,   // PREA0
               /* bit7-0 will be set during OL2381_rx_setup */
    0x00,   // PREA1
               /* bit7-0 will be set during OL2381_rx_setup */
    0x00,   // PREA2
               /* bit7-0 will be set during OL2381_rx_setup */
    0x00,   // PREA3
               /* bit7-0 will be set during OL2381_rx_setup */
    0x00    // BANKSEL
               /* bit7-1 always 0000000b */
               /* bit0   BANK_SEL=0, Bank 0 selected */
};

static const char sfr_buffer_bank1[17] =
{
    0x80,   // EXTRXSTATUS
    0x00,   // CFRCCAL
    0x00,   // CFIQCAL
    0x02,   // EXPERT0
               /* bit7   RED_VCO_SWING=0b */
               /* bit6   LARGE_PLL_RST_DELAY=0b */
               /* bit5   FASTCFFILTSETTL=0b */
               /* bit4-0 PLL_ICP=00010b (for best result) */
    0x49,   // EXPERT1
    0x02,   // EXPERT2
    0x00,   // EXPERT3
    0x00,   // TEST0
    0x00,   // TEST1
    0x00,   // TEST2
    0x00,   // TEST3
    0x00,   // TEST4
    0x00,   // TEST5
    0x00,   // reserved for future use
    0x00,   // TCBEN0
    0x00,   // TCBR
    0x00    // TCBEN1
};

#endif

// eof
