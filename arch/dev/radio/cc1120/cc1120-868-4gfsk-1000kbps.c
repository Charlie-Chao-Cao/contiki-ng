/*
 * Copyright (c) 2015, Weptech elektronik GmbH Germany
 * http://www.weptech.de
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

#include "dev/radio/cc1120/cc1120-rf-cfg.h"
#include "dev/radio/cc1120/cc1120-const.h"
#include "net/mac/tsch/tsch.h"

/*
* Register settings exported from SmartRF Studio using the standard 1000kpbs 868MHz template
 */

 /* Base frequency in kHz */
 #define RF_CFG_CHAN_CENTER_F0           863125
 /* Channel spacing in Hz */
 #define RF_CFG_CHAN_SPACING             1666667
/* The minimum channel */
#define RF_CFG_MIN_CHANNEL              0
/* The maximum channel */
#define RF_CFG_MAX_CHANNEL              3
/* The maximum output power in dBm */
#define RF_CFG_MAX_TXPOWER              CC1120_CONST_TX_POWER_MAX
/* The carrier sense level used for CCA in dBm */
#define RF_CFG_CCA_THRESHOLD            (-91)
/* The RSSI offset in dBm */
#define RF_CFG_RSSI_OFFSET              (-81)
/*---------------------------------------------------------------------------*/
static const char rf_cfg_descriptor[] = "868MHz 2-GFSK 1000 kbps";
/*---------------------------------------------------------------------------*/

/* 1 byte time: 8 usec */
#define CC1120_TSCH_PREAMBLE_LENGTH               800 /* 5 bytes */
#define CC1120_TSCH_CONF_RX_WAIT                 2200
#define CC1120_TSCH_CONF_RX_ACK_WAIT              400

#define CC1120_TSCH_DEFAULT_TS_CCA_OFFSET        1800
#define CC1120_TSCH_DEFAULT_TS_CCA                128
#define CC1120_TSCH_DEFAULT_TS_TX_OFFSET         2200
#define CC1120_TSCH_DEFAULT_TS_RX_OFFSET         (CC1120_TSCH_DEFAULT_TS_TX_OFFSET - CC1120_TSCH_PREAMBLE_LENGTH - (CC1120_TSCH_CONF_RX_WAIT / 2))
#define CC1120_TSCH_DEFAULT_TS_RX_ACK_DELAY      (CC1120_TSCH_DEFAULT_TS_TX_ACK_DELAY - CC1120_TSCH_PREAMBLE_LENGTH - (CC1120_TSCH_CONF_RX_ACK_WAIT / 2))
#define CC1120_TSCH_DEFAULT_TS_TX_ACK_DELAY      1900
#define CC1120_TSCH_DEFAULT_TS_RX_WAIT           (CC1120_TSCH_PREAMBLE_LENGTH + CC1120_TSCH_CONF_RX_WAIT)
#define CC1120_TSCH_DEFAULT_TS_ACK_WAIT          (CC1120_TSCH_PREAMBLE_LENGTH + CC1120_TSCH_CONF_RX_ACK_WAIT)
#define CC1120_TSCH_DEFAULT_TS_RX_TX              192
#define CC1120_TSCH_DEFAULT_TS_MAX_ACK            168 /* 17+1+3 bytes at 1000 kbps */
#define CC1120_TSCH_DEFAULT_TS_MAX_TX            1040 /* 126+1+3 bytes at 1000 kbps */

#define CC1120_TSCH_DEFAULT_SLACK_TIME            500
/* Timeslot length: 5808 usec */
#define CC1120_TSCH_DEFAULT_TS_TIMESLOT_LENGTH  \
                                                  ( CC1120_TSCH_DEFAULT_TS_TX_OFFSET \
                                                  + CC1120_TSCH_DEFAULT_TS_MAX_TX \
                                                  + CC1120_TSCH_DEFAULT_TS_TX_ACK_DELAY \
                                                  + CC1120_TSCH_DEFAULT_TS_MAX_ACK \
                                                  + CC1120_TSCH_DEFAULT_SLACK_TIME \
                                                  )

/* TSCH timeslot timing (in rtimer ticks) */
static const tsch_timeslot_timing_usec cc1120_1000kbps_tsch_timing = {
  CC1120_TSCH_DEFAULT_TS_CCA_OFFSET,
  CC1120_TSCH_DEFAULT_TS_CCA,
  CC1120_TSCH_DEFAULT_TS_TX_OFFSET,
  CC1120_TSCH_DEFAULT_TS_RX_OFFSET,
  CC1120_TSCH_DEFAULT_TS_RX_ACK_DELAY,
  CC1120_TSCH_DEFAULT_TS_TX_ACK_DELAY,
  CC1120_TSCH_DEFAULT_TS_RX_WAIT,
  CC1120_TSCH_DEFAULT_TS_ACK_WAIT,
  CC1120_TSCH_DEFAULT_TS_RX_TX,
  CC1120_TSCH_DEFAULT_TS_MAX_ACK,
  CC1120_TSCH_DEFAULT_TS_MAX_TX,
  CC1120_TSCH_DEFAULT_TS_TIMESLOT_LENGTH,
};

