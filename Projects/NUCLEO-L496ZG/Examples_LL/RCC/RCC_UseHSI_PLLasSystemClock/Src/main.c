/**
  ******************************************************************************
  * @file    Examples_LL/RCC/RCC_UseHSI_PLLasSystemClock/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to change dynamically SYSCLK through
  *          the STM32L4xx RCC LL API.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L4xx_LL_Examples
  * @{
  */

/** @addtogroup RCC_UseHSI_PLLasSystemClock
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Structure based on parameters used for PLL config */
typedef struct
{
  uint32_t Frequency;	/*!< SYSCLK frequency requested */
  uint32_t PLLM;	    /*!< PLLM factor used for PLL */
  uint32_t PLLN;	    /*!< PLLN factor used for PLL */
  uint32_t PLLR;	    /*!< PLLN factor used for PLL */
  uint32_t Latency;	  /*!< Latency to be used with SYSCLK frequency */
} RCC_PLL_ConfigTypeDef;

/* Private define ------------------------------------------------------------*/
/* Number of PLL Config */
#define RCC_PLL_CONFIG_NB   2
#define RCC_FREQUENCY_LOW          ((uint32_t)40000000) /* Low Frequency set to 40MHz*/
#define RCC_FREQUENCY_HIGH         ((uint32_t)80000000) /* High Frequency set to 80Hz*/

/* Oscillator time-out values */
#define HSE_TIMEOUT_VALUE          ((uint32_t)5000) /* Time out for HSE start up, in ms */
#define HSI_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define MSI_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define PLL_TIMEOUT_VALUE          ((uint32_t)100)  /* 100 ms */
#define CLOCKSWITCH_TIMEOUT_VALUE  ((uint32_t)5000) /* 5 s    */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Variable to set different PLL config with HSI as PLL source clock */
static RCC_PLL_ConfigTypeDef aPLL_ConfigHSI[RCC_PLL_CONFIG_NB] = {
    {RCC_FREQUENCY_LOW, LL_RCC_PLLM_DIV_4, 20, LL_RCC_PLLR_DIV_2, LL_FLASH_LATENCY_2},
    {RCC_FREQUENCY_HIGH, LL_RCC_PLLM_DIV_2, 40, LL_RCC_PLLR_DIV_4, LL_FLASH_LATENCY_4},
  };
  
/* PLL Config index */
__IO uint8_t bPLLIndex = 0;

/* Variable to save the current configuration to apply */
static uint32_t uwFrequency = RCC_FREQUENCY_HIGH, uwPLLN = 0, uwPLLM = 0, uwPLLR  = 0, uwLatency = 0;
  
/* Variable to indicate a change of PLL config after a button press */
__IO uint8_t bButtonPress = 0;

uint32_t Timeout = 0; /* Variable used for Timeout management */

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void     SystemClock_Config(void);
uint32_t RCC_StartHSIAndWaitForHSIReady(void);
uint32_t ChangePLL_HSI_Config(void);
void     LED_Init(void);
void     LED_Blinking(uint32_t Period);
void     UserButton_Init(void);
uint32_t ChangePLLConfiguration(uint32_t PLLSource, uint32_t PLLM, uint32_t PLLN, uint32_t PLLR);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the system clock to 80 MHz */
  SystemClock_Config();

  /* Initialize LED1 */
  LED_Init();
 
  /* Initialize button in EXTI mode */
  UserButton_Init();

  /* Enable HSI to be able to select it as sysclk source */
  /* Start HSI and wait for ready */
  if (RCC_StartHSIAndWaitForHSIReady() != RCC_ERROR_NONE)
  {
    /* Problem to enable HSI, blink LED1 */
    LED_Blinking(LED_BLINK_ERROR);
  }
  
  /* Infinite loop */
  while (1)
  {
    /* Toggle LED accordingly to the frequency */
    if (uwFrequency == RCC_FREQUENCY_LOW)
    {
      /* Slow toggle */
      LED_Blinking(LED_BLINK_SLOW);
    }
    else
    {
      /* Fast toggle */
      LED_Blinking(LED_BLINK_FAST);
    }
    
    /* PLL config change has been requested */
    if (ChangePLL_HSI_Config() != RCC_ERROR_NONE)
    {
      /* Problem to switch to HSI, blink LED1 */
       LED_Blinking(LED_BLINK_ERROR);
    }
    
    /* Reset button press */
    bButtonPress = 0;
  }
}

/**
  * @brief  Enable HSI and Wait for HSI ready
  * @param  None
  * @retval RCC_ERROR_NONE if no error
  */
