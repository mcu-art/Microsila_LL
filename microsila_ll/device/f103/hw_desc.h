
/* STM32F103 hardware description
*/

#ifndef STM32F103_HW_DESC_H
#define STM32F103_HW_DESC_H

/******************************************************************************************/ 

/* FLASH AND RAM SECTION BEGIN */


#define EMBEDDED_RAM_SIZE             0x5000  //0x5000 = 20480 bytes
#define EMBEDDED_FLASH_BASE_ADDR      0x08000000  
#define EMBEDDED_FLASH_SIZE           0x10000  //64Kb = 65536 bytes
// This information can be found in Reference Manual p.56 "Flash module organization" 
#define EMBEDDED_FLASH_SECTORS_TOTAL  64  
#define EMBEDDED_FLASH_SECTOR_SIZE    0x400 // 1024 bytes

/* FLASH AND RAM SECTION END */



/* UART SECTION BEGIN */

#define  UART_DEFAULT_BAUD_RATE         115200

// UART1

// Clock
#define UART1_ENABLE_CLOCK            LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define UART1_ENABLE_DMA_CLOCK        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define UART1_ENABLE_GPIO_CLOCK       LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)

// GPIO pins:
// TX: PA9
#define UART1_TX_PORT 			GPIOA
#define UART1_TX_PIN  			LL_GPIO_PIN_9

// RX: PA10
#define UART1_RX_PORT 			GPIOA
#define UART1_RX_PIN  			LL_GPIO_PIN_10

// DMA:
// RX: DMA1 channel 5
// TX: DMA1 channel 4

#define UART1_DMA													DMA1

// RX
#define UART1_DMA_RX_CHANNEL							LL_DMA_CHANNEL_5
#define UART1_DMA_RX_CHANNEL_ENABLED			(LL_DMA_IsEnabledChannel(UART1_DMA, UART1_DMA_RX_CHANNEL))
#define UART1_DMA_RX_IRQn								  DMA1_Channel5_IRQn
#define UART1_DMA_RX_TC_FLAG_ACTIVE   		(LL_DMA_IsActiveFlag_TC5(UART1_DMA))  // Transfer Complete
#define UART1_DMA_RX_HT_FLAG_ACTIVE   		(LL_DMA_IsActiveFlag_HT5(UART1_DMA))  // Half Transfer complete
#define UART1_DMA_CLEAR_RX_TC_FLAG    		(LL_DMA_ClearFlag_TC5(UART1_DMA))  // Transfer Complete
#define UART1_DMA_CLEAR_RX_HT_FLAG    		(LL_DMA_ClearFlag_HT5(UART1_DMA))  // Half Transfer
#define UART1_DMA_CLEAR_RX_GI_FLAG    		(LL_DMA_ClearFlag_GI5(UART1_DMA))  // Global Interrupt
#define UART1_DMA_CLEAR_RX_TE_FLAG    		(LL_DMA_ClearFlag_TE5(UART1_DMA))  // Transfer Error

// TX
#define UART1_DMA_TX_CHANNEL							LL_DMA_CHANNEL_4
#define UART1_DMA_TX_CHANNEL_ENABLED			(LL_DMA_IsEnabledChannel(UART1_DMA, UART1_DMA_TX_CHANNEL))
#define UART1_DMA_TX_IRQn								  DMA1_Channel4_IRQn
#define UART1_DMA_TX_TC_FLAG_ACTIVE   		(LL_DMA_IsActiveFlag_TC4(UART1_DMA))
#define UART1_DMA_TX_TE_FLAG_ACTIVE   		(LL_DMA_IsActiveFlag_TE4(UART1_DMA))
#define UART1_DMA_CLEAR_TX_HT_FLAG    		(LL_DMA_ClearFlag_HT4(UART1_DMA))
#define UART1_DMA_CLEAR_TX_TC_FLAG    		(LL_DMA_ClearFlag_TC4(UART1_DMA))
#define UART1_DMA_CLEAR_TX_GI_FLAG    		(LL_DMA_ClearFlag_GI4(UART1_DMA))
#define UART1_DMA_CLEAR_TX_TE_FLAG    		(LL_DMA_ClearFlag_TE4(UART1_DMA))


/******************************************************************************************/ 
// UART2

// Clock
#define UART2_ENABLE_CLOCK            LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
#define UART2_ENABLE_DMA_CLOCK        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define UART2_ENABLE_GPIO_CLOCK       LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)

// GPIO pins:
// TX: PA2
#define UART2_TX_PORT       GPIOA
#define UART2_TX_PIN        LL_GPIO_PIN_2

// RX: PA3
#define UART2_RX_PORT       GPIOA
#define UART2_RX_PIN        LL_GPIO_PIN_3

// DMA:
// RX: DMA1 channel 6
// TX: DMA1 channel 7

#define UART2_DMA                         DMA1

