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
 * This is a setup for the following configuration:
 *
 * 802.15.4g
 * =========
 * Table 68f: Frequency band identifier 4 (863-870 MHz)
 * Table 68g: Modulation scheme identifier 0 (Filtered FSK)
 * Table 68h: Mode #1 (50kbps)
 */

/* Base frequency in kHz */
#define RF_CFG_CHAN_CENTER_F0           0x69E100  //169.40625MHz
/* Channel spacing in Hz */
#define RF_CFG_CHAN_SPACING             0x200 //Channel spacing 0.0125MHz
/* The minimum channel */
#define RF_CFG_MIN_CHANNEL              0
/* The maximum channel */
#define RF_CFG_MAX_CHANNEL              5  
/* The maximum output power in dBm */
#define RF_CFG_MAX_TXPOWER              CC1120_CONST_TX_POWER_MAX
/* The carrier sense level used for CCA in dBm */
#define RF_CFG_CCA_THRESHOLD            (-80)  //-100
/* The RSSI offset in dBm */
#define RF_CFG_RSSI_OFFSET              (-102)  //-102
/*---------------------------------------------------------------------------*/
static const char rf_cfg_descriptor[] = "802.15.4g 169MHz 4-FSK mode #3 Charlie";
/*---------------------------------------------------------------------------*/

/* 1 byte time: 160 usec */
#define CC1120_TSCH_PREAMBLE_LENGTH               800 /* 5 bytes */
#define CC1120_TSCH_CONF_RX_WAIT                 2200
#define CC1120_TSCH_CONF_RX_ACK_WAIT              400

#define CC1120_TSCH_DEFAULT_TS_CCA_OFFSET        1800
#define CC1120_TSCH_DEFAULT_TS_CCA                128

#define CC1120_TSCH_DEFAULT_TS_TX_OFFSET         3800
#define CC1120_TSCH_DEFAULT_TS_RX_OFFSET         (CC1120_TSCH_DEFAULT_TS_TX_OFFSET - CC1120_TSCH_PREAMBLE_LENGTH - (CC1120_TSCH_CONF_RX_WAIT / 2))
#define CC1120_TSCH_DEFAULT_TS_RX_ACK_DELAY      (CC1120_TSCH_DEFAULT_TS_TX_ACK_DELAY - CC1120_TSCH_PREAMBLE_LENGTH - (CC1120_TSCH_CONF_RX_ACK_WAIT / 2))
#define CC1120_TSCH_DEFAULT_TS_TX_ACK_DELAY      3000

#define CC1120_TSCH_DEFAULT_TS_RX_WAIT           (CC1120_TSCH_PREAMBLE_LENGTH + CC1120_TSCH_CONF_RX_WAIT)
#define CC1120_TSCH_DEFAULT_TS_ACK_WAIT          (CC1120_TSCH_PREAMBLE_LENGTH + CC1120_TSCH_CONF_RX_ACK_WAIT)
#define CC1120_TSCH_DEFAULT_TS_RX_TX              192
#define CC1120_TSCH_DEFAULT_TS_MAX_ACK           3360 /* 17+1+3 bytes at 50 kbps */
#define CC1120_TSCH_DEFAULT_TS_MAX_TX           20800 /* 126+1+3 bytes at 50 kbps */

#define CC1120_TSCH_DEFAULT_SLACK_TIME            500
/* Timeslot length: 31460 usec */
#define CC1120_TSCH_DEFAULT_TS_TIMESLOT_LENGTH  \
                                                  ( CC1120_TSCH_DEFAULT_TS_TX_OFFSET \
                                                  + CC1120_TSCH_DEFAULT_TS_MAX_TX \
                                                  + CC1120_TSCH_DEFAULT_TS_TX_ACK_DELAY \
                                                  + CC1120_TSCH_DEFAULT_TS_MAX_ACK \
                                                  + CC1120_TSCH_DEFAULT_SLACK_TIME \
                                                  )

