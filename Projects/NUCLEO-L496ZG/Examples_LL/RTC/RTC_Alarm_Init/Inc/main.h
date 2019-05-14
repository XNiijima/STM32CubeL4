/**
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_Alarm_Init/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_cortex.h"
#include <stdio.h>
#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Define used to enable time-out management*/
#define USE_TIMEOUT       0

/**
  * @brief LED1 
  */

#define LED1_PIN                           LL_GPIO_PIN_7
#define LED1_GPIO_PORT                     GPIOC
#define LED1_GPIO_CLK_ENABLE()             LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC)

/**
  * @brief Toggle periods for various blinking modes
  */

#define LED_BLINK_FAST  200
#define LED_BLINK_SLOW 500
#define LED_BLINK_ERROR 1000


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* IRQ Handler treatment.  */
void Alarm_Callback(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