uint32_t RCC_StartHSIAndWaitForHSIReady()
{
  /* Enable HSI and wait for HSI ready*/
  LL_RCC_HSI_Enable();
  
#if (USE_TIMEOUT == 1)
  Timeout = HSI_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
  while(LL_RCC_HSI_IsReady() != 1) 
  {
#if (USE_TIMEOUT == 1)
    /* Check Systick counter flag to decrement the Time-out value */
    if (LL_SYSTICK_IsActiveCounterFlag()) 
    { 
      if(Timeout-- == 0)
      {
        /* Time-out occurred. Return an error */
        return RCC_ERROR_TIMEOUT;
      }
    } 
#endif /* USE_TIMEOUT */
  };

  return RCC_ERROR_NONE;
}

/**
  * @brief  Switch the PLL source to HSI, and select the PLL as SYSCLK
  *         source to reach new requested frequency.
  * @param  None
  * @retval RCC_ERROR_NONE if no error
  */
uint32_t ChangePLL_HSI_Config(void)
{  
  /* Select HSI as system clock */
  /* Wait for HSI switched */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI); 
#if (USE_TIMEOUT == 1)
  Timeout = CLOCKSWITCH_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) 
  {
#if (USE_TIMEOUT == 1)
    /* Check Systick counter flag to decrement the time-out value */
    if (LL_SYSTICK_IsActiveCounterFlag()) 
    { 
      if(Timeout-- == 0)
      {
        /* Time-out occurred. Return an error */
        return RCC_ERROR_TIMEOUT;
      }
    } 
#endif /* USE_TIMEOUT */
  }

  /* Configure PLL with new configuration */
  if (ChangePLLConfiguration(LL_RCC_PLLSOURCE_HSI, uwPLLM, uwPLLN, uwPLLR) != RCC_ERROR_NONE)
  {
    return RCC_ERROR_TIMEOUT;
  }
  
  /* Latency must be managed differently if increase or decrease the frequency */
  if (uwFrequency == RCC_FREQUENCY_LOW)
  {
    /* Decrease Frequency - latency should be set after setting PLL as clock source */
    /* Select PLL as system clock */
    /* Wait until the PLL is switched on */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL); 
#if (USE_TIMEOUT == 1)
    Timeout = CLOCKSWITCH_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) 
    {
#if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag()) 
      { 
        if(Timeout-- == 0)
        {
          /* Time-out occurred. Return an error */
          return RCC_ERROR_TIMEOUT;
        }
      } 
#endif /* USE_TIMEOUT */
    }
    
    /* Set new latency */
    LL_FLASH_SetLatency(uwLatency);
  }
  else
  {
    /* Increase Frequency - latency should be set before setting PLL as clock source */
    /* Set new latency */
    LL_FLASH_SetLatency(uwLatency);
    
    /* Select PLL as system clock */
    /* Wait until the PLL is switched on */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL); 
#if (USE_TIMEOUT == 1)
    Timeout = CLOCKSWITCH_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) 
    {
#if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag()) 
      { 
        if(Timeout-- == 0)
        {
          /* Time-out occurred. Return an error */
          return RCC_ERROR_TIMEOUT;
        }
      } 
#endif /* USE_TIMEOUT */
    }
  }
  
  /* Set systick to 1ms */
  LL_Init1msTick(uwFrequency);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  SystemCoreClock = uwFrequency;
 
  return RCC_ERROR_NONE;
}

/**
  * @brief  Fonction to change Main PLL configuration
  * @param  PLLSource: This parameter can be one of the following values: 
  *         @arg LL_RCC_PLLSOURCE_NONE
  *         @arg LL_RCC_PLLSOURCE_MSI
  *         @arg LL_RCC_PLLSOURCE_HSI
  *         @arg LL_RCC_PLLSOURCE_HSE
  * @param  PLLM: This parameter can be one of the following values: 
  *         @arg LL_RCC_PLLM_DIV_1
  *         @arg LL_RCC_PLLM_DIV_2
  *         @arg LL_RCC_PLLM_DIV_3
  *         @arg LL_RCC_PLLM_DIV_4
  *         @arg LL_RCC_PLLM_DIV_5
  *         @arg LL_RCC_PLLM_DIV_6
  *         @arg LL_RCC_PLLM_DIV_7
  *         @arg LL_RCC_PLLM_DIV_8
  * @param  PLLN: Between 8 and 86
  * @param  PLLR: This parameter can be one of the following values: 
  *         @arg LL_RCC_PLLR_DIV_2
  *         @arg LL_RCC_PLLR_DIV_4
  *         @arg LL_RCC_PLLR_DIV_6
  *         @arg LL_RCC_PLLR_DIV_8
  * @retval RCC_ERROR_NONE if no error
  */
