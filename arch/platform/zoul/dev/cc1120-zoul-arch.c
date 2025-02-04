/*
 * Copyright (c) 2015, Zolertia
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
 *
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
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup zoul
 * @{
 *
 * \defgroup zoul-cc1120 Zoul CC1120 arch
 *
 * CC1120 Zoul arch specifics
 * @{
 *
 * \file
 * CC1120 Zoul arch specifics
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "reg.h"
#include "dev/spi-arch-legacy.h"
#include "dev/ioc.h"
#include "dev/sys-ctrl.h"
#include "dev/spi-legacy.h"
#include "dev/ssi.h"
#include "dev/gpio.h"
#include "dev/gpio-hal.h"
#include <stdio.h>
/*---------------------------------------------------------------------------*/
#define CC1120_SPI_CLK_PORT_BASE   GPIO_PORT_TO_BASE(SPI0_CLK_PORT)
#define CC1120_SPI_CLK_PIN_MASK    GPIO_PIN_MASK(SPI0_CLK_PIN)
#define CC1120_SPI_MOSI_PORT_BASE  GPIO_PORT_TO_BASE(SPI0_TX_PORT)
#define CC1120_SPI_MOSI_PIN_MASK   GPIO_PIN_MASK(SPI0_TX_PIN)
#define CC1120_SPI_MISO_PORT_BASE  GPIO_PORT_TO_BASE(SPI0_RX_PORT)
#define CC1120_SPI_MISO_PIN_MASK   GPIO_PIN_MASK(SPI0_RX_PIN)
#define CC1120_SPI_CSN_PORT_BASE   GPIO_PORT_TO_BASE(CC1120_SPI_CSN_PORT)
#define CC1120_SPI_CSN_PIN_MASK    GPIO_PIN_MASK(CC1120_SPI_CSN_PIN)
#define CC1120_GDO0_PORT_BASE      GPIO_PORT_TO_BASE(CC1120_GDO0_PORT)
#define CC1120_GDO0_PIN_MASK       GPIO_PIN_MASK(CC1120_GDO0_PIN)
#define CC1120_GDO2_PORT_BASE      GPIO_PORT_TO_BASE(CC1120_GDO2_PORT)
#define CC1120_GDO2_PIN_MASK       GPIO_PIN_MASK(CC1120_GDO2_PIN)
#define CC1120_RESET_PORT_BASE     GPIO_PORT_TO_BASE(CC1120_RESET_PORT)
#define CC1120_RESET_PIN_MASK      GPIO_PIN_MASK(CC1120_RESET_PIN)
/*---------------------------------------------------------------------------*/
#ifndef DEBUG_CC1120_ARCH
#define DEBUG_CC1120_ARCH 0
#endif
/*---------------------------------------------------------------------------*/
#if DEBUG_CC1120_ARCH > 0
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
extern int cc1120_rx_interrupt(void);
/*---------------------------------------------------------------------------*/
void
cc1120_int_handler(gpio_hal_pin_mask_t pin_mask)
{
  /* To keep the gpio_register_callback happy */
  cc1120_rx_interrupt();
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_spi_select(void)
{
  /* Set CSn to low (0) */
  GPIO_CLR_PIN(CC1120_SPI_CSN_PORT_BASE, CC1120_SPI_CSN_PIN_MASK);
  /* The MISO pin should go low before chip is fully enabled. */
  RTIMER_BUSYWAIT_UNTIL(
    GPIO_READ_PIN(CC1120_SPI_MISO_PORT_BASE, CC1120_SPI_MISO_PIN_MASK) == 0,
    RTIMER_SECOND / 100);
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_spi_deselect(void)
{
  /* Set CSn to high (1) */
  GPIO_SET_PIN(CC1120_SPI_CSN_PORT_BASE, CC1120_SPI_CSN_PIN_MASK);
}
/*---------------------------------------------------------------------------*/
int
cc1120_arch_spi_rw_byte(uint8_t c)
{
  SPI_WAITFORTx_BEFORE();
  SPIX_BUF(CC1120_SPI_INSTANCE) = c;
  SPIX_WAITFOREOTx(CC1120_SPI_INSTANCE);
  SPIX_WAITFOREORx(CC1120_SPI_INSTANCE);
  c = SPIX_BUF(CC1120_SPI_INSTANCE);

  return c;
}
/*---------------------------------------------------------------------------*/
int
cc1120_arch_spi_rw(uint8_t *inbuf, const uint8_t *write_buf, uint16_t len)
{
  int i;
  uint8_t c;

  if((inbuf == NULL && write_buf == NULL) || len <= 0) {
    return 1;
  } else if(inbuf == NULL) {
    for(i = 0; i < len; i++) {
      SPI_WAITFORTx_BEFORE();
      SPIX_BUF(CC1120_SPI_INSTANCE) = write_buf[i];
      SPIX_WAITFOREOTx(CC1120_SPI_INSTANCE);
      SPIX_WAITFOREORx(CC1120_SPI_INSTANCE);
      c = SPIX_BUF(CC1120_SPI_INSTANCE);
      /* read and discard to avoid "variable set but not used" warning */
      (void)c;
    }
  } else if(write_buf == NULL) {
    for(i = 0; i < len; i++) {
      SPI_WAITFORTx_BEFORE();
      SPIX_BUF(CC1120_SPI_INSTANCE) = 0;
      SPIX_WAITFOREOTx(CC1120_SPI_INSTANCE);
      SPIX_WAITFOREORx(CC1120_SPI_INSTANCE);
      inbuf[i] = SPIX_BUF(CC1120_SPI_INSTANCE);
    }
  } else {
    for(i = 0; i < len; i++) {
      SPI_WAITFORTx_BEFORE();
      SPIX_BUF(CC1120_SPI_INSTANCE) = write_buf[i];
      SPIX_WAITFOREOTx(CC1120_SPI_INSTANCE);
      SPIX_WAITFOREORx(CC1120_SPI_INSTANCE);
      inbuf[i] = SPIX_BUF(CC1120_SPI_INSTANCE);
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static gpio_hal_event_handler_t interrupt_handler = {
  .next = NULL,
  .handler = cc1120_int_handler,
  .pin_mask =
    (gpio_hal_pin_to_mask(CC1120_GDO0_PIN) << (CC1120_GDO0_PORT << 3)) |
    (gpio_hal_pin_to_mask(CC1120_GDO2_PIN) << (CC1120_GDO2_PORT << 3))
};
/*---------------------------------------------------------------------------*/
void
cc1120_arch_gpio0_setup_irq(int rising)
{

  GPIO_SOFTWARE_CONTROL(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_SET_INPUT(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_DETECT_EDGE(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_TRIGGER_SINGLE_EDGE(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);

  if(rising) {
    GPIO_DETECT_RISING(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  } else {
    GPIO_DETECT_FALLING(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  }

  GPIO_ENABLE_INTERRUPT(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  ioc_set_over(CC1120_GDO0_PORT, CC1120_GDO0_PIN, IOC_OVERRIDE_PUE);
  NVIC_EnableIRQ(CC1120_GPIOx_VECTOR);
  gpio_hal_register_handler(&interrupt_handler);
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_gpio2_setup_irq(int rising)
{

  GPIO_SOFTWARE_CONTROL(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  GPIO_SET_INPUT(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  GPIO_DETECT_EDGE(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  GPIO_TRIGGER_SINGLE_EDGE(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);

  if(rising) {
    GPIO_DETECT_RISING(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  } else {
    GPIO_DETECT_FALLING(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  }

  GPIO_ENABLE_INTERRUPT(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  ioc_set_over(CC1120_GDO2_PORT, CC1120_GDO2_PIN, IOC_OVERRIDE_PUE);
  NVIC_EnableIRQ(CC1120_GPIOx_VECTOR);
  gpio_hal_register_handler(&interrupt_handler);
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_gpio0_enable_irq(void)
{
  GPIO_ENABLE_INTERRUPT(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  ioc_set_over(CC1120_GDO0_PORT, CC1120_GDO0_PIN, IOC_OVERRIDE_PUE);
  NVIC_EnableIRQ(CC1120_GPIOx_VECTOR);
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_gpio0_disable_irq(void)
{
  GPIO_DISABLE_INTERRUPT(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_gpio2_enable_irq(void)
{
  GPIO_ENABLE_INTERRUPT(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  ioc_set_over(CC1120_GDO2_PORT, CC1120_GDO2_PIN, IOC_OVERRIDE_PUE);
  NVIC_EnableIRQ(CC1120_GPIOx_VECTOR);
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_gpio2_disable_irq(void)
{
  GPIO_DISABLE_INTERRUPT(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
}
/*---------------------------------------------------------------------------*/
int
cc1120_arch_gpio0_read_pin(void)
{
  return (GPIO_READ_PIN(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
int
cc1120_arch_gpio2_read_pin(void)
{
  return GPIO_READ_PIN(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
}
/*---------------------------------------------------------------------------*/
int
cc1120_arch_gpio3_read_pin(void)
{
  return 0x00;
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_init(void)
{
  /* First leave RESET high */
  GPIO_SOFTWARE_CONTROL(CC1120_RESET_PORT_BASE, CC1120_RESET_PIN_MASK);
  GPIO_SET_OUTPUT(CC1120_RESET_PORT_BASE, CC1120_RESET_PIN_MASK);
  ioc_set_over(CC1120_RESET_PORT, CC1120_RESET_PIN, IOC_OVERRIDE_OE);
  GPIO_SET_PIN(CC1120_RESET_PORT_BASE, CC1120_RESET_PIN_MASK);

  /* Initialize CSn, enable CSn and then wait for MISO to go low*/
  spix_cs_init(CC1120_SPI_CSN_PORT, CC1120_SPI_CSN_PIN);

  /* Initialize SPI */
  spix_init(CC1120_SPI_INSTANCE);

  /* Configure GPIOx */
  GPIO_SOFTWARE_CONTROL(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_SET_INPUT(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_SOFTWARE_CONTROL(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  GPIO_SET_INPUT(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);

  /* Leave CSn as default */
  cc1120_arch_spi_deselect();

  /* Ensure MISO is high */
  RTIMER_BUSYWAIT_UNTIL(
    GPIO_READ_PIN(CC1120_SPI_MISO_PORT_BASE, CC1120_SPI_MISO_PIN_MASK),
    RTIMER_SECOND / 10);
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
