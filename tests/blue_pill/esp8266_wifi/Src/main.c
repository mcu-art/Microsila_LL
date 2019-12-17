/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <microsila_ll/core/utils.h>
#include <microsila_ll/core/byte_buf.h>
#include <microsila_ll/core/dbg_console.h>
#include <microsila_ll/periph/uart1.h>
#include <microsila_ll/periph/uart3.h>
#include <microsila_ll/externals/esp8266_wifi.h>
#include "user/leds.h"
#include "test/test.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// One TIM2 tick is set to 25 microseconds, that is 40 ticks per millisecond
//#define MILLIS_TO_SYSTICKS(_TICKS) (_TICKS * 40)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// Counter of 1 millisec units after system start; 

// Safe to be used from main() at any time
//volatile uint64_t sys_millis = 0; 

// Counter of timer ticks (usually of 25 microsec period);
// Requeres lock to be applied when used from main()
volatile uint64_t _systicks_main = 0; 
// Lock to prevent race condition when 
// _systicks_main updated from timer interrupt
volatile BOOL _systicks_main_locked = FALSE;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */

static void _main_routine(void);
static void on_uart1_rx(ByteBuf* data);
uint64_t get_sys_millis(void);

void on_wifi_data_received(const uint8_t channel_id, ByteBuf* data);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void _enable_timer(void) {
      LL_TIM_EnableCounter(TIM2);
      LL_TIM_EnableIT_UPDATE(TIM2);
    }
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),10, 0));

  /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled 
  */
  LL_GPIO_AF_Remap_SWJ_NOJTAG();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
	uart1_init(115200, on_uart1_rx, 3, 3, 3);
	uart3_init(115200, esp8266_on_uart_rx, 3, 3, 3);
	esp8266_init(on_wifi_data_received);

	_enable_timer();

	_main_routine();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
    Error_Handler();  
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(72000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(72000000);
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  /* TIM2 interrupt Init */
  NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM2_IRQn);

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  TIM_InitStruct.Prescaler = 35;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 49;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM2, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_SetOutputPin(INFO_LED_GPIO_Port, INFO_LED_Pin);

  /**/
  GPIO_InitStruct.Pin = INFO_LED_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(INFO_LED_GPIO_Port, &GPIO_InitStruct);

}


/* USER CODE BEGIN 4 */

void delay(const uint32_t millis) {
	uint64_t target_ts = get_sys_millis() + millis;
	while (get_sys_millis() < target_ts) { }
}


uint32_t uint32_mod_perf_test(const uint32_t ITERS) {
	volatile uint32_t count = 0;
	const uint32_t START = 0x11223344;
	uint32_t UNTIL = START + ITERS;
	uint32_t val = 40;
	for (uint32_t i=START; i<UNTIL; ++i) {
		if (! (i%val)) { count++; }
	}
	return count;
}


uint64_t uint64_mod_perf_test(const uint32_t ITERS) {
	volatile uint64_t count = 0;
	const uint64_t START = 0x1122334455667788ULL;
	uint64_t UNTIL = START + ITERS;
	uint64_t val = 40;
	for (uint64_t i=START; i<UNTIL; ++i) {
		if (! (i%val)) { count++; }
	}
	return count;
}


uint32_t uint32_div_perf_test(const uint32_t ITERS) {
	volatile uint32_t count = 0;
	const uint32_t START = 0x11223344;
	uint32_t UNTIL = START + ITERS;
	uint32_t val = 40;
	for (uint32_t i=START; i<UNTIL; ++i) {
		if ((i/val) == 0x257) { count++; }
	}
	return count;
}


uint64_t uint64_div_perf_test(const uint32_t ITERS) {
	volatile uint64_t count = 0;
	const uint64_t START = 0x1122334455667788ULL;
	uint64_t UNTIL = START + ITERS;
	uint64_t val = 40;
	for (uint64_t i=START; i<UNTIL; ++i) {
		if ((i/val) == 0x2578976) { count++; }
	}
	return count;
}


uint32_t uint32_inc_perf_test(const uint32_t ITERS) {
	volatile uint32_t count = 0;
	const uint32_t START = 0x11223344;
	uint32_t UNTIL = START + ITERS;
	for (uint32_t i=START; i<UNTIL; ++i) {
		++count;
	}
	return count;
}


