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

#ifndef CC1120_H_
#define CC1120_H_

#include "contiki.h"

/*---------------------------------------------------------------------------*/
/*
 * Can we use GPIO2 (in addition to GPIO0)?
 *
 * If this is the case, we can easily handle payloads > 125 bytes
 * (and even > 127 bytes). If GPIO2 is available, we use it as an indicator
 * pin for RX / TX FIFO threshold.
 */
#ifdef CC1120_CONF_USE_GPIO2
#define CC1120_USE_GPIO2                CC1120_CONF_USE_GPIO2
#else
#define CC1120_USE_GPIO2                1
#endif
/*---------------------------------------------------------------------------*/
/*
 * The maximum payload length the driver can handle.
 *
 * - If CC1120_MAX_PAYLOAD_LEN <= 125 and CC1120_USE_GPIO2 == 0, we read
 * out the RX FIFO at the end of the packet. RXOFF_MODE is set to RX in this
 * case.
 * - If 125 < CC1120_MAX_PAYLOAD_LEN <= 127 and CC1120_USE_GPIO2 == 0, we
 * also read out the RX FIFO at the end of the packet, but read out
 * RSSI + LQI "by hand". In this case, we also have to restart RX
 * manually because RSSI + LQI are overwritten as soon as RX re-starts.
 * This will lead to an increased RX/RX turnaround time.
 * - If CC1120_USE_GPIO2 is set, we can use an arbitrary payload length
 * (only limited by the payload length defined in the phy header).
 *
 * See below for 802.15.4g support.
 */
#ifdef CC1120_CONF_MAX_PAYLOAD_LEN
#define CC1120_MAX_PAYLOAD_LEN           CC1120_CONF_MAX_PAYLOAD_LEN
#else
#define CC1120_MAX_PAYLOAD_LEN           125
#endif
/*---------------------------------------------------------------------------*/
/*
 * The RX watchdog is used to check whether the radio is in RX mode at regular
 * intervals (once per second). Can be used to improve reliability especially
 * if CSMA is used. Turned of by default.
 */
#ifdef CC1120_CONF_USE_RX_WATCHDOG
#define CC1120_USE_RX_WATCHDOG          CC1120_CONF_USE_RX_WATCHDOG
#else
#define CC1120_USE_RX_WATCHDOG          0
#endif
/*---------------------------------------------------------------------------*/
/*
 * Use 802.15.4g frame format? Supports frame lenghts up to 2047 bytes!
 */
#ifdef CC1120_CONF_802154G
#define CC1120_802154G                  CC1120_CONF_802154G
#else
#define CC1120_802154G                  0
#endif
/*---------------------------------------------------------------------------*/
/*
 * Do we use withening in 802.15.4g mode? Set to 1 if enabled, 0 otherwise.
 */
#ifdef CC1120_CONF_802154G_WHITENING
#define CC1120_802154G_WHITENING        CC1120_CONF_802154G_WHITENING
#else
#define CC1120_802154G_WHITENING        0
#endif
/*---------------------------------------------------------------------------*/
/*
 * Do we use CRC16 in 802.15.4g mode? Set to 1 if enabled, 0 otherwise.
 *
 * It set to 0, we use FCS type 0: CRC32.
 */
#ifdef CC1120_CONF_802154G_CRC16
#define CC1120_802154G_CRC16            CC1120_CONF_802154G_CRC16
#else
/* Use FCS type 1: CRC16 if 802154G is enabled, else use FCS type 0: CRC32 */
#define CC1120_802154G_CRC16            CC1120_802154G
#endif
/*---------------------------------------------------------------------------*/
/* The RF configuration to be used. */
#ifdef CC1120_CONF_RF_CFG
#define CC1120_RF_CFG                   CC1120_CONF_RF_CFG
#else
#define CC1120_RF_CFG                   cc1120_802154g_863_870_fsk_50kbps   //868MHz
//#define CC1120_RF_CFG                   cc1120_802154g_169_4fsk_9_6kbps     //169MHz
#endif
/*---------------------------------------------------------------------------*/
/*
 * The frequency 
 *
 * Might be hardware dependent (e.g. depending on crystal load capacitances),
 * so we make it a configuration parameter. Written to FREQOFF1 / FREQOFF2.
 * Signed 16 bit number, see cc1120 user's guide.
 *
 * TODO: Make it a parameter for set_value() / get_value()
 */