/* TSCH timeslot timing (mircoseconds) */
static const tsch_timeslot_timing_usec cc1120_50kbps_tsch_timing = {
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

/*
 * Register settings exported from SmartRF Studio using the standard template
 * "trxEB RF Settings Performance Line".
 */

// Address Config = No address check 
// Bit Rate = 9.6 
// Carrier Frequency = 169.406250 
// Deviation = 3.997803 
// Device Address = 0 
// Manchester Enable = false 
// Modulation Format = 2-GFSK 
// PA Ramping = true 
// Packet Bit Length = 0 
// Packet Length = 255 
// Packet Length Mode = Variable 
// Performance Mode = High Performance 
// RX Filter BW = 100.000000 
// Symbol rate = 9.6 
// TX Power = 15 
// Whitening = false 

static const registerSetting_t preferredSettings[]= 
{
  {CC1120_IOCFG3,            0xB0},
  {CC1120_IOCFG2,            0x06},
  {CC1120_IOCFG1,            0xB0},
  {CC1120_IOCFG0,            0x40},
  {CC1120_SYNC3,             0x55},
  {CC1120_SYNC2,             0x55},
  {CC1120_SYNC1,             0x7A},
  {CC1120_SYNC0,             0x0E},
  {CC1120_SYNC_CFG1,         0x08},
  {CC1120_SYNC_CFG0,         0x0B},
  {CC1120_MODCFG_DEV_E,      0x0B},
  {CC1120_DCFILT_CFG,        0x15},
  {CC1120_PREAMBLE_CFG1,     0x18},
  {CC1120_FREQ_IF_CFG,       0x3A},
  {CC1120_IQIC,              0x00},
  {CC1120_CHAN_BW,           0x02},
  {CC1120_MDMCFG0,           0x05},
  {CC1120_SYMBOL_RATE2,      0x73},
  {CC1120_AGC_REF,           0x3C},
  {CC1120_AGC_CS_THR,        0xEF},
  {CC1120_AGC_CFG1,          0xA9},
  {CC1120_AGC_CFG0,          0xC0},
  {CC1120_FIFO_CFG,          0x00},
  {CC1120_FS_CFG,            0x1A},
  {CC1120_PKT_CFG0,          0x20},
  {CC1120_PA_CFG0,           0x7D},
  {CC1120_PKT_LEN,           0xFF},
  {CC1120_IF_MIX_CFG,        0x00},
  {CC1120_TOC_CFG,           0x0A},
  {CC1120_FREQ2,             0x69},
  {CC1120_FREQ1,             0xE1},
  {CC1120_FS_DIG1,           0x00},
  {CC1120_FS_DIG0,           0x5F},
  {CC1120_FS_CAL1,           0x40},
  {CC1120_FS_CAL0,           0x0E},
  {CC1120_FS_DIVTWO,         0x03},
  {CC1120_FS_DSM0,           0x33},
  {CC1120_FS_DVC0,           0x17},
  {CC1120_FS_PFD,            0x50},
  {CC1120_FS_PRE,            0x6E},
  {CC1120_FS_REG_DIV_CML,    0x14},
  {CC1120_FS_SPARE,          0xAC},
  {CC1120_FS_VCO0,           0xB4},
  {CC1120_XOSC5,             0x0E},
  {CC1120_XOSC1,             0x03},
};
/*---------------------------------------------------------------------------*/
/* Global linkage: symbol name must be different in each exported file! */
const cc1120_rf_cfg_t cc1120_802154g_169_4fsk_9_6kbps = {
  .cfg_descriptor = rf_cfg_descriptor,
  .register_settings = preferredSettings,
  .size_of_register_settings = sizeof(preferredSettings),
  .tx_pkt_lifetime = (RTIMER_SECOND / 5),
  .tx_rx_turnaround = (RTIMER_SECOND / 100),
  /* Includes 3 Bytes preamble + 2 Bytes SFD, at 160usec per byte = 800 usec */
  /* Includes time to completion of "Wait for TX to start" if cc1120.c: 397 usec */
  .delay_before_tx = ((unsigned)US_TO_RTIMERTICKS(800 + 397 + 423)),
  .delay_before_rx = (unsigned)US_TO_RTIMERTICKS(400),
  .delay_before_detect = 0,
  .chan_center_freq0 = RF_CFG_CHAN_CENTER_F0,
  .chan_spacing = RF_CFG_CHAN_SPACING,
  .min_channel = RF_CFG_MIN_CHANNEL,
  .max_channel = RF_CFG_MAX_CHANNEL,
  .max_txpower = RF_CFG_MAX_TXPOWER,
  .cca_threshold = RF_CFG_CCA_THRESHOLD,
  .rssi_offset = RF_CFG_RSSI_OFFSET,
  .bitrate = 9600,
  .tsch_timing = NULL,
  //.tsch_timing = cc1120_50kbps_tsch_timing,
};
/*---------------------------------------------------------------------------*/
