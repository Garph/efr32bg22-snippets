/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/


#include "em_device.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "em_prs.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/***************************************************************************//**
 * Constants
 ******************************************************************************/

/**
 * Port/pin definition for expansion header 16
 */
#define THUNDERBOARD_EXPANSION_HEADER_16_PORT (gpioPortD)
#define THUNDERBOARD_EXPANSION_HEADER_16_PIN (2)

/**
 * Port/pin definition for expansion header 16
 */
#define THUNDERBOARD_LED_PORT (gpioPortB)
#define THUNDERBOARD_LED_PIN (0)

/**
 * Routes CLKOUT0 to expansion header 16 (GPIO_CMU_CLKOUT0ROUTE register)
 */
#define CLOCK_OUT_0_ROUTE_MASK \
  ((THUNDERBOARD_EXPANSION_HEADER_16_PORT << _GPIO_CMU_CLKOUT0ROUTE_PORT_SHIFT) |\
   (THUNDERBOARD_EXPANSION_HEADER_16_PIN << _GPIO_CMU_CLKOUT0ROUTE_PIN_SHIFT))

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/

void app_init(void)
{
  /**
   * Clock initialization
   */

  // Select LFXO (32.768 kHz) for EM23GRPACLK (upstream of LETIMER0)
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);

  // Enable clock gates on peripherals
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_LETIMER0, true);
  CMU_ClockEnable(cmuClock_PRS, true);

  /**
   * Clock output on a pin
   *
   * 1. Configure pin for digital output
   *
   * 2. Export the desired clock to digital-bus with CMU_EXPORTCLKCTRL register
   *
   * 3. Select the output port/pin with GPIO_CMU_CLKOUTnROUTE
   *
   * 4. Enable the route with GPIO_CMU_ROUTEEN
   *
   **/


  GPIO_PinModeSet(THUNDERBOARD_EXPANSION_HEADER_16_PORT,
                  THUNDERBOARD_EXPANSION_HEADER_16_PIN,
                  gpioModePushPull,
                  0);

  CMU->EXPORTCLKCTRL |= CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFXO;

  GPIO->CMUROUTE.CLKOUT0ROUTE |= CLOCK_OUT_0_ROUTE_MASK;

  GPIO->CMUROUTE.ROUTEEN |= GPIO_CMU_ROUTEEN_CLKOUT0PEN;

  /**
   * 1 Hz LED blink pattern for user feedback
   *
   * 1. Configure LED pin for digital output
   *
   * 2. Configure LETIMER0 to generate a 1 Hz underflow output action (toggle)
   *
   * 3. Connect LETIMER0 to GPIO with async PRS channel 0
   */

  GPIO_PinModeSet(THUNDERBOARD_LED_PORT,
                  THUNDERBOARD_LED_PIN,
                  gpioModePushPull,
                  0);

  LETIMER_Init_TypeDef config = LETIMER_INIT_DEFAULT;
  config.topValue = CMU_ClockFreqGet(cmuClock_EM23GRPACLK) / 2;
  config.ufoa0 = letimerUFOAToggle;
  LETIMER_Init(LETIMER0, &config);

  PRS_SourceAsyncSignalSet(0,
                           PRS_ASYNC_CH_CTRL_SOURCESEL_LETIMER0,
                           PRS_ASYNC_CH_CTRL_SIGSEL_LETIMER0CH0);

  PRS_PinOutput(0, prsTypeAsync, gpioPortB, 0);
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
}