#ifdef CC1120_CONF_FREQ_OFFSET
#define CC1120_FREQ_OFFSET              CC1120_CONF_FREQ_OFFSET
#else
#define CC1120_FREQ_OFFSET              (0)
#endif
/*---------------------------------------------------------------------------*/
/*
 * The default channel to use. 
 *
 * Permitted values depending on the data rate + band used are defined
 * in the appropriate rf configuration file. Make sure the default value
 * is within these limits!
 */
#ifdef CC1120_CONF_DEFAULT_CHANNEL
#define CC1120_DEFAULT_CHANNEL          CC1120_CONF_DEFAULT_CHANNEL
#else
/* 868.325 MHz */
//#define CC1120_DEFAULT_CHANNEL          40
/* 169.40625 MHz */
#define CC1120_DEFAULT_CHANNEL          3
#endif
/*---------------------------------------------------------------------------*/
/*
 * Wether to use auto calibration or not.
 *
 * If set to 0, calibration is performed manually when turning the radio
 * on (on()), when transmitting (transmit()) or when changing the channel.
 * Enabling auto calibration will increase turn around times +
 * energy consumption. If enabled, we calibrate every time we go from
 * IDLE to RX or TX.
 * When RDC or channel hopping is used, there is no need to turn calibration
 * on because either on() is called frequently or the channel is updated.
 */
#ifdef CC1120_CONF_AUTOCAL
#define CC1120_AUTOCAL                  CC1120_CONF_AUTOCAL
#else
#define CC1120_AUTOCAL                  0 //charlie default 0
#endif
/*---------------------------------------------------------------------------*/
/*
 * If CC1120_AUTOCAL is not set, we use this parameter to defer 
 * calibration until a certain amount of time has expired. 
 *
 * This is what happens in detail:
 *
 * - We (manually) calibrate once after initialization
 * - We (manually) calibrate every time we change the channel
 * - We (manually) calibrate when the radio is turned on() only if
 *   the timeout has expired
 * - We (manually) calibrate when transmitting only of the timeout has expired
 * 
 * Set this parameter to 0 when this feature is not used. In this case we 
 * (manually) calibrate in all situations mentioned above.
 */
#ifdef CC1120_CONF_CAL_TIMEOUT_SECONDS
#define CC1120_CAL_TIMEOUT_SECONDS      CC1120_CONF_CAL_TIMEOUT_SECONDS
#else
/* Calibrate at the latest every 15 minutes */
#define CC1120_CAL_TIMEOUT_SECONDS      900
#endif
/*---------------------------------------------------------------------------*/
/*
 * If defined, use these LEDS to indicate TX activity
 *
 * The LEDs are turned on once the radio enters TX mode
 * (together with ENERGEST_ON(ENERGEST_TYPE_TRANSMIT),
 * and turned of as soon as TX has completed.
 */
#ifdef CC1120_CONF_TX_LEDS
#define CC1120_TX_LEDS                  CC1120_CONF_TX_LEDS
#endif
/*---------------------------------------------------------------------------*/
/*
 * If defined, use these LEDS to indicate RX activity
 *
 * The LEDs are turned on as soon as the first byte is read out from
 * the RX FIFO
 */
#ifdef CC1120_CONF_RX_LED
#define CC1120_RX_LEDS                  CC1120_CONF_RX_LEDS
#endif
/*---------------------------------------------------------------------------*/

#endif /* CC1120_H_ */