// RX
#define UART2_DMA_RX_CHANNEL              LL_DMA_CHANNEL_6
#define UART2_DMA_RX_CHANNEL_ENABLED			(LL_DMA_IsEnabledChannel(UART2_DMA, UART2_DMA_RX_CHANNEL))
#define UART2_DMA_RX_IRQn                 DMA1_Channel6_IRQn
#define UART2_DMA_RX_TC_FLAG_ACTIVE       (LL_DMA_IsActiveFlag_TC6(UART2_DMA))
#define UART2_DMA_RX_HT_FLAG_ACTIVE       (LL_DMA_IsActiveFlag_HT6(UART2_DMA))
#define UART2_DMA_CLEAR_RX_TC_FLAG        (LL_DMA_ClearFlag_TC6(UART2_DMA))
#define UART2_DMA_CLEAR_RX_HT_FLAG        (LL_DMA_ClearFlag_HT6(UART2_DMA))
#define UART2_DMA_CLEAR_RX_GI_FLAG        (LL_DMA_ClearFlag_GI6(UART2_DMA))
#define UART2_DMA_CLEAR_RX_TE_FLAG        (LL_DMA_ClearFlag_TE6(UART2_DMA))

// TX
#define UART2_DMA_TX_CHANNEL              LL_DMA_CHANNEL_7
#define UART2_DMA_TX_CHANNEL_ENABLED			(LL_DMA_IsEnabledChannel(UART2_DMA, UART2_DMA_TX_CHANNEL))
#define UART2_DMA_TX_IRQn                 DMA1_Channel7_IRQn
#define UART2_DMA_TX_TC_FLAG_ACTIVE       (LL_DMA_IsActiveFlag_TC7(UART2_DMA))
#define UART2_DMA_TX_TE_FLAG_ACTIVE       (LL_DMA_IsActiveFlag_TE7(UART2_DMA))
#define UART2_DMA_CLEAR_TX_HT_FLAG        (LL_DMA_ClearFlag_HT7(UART2_DMA))
#define UART2_DMA_CLEAR_TX_TC_FLAG        (LL_DMA_ClearFlag_TC7(UART2_DMA))
#define UART2_DMA_CLEAR_TX_GI_FLAG        (LL_DMA_ClearFlag_GI7(UART2_DMA))
#define UART2_DMA_CLEAR_TX_TE_FLAG        (LL_DMA_ClearFlag_TE7(UART2_DMA))


/******************************************************************************************/
// UART3

// Clock
#define UART3_ENABLE_CLOCK            LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#define UART3_ENABLE_DMA_CLOCK        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define UART3_ENABLE_GPIO_CLOCK       LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB)

// GPIO pins:
// TX: PB10
#define UART3_TX_PORT       GPIOB
#define UART3_TX_PIN        LL_GPIO_PIN_10

// RX: PB11
#define UART3_RX_PORT       GPIOB
#define UART3_RX_PIN        LL_GPIO_PIN_11

// DMA:
// RX: DMA1 channel 3
// TX: DMA1 channel 2

#define UART3_DMA                         DMA1

// RX
#define UART3_DMA_RX_CHANNEL              LL_DMA_CHANNEL_3
#define UART3_DMA_RX_CHANNEL_ENABLED			(LL_DMA_IsEnabledChannel(UART3_DMA, UART3_DMA_RX_CHANNEL))
#define UART3_DMA_RX_IRQn                 DMA1_Channel3_IRQn
#define UART3_DMA_RX_TC_FLAG_ACTIVE       (LL_DMA_IsActiveFlag_TC3(UART3_DMA))
#define UART3_DMA_RX_HT_FLAG_ACTIVE       (LL_DMA_IsActiveFlag_HT3(UART3_DMA))
#define UART3_DMA_CLEAR_RX_TC_FLAG        (LL_DMA_ClearFlag_TC3(UART3_DMA))
#define UART3_DMA_CLEAR_RX_HT_FLAG        (LL_DMA_ClearFlag_HT3(UART3_DMA))
#define UART3_DMA_CLEAR_RX_GI_FLAG        (LL_DMA_ClearFlag_GI3(UART3_DMA))
#define UART3_DMA_CLEAR_RX_TE_FLAG        (LL_DMA_ClearFlag_TE3(UART3_DMA))

// TX
#define UART3_DMA_TX_CHANNEL              LL_DMA_CHANNEL_2
#define UART3_DMA_TX_CHANNEL_ENABLED			(LL_DMA_IsEnabledChannel(UART3_DMA, UART3_DMA_TX_CHANNEL))
#define UART3_DMA_TX_IRQn                 DMA1_Channel2_IRQn
#define UART3_DMA_TX_TC_FLAG_ACTIVE       (LL_DMA_IsActiveFlag_TC2(UART3_DMA))
#define UART3_DMA_TX_TE_FLAG_ACTIVE       (LL_DMA_IsActiveFlag_TE2(UART3_DMA))
#define UART3_DMA_CLEAR_TX_HT_FLAG        (LL_DMA_ClearFlag_HT2(UART3_DMA))
#define UART3_DMA_CLEAR_TX_TC_FLAG        (LL_DMA_ClearFlag_TC2(UART3_DMA))
#define UART3_DMA_CLEAR_TX_GI_FLAG        (LL_DMA_ClearFlag_GI2(UART3_DMA))
#define UART3_DMA_CLEAR_TX_TE_FLAG        (LL_DMA_ClearFlag_TE2(UART3_DMA))
 

/* UART SECTION END */




#endif