uint32_t ChangePLLConfiguration(uint32_t PLLSource, uint32_t PLLM, uint32_t PLLN, uint32_t PLLR)
{
  /* Disable the PLL */
  /* Wait until PLLRDY is cleared */
  LL_RCC_PLL_Disable();
#if (USE_TIMEOUT == 1)
  Timeout = PLL_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
  while(LL_RCC_PLL_IsReady() != 0)
  {
#if (USE_TIMEOUT == 1)
    /* Check Systick counter flag to decrement the time-out value */
    if (LL_SYSTICK_IsActiveCounterFlag()) 
    { 
      if(Timeout-- == 0)
      {
        /* Time-out occurred. Return an error */
        return RCC_ERROR_TIMEOUT;
      }
    } 
#endif /* USE_TIMEOUT */
  }
  
  /* Configure PLL */
  LL_RCC_PLL_ConfigDomain_SYS(PLLSource, PLLM, PLLN, PLLR);

  /* Enable the PLL */ 
  /* Wait until PLLRDY is set */
  LL_RCC_PLL_Enable();
#if (USE_TIMEOUT == 1)
  Timeout = PLL_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
  while(LL_RCC_PLL_IsReady() != 1)
  {
#if (USE_TIMEOUT == 1)
    /* Check Systick counter flag to decrement the time-out value */
    if (LL_SYSTICK_IsActiveCounterFlag()) 
    { 
      if(Timeout-- == 0)
      {
        /* Time-out occurred. Return an error */
        return RCC_ERROR_TIMEOUT;
      }
    } 
#endif /* USE_TIMEOUT */
  }

  return RCC_ERROR_NONE;
}

/**
  * @brief  Initialize LED1.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  /* Enable the LED1 Clock */
  LED1_GPIO_CLK_ENABLE();

  /* Configure IO in output push-pull mode to drive external LED1 */
  LL_GPIO_SetPinMode(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
  /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
  //LL_GPIO_SetPinOutputType(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  /* Reset value is LL_GPIO_SPEED_FREQ_LOW */
  //LL_GPIO_SetPinSpeed(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
  /* Reset value is LL_GPIO_PULL_NO */
  //LL_GPIO_SetPinPull(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_PULL_NO);
}

/**
  * @brief  Set LED1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  *         Exit of this function when a press button is detected 
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  if (Period != LED_BLINK_ERROR)
  {
    /* Toggle IO in an infinite loop up to a detection of press button */
    while (bButtonPress != 1)
    {
      /* LED1 is blinking at Period ms */
      LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);  
      LL_mDelay(Period);
    }
  }
  else
  {
    /* Toggle IO in an infinite loop due to an error */
    while (1)
    {
      /* Error if LED1 is slowly blinking (1 sec. period) */
      LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);  
      LL_mDelay(Period);
    }
  }
}

/**
  * @brief  Configures User push-button in GPIO or EXTI Line Mode.
  * @param  None
  * @retval None
  */
void UserButton_Init(void)
{
  /* Enable the BUTTON Clock */
  USER_BUTTON_GPIO_CLK_ENABLE();
  
  /* Configure GPIO for BUTTON */
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_PULL_NO);

  /* Connect External Line to the GPIO*/
  USER_BUTTON_SYSCFG_SET_EXTI();
  
  /* Enable a rising trigger EXTI line 13 Interrupt */
  USER_BUTTON_EXTI_LINE_ENABLE();
  USER_BUTTON_EXTI_FALLING_TRIG_ENABLE();
  
  /* Configure NVIC for USER_BUTTON_EXTI_IRQn */
  NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn); 
  NVIC_SetPriority(USER_BUTTON_EXTI_IRQn,0x03);  
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  /* MSI configuration and activation */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  LL_RCC_MSI_Enable();
  while(LL_RCC_MSI_IsReady() != 1) 
  {
  };
  
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1) 
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) 
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 80MHz */
  /* This frequency can be calculated through LL RCC macro */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ(__LL_RCC_CALC_MSI_FREQ(LL_RCC_MSIRANGESEL_RUN, LL_RCC_MSIRANGE_6), 
                                  LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2)*/
  LL_Init1msTick(80000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(80000000);
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/
/**
  * @brief  Function to manage User button press
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* Get the PLL config to apply */
  uwFrequency = aPLL_ConfigHSI[bPLLIndex].Frequency;
  uwPLLM      = aPLL_ConfigHSI[bPLLIndex].PLLM;
  uwPLLN      = aPLL_ConfigHSI[bPLLIndex].PLLN;
  uwPLLR      = aPLL_ConfigHSI[bPLLIndex].PLLR;
  uwLatency   = aPLL_ConfigHSI[bPLLIndex].Latency;
  
  /* Set new PLL config Index */
  bPLLIndex = (bPLLIndex + 1) % RCC_PLL_CONFIG_NB;
  
  /* Set variable to request of PLL config change */
  bButtonPress = 1;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