static const registerSetting_t preferredSettings[]=
{
 {CC1120_IOCFG2,            0x06},
 {CC1120_SYNC_CFG1,         0xA8},
 {CC1120_DEVIATION_M,       0x47},
 {CC1120_MODCFG_DEV_E,      0x2F},
 {CC1120_DCFILT_CFG,        0x1E},
 {CC1120_PREAMBLE_CFG0,     0x8A},
 {CC1120_IQIC,              0x00},
 {CC1120_CHAN_BW,           0x01},
 {CC1120_MDMCFG1,           0x42},
 {CC1120_MDMCFG0,           0x05},
 {CC1120_SYMBOL_RATE2,      0xC9},
 {CC1120_SYMBOL_RATE1,      0x99},
 {CC1120_SYMBOL_RATE0,      0x99},
 {CC1120_AGC_REF,           0x2F},
 {CC1120_AGC_CS_THR,        0xF8},
 {CC1120_AGC_CFG2,          0x60},
 {CC1120_AGC_CFG1,          0x12},
 {CC1120_AGC_CFG0,          0x84},
 {CC1120_FIFO_CFG,          0x00},
 {CC1120_FS_CFG,            0x12},
 {CC1120_PKT_CFG2,          0x00},
 {CC1120_PKT_CFG0,          0x20},
 {CC1120_PKT_LEN,           0xFF},
 {CC1120_FREQOFF_CFG,       0x23},
 {CC1120_MDMCFG2,           0x00},
 {CC1120_FREQ2,             0x56},
 {CC1120_FREQ1,             0xCC},
 {CC1120_FREQ0,             0xCC},
 {CC1120_IF_ADC1,           0xEE},
 {CC1120_IF_ADC0,           0x10},
 {CC1120_FS_DIG1,           0x04},
 {CC1120_FS_DIG0,           0xA3},
 {CC1120_FS_CAL1,           0x40},
 {CC1120_FS_CAL0,           0x0E},
 {CC1120_FS_DIVTWO,         0x03},
 {CC1120_FS_DSM0,           0x33},
 {CC1120_FS_DVC1,           0xF7},
 {CC1120_FS_DVC0,           0x0F},
 {CC1120_FS_PFD,            0x00},
 {CC1120_FS_PRE,            0x6E},
 {CC1120_FS_REG_DIV_CML,    0x1C},
 {CC1120_FS_SPARE,          0xAC},
 {CC1120_FS_VCO0,           0xB5},
 {CC1120_IFAMP,             0x0D},
 {CC1120_XOSC5,             0x0E},
 {CC1120_XOSC1,             0x03},
};
/*---------------------------------------------------------------------------*/
/* Global linkage: symbol name must be different in each exported file! */
const cc1120_rf_cfg_t cc1120_868_4gfsk_1000kbps = {
  .cfg_descriptor = rf_cfg_descriptor,
  .register_settings = preferredSettings,
  .size_of_register_settings = sizeof(preferredSettings),
  .tx_pkt_lifetime = (RTIMER_SECOND),
  .tx_rx_turnaround = (RTIMER_SECOND / 100),
  /* Includes 3 Bytes preamble + 2 Bytes SFD, at 8usec per byte = 40 usec */
  /* Includes time to completion of "Wait for TX to start" if cc1120.c: 397 usec */
  .delay_before_tx = ((unsigned)US_TO_RTIMERTICKS(40 + 397 + 207)),
  .delay_before_rx = (unsigned)US_TO_RTIMERTICKS(400),
  .delay_before_detect = 0,
  .chan_center_freq0 = RF_CFG_CHAN_CENTER_F0,
  .chan_spacing = RF_CFG_CHAN_SPACING,
  .min_channel = RF_CFG_MIN_CHANNEL,
  .max_channel = RF_CFG_MAX_CHANNEL,
  .max_txpower = RF_CFG_MAX_TXPOWER,
  .cca_threshold = RF_CFG_CCA_THRESHOLD,
  .rssi_offset = RF_CFG_RSSI_OFFSET,
  .bitrate = 1000000,
  .tsch_timing = cc1120_1000kbps_tsch_timing,
};
/*---------------------------------------------------------------------------*/