uint64_t uint64_inc_perf_test(const uint32_t ITERS) {
	volatile uint64_t count = 0;
	const uint64_t START = 0x1122334455667788ULL;
	uint64_t UNTIL = START + ITERS;
	for (uint64_t i=START; i<UNTIL; ++i) {
		++count;
	}
	return count;
}

static void visualize_result(OP_RESULT result) {
	switch (result) {
		case OPR_OK:
			led_signal_success();
		  break;
		case OPR_TIMEOUT:
			led_signal_timeout();
		  break;
		default:
			led_signal_error();
	}
}

void halt_on_error(OP_RESULT result) {
	if (result == OPR_OK) { return; }
	visualize_result(result);
	while (TRUE) { }
}


static void on_uart1_rx(ByteBuf* data) {
	// TODO: process received data
}




// Updates and calculates number of milliseconds
// elapsed from system start
uint64_t get_sys_millis(void) {
	#define TICKS_PER_MILLISEC  40
	// This complexity is intended to avoid 64-bit division and modulo calculation,
	// which are extremely expensive on STM32
	static uint64_t sys_millis = 0;
	static uint64_t systicks_last = 0;
	static uint32_t ticks = 0;
	_systicks_main_locked = TRUE;
	ticks += (uint32_t) (_systicks_main - systicks_last);
	systicks_last = _systicks_main;
	_systicks_main_locked = FALSE;
	sys_millis += ticks / TICKS_PER_MILLISEC;
	ticks %= TICKS_PER_MILLISEC; // store the remainder
	return sys_millis;
}


void on_wifi_data_received(const uint8_t channel_id, ByteBuf* data) {
	dc_printf("CH(%d) received: \n", (uint32_t) channel_id); 
	dc_printbuf(data);
}


static void _main_routine(void) {
	
	//led_signal_disable();
	visualize_result(OPR_OK);
	uart1_enable();
	uart3_enable();
	
	
  uint64_t start_ts;
	
	/*
	start_ts = get_sys_millis();
  dc_print("Esp8266 module test begin...\n");
  //halt_on_error(esp8266_test_all());
  dc_printf("Test complete in %d millisecond(s).\n", get_sys_millis() - start_ts);
	*/
	
	
	const uint32_t ITERS = 100000;
	

  start_ts = get_sys_millis();
	dc_print("uint32_mod_perf_test begin...\n");
	volatile uint32_t r32 = uint32_mod_perf_test(ITERS);
  dc_printf("Test complete in %d millisecond(s). \n", (uint32_t) (get_sys_millis() - start_ts));
	

  start_ts = get_sys_millis();
	dc_print("uint64_mod_perf_test begin...\n");
	volatile uint64_t r64 = uint64_mod_perf_test(ITERS);
  dc_printf("Test complete in %d millisecond(s). \n", (uint32_t) (get_sys_millis() - start_ts));
	
	

  start_ts = get_sys_millis();
	dc_print("uint32_inc_perf_test begin...\n");
	r32 = uint32_inc_perf_test(ITERS);
  dc_printf("Test complete in %d millisecond(s). \n", (uint32_t) (get_sys_millis() - start_ts));
	

  start_ts = get_sys_millis();
	dc_print("uint64_inc_perf_test begin...\n");
	r64 = uint64_inc_perf_test(ITERS);
  dc_printf("Test complete in %d millisecond(s). \n", (uint32_t) (get_sys_millis() - start_ts));
	
	

  start_ts = get_sys_millis();
	dc_print("uint32_div_perf_test begin...\n");
	r32 = uint32_div_perf_test(ITERS);
  dc_printf("Test complete in %d millisecond(s). \n", (uint32_t) (get_sys_millis() - start_ts));
	
  start_ts = get_sys_millis();
	dc_print("uint64_div_perf_test begin...\n");
	r64 = uint64_div_perf_test(ITERS);
  dc_printf("Test complete in %d millisecond(s). \n", (uint32_t) (get_sys_millis() - start_ts));
	
	
  visualize_result(OPR_OK);

	// Reset function stops ongoing transfers and clears all buffers,
	// so that previous operation does not affect next one;
	
	while (TRUE) { 
		delay(1); 
		// uart1_do_processing(); // no rx, no processing
		uart3_do_processing();
		esp8266_main_process(get_sys_millis());
	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


