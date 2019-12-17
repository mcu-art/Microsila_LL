

#include "hw_desc.h"
#include "c8_non_volatile_storage.h"
#include "stm32f1xx.h"
#include "../../core/utils.h"

// https://hubstub.ru/stm32/180-stm32-flash.html
// PM0075 - STM32F103 flash memory programming


#ifdef USE_NON_VOLATILE_STORAGE

// Settings for STM32F103K8T begin -----------------------------------------------

/* Embedded RAM size */
#define EMBEDDED_RAM_SIZE						0x5000  //0x5000 = 20480 bytes

/* Embedded flash memory addr */
#define EMBEDDED_FLASH_BASE_ADDR			0x08000000  

/* Embedded flash memory size */
#define EMBEDDED_FLASH_SIZE						0x10000  //64Kb = 65536 bytes

/* Embedded flash memory number of sectors.
THIS INFORMATION CAN BE FOUND AT Reference manual p.56 "Flash module organization" */
#define EMBEDDED_FLASH_SECTORS_TOTAL		64  
																										
/* All sectors have equall size. 
	Here we treat pages as sectors because in this device a page is the smallest eraseable unit  */		
#define EMBEDDED_FLASH_SECTOR_SIZE 		0x400 // 1024 bytes

// Settings for STM32F103K8T end --------------------------------------------------


typedef enum  {
	NVS_NOT_INIT = 0,
	NVS_READY,
	NVS_ERASE_IN_PROGRESS, 
	NVS_WRITE_IN_PROGRESS,
	NVS_ERROR
} NvsStatus;


typedef struct {
	NvsStatus status;
	uint32_t next_write_index;
	BOOL modified;
	
} NvsDesc;


NvsDesc nvs1 = {0};


static uint16_t nvs1_data[EMBEDDED_FLASH_SECTOR_SIZE/2];


#define LAST_FLASH_SECTOR_ADDR  (EMBEDDED_FLASH_BASE_ADDR + \
	EMBEDDED_FLASH_SECTOR_SIZE * (EMBEDDED_FLASH_SECTORS_TOTAL - 1))



static inline void flash_lock(void) {
	// Set the LOCK Bit to lock the FLASH Registers access
  SET_BIT(FLASH->CR, FLASH_CR_LOCK);
  
#if defined(FLASH_BANK2_END)
  // Set the LOCK Bit to lock the FLASH BANK2 Registers access
  SET_BIT(FLASH->CR2, FLASH_CR2_LOCK);

#endif // FLASH_BANK2_END

}


static OP_RESULT flash_unlock(void) {
	OP_RESULT result = OPR_OK;
	if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
  {
    // Authorize the FLASH Registers access
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);

    // Verify Flash is unlocked
    if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
    {
      result = OPR_ERROR;
    }
  }
	
#if defined(FLASH_BANK2_END)
  if(READ_BIT(FLASH->CR2, FLASH_CR2_LOCK) != RESET)
  {
    // Authorize the FLASH BANK2 Registers access
    WRITE_REG(FLASH->KEYR2, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR2, FLASH_KEY2);
    
    // Verify Flash BANK2 is unlocked
    if(READ_BIT(FLASH->CR2, FLASH_CR2_LOCK) != RESET)
    { result = OPR_ERROR; }
  }
#endif /* FLASH_BANK2_END */
	
	return result;
}

inline BOOL flash_ready(void) 
{ return !(FLASH->SR & FLASH_SR_BSY); }


BOOL check_EOP(void)
{
	if (FLASH->SR & FLASH_SR_EOP)
	{	
		// Two possible explanations:
		//   1) mistake, must be FLASH->SR &= ~(FLASH_SR_EOP);
		//   2) this bit must normally be set, but it's cleared automatically every time 
		//      someone checks it
		FLASH->SR |= FLASH_SR_EOP; 
		return TRUE;
	}	
	return FALSE;
	
}	
//////////////////////////////////////////////////////////////////////////////////////

/*
BOOL flash_erase_page(uint32_t address) 
{
  while(!flash_ready()); // wait until flash is ready for write
  FLASH->CR |= FLASH_CR_PER; // set bit of one page erase mode
  FLASH->AR = address; // set page address
  FLASH->CR |= FLASH_CR_STRT; // start erase
  while(!flash_ready());  // wait until erase complete
  FLASH->CR &= ~FLASH_CR_PER; // clear bit of one page erase mode
  return check_EOP(); // clear end of operation flag
}

 

// sectorId must be in range (0... EMBEDDED_FLASH_SECTORS_TOTAL - 1)
static BOOL erase_flash_sector(uint32_t sectorId ){
	
  
	// Fill EraseInit structure
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t flashSectorError = 0;
	
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = LAST_FLASH_SECTOR_ADDR;
  EraseInitStruct.NbPages = 1;
	
	// Unlock the Flash to enable the flash control register access
  HAL_FLASH_Unlock();
	
	BOOL result = FALSE;
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &flashSectorError) == HAL_OK) result = TRUE;
	return result;
	
	
	
	flash_unlock();
	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = LAST_FLASH_SECTOR_ADDR;
	FLASH->CR |= FLASH_CR_STRT;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
	flash_lock();
}

*/

/*
BOOL load_settings(void) {
	if (nvs_busy()) { return FALSE; } 
	//Set the address of the data
	__IO uint16_t* flashData = (__IO uint16_t*) LAST_FLASH_SECTOR_ADDR;
	//Copy data from the flash to RAM
	for(uint16_t x=0; x<EMBEDDED_FLASH_SECTOR_SIZE/2; ++x){
		nvs1_data[x] = flashData[x];
	}
	return TRUE;
}


OP_RESULT nvs_store(void) {
	OP_RESULT result = OPR_OK;
	
	erase_flash_sector(EMBEDDED_FLASH_SECTORS_TOTAL - 1);
	// Unlock the Flash to enable the flash control register access
  HAL_FLASH_Unlock();
	
	BOOL result = TRUE;	
	uint32_t curAddr = LAST_FLASH_SECTOR_ADDR;
	uint32_t endAddr = curAddr + sizeof(Settings);
	uint64_t dataDword;
	uint16_t reader_index = 0;
	//Write all data to the flash
	while (curAddr < endAddr)
  {
		dataDword = settings_raw_data[reader_index++];  //dataToWrite->data[dataToWrite->readerIndex];
		
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, curAddr, dataDword) != HAL_OK)
    {
      result = FALSE;
    }
		
		curAddr += 2;
  }

  // Lock the Flash to disable the flash control register access (recommended
  //   to protect the FLASH memory against possible unwanted operation) 
  HAL_FLASH_Lock();
	
	
	return result;
}

*/


//////////////////////////////////////////////////////////////////////////////////////////////


void nvs_start_erase(void) {
	
}


OP_RESULT nvs_load(void) {
	
	OP_RESULT result = OPR_OK;
	if (nvs_busy()) { return OPR_BUSY; }
	//Set the address of the data
	__IO uint16_t* flashData = (__IO uint16_t*) LAST_FLASH_SECTOR_ADDR;
	//Copy data from the flash to RAM
	for(uint16_t x=0; x<EMBEDDED_FLASH_SECTOR_SIZE/2; ++x){
		nvs1_data[x] = flashData[x];
	}
	nvs1.status = NVS_READY;
	return result;
}



OP_RESULT nvs_store(void) {
	OP_RESULT result = OPR_OK;
	
	return result;
}



inline BYTE* nvs_get_data(void) {
	
	return (BYTE*) nvs1_data;
}


inline BOOL nvs_busy(void) {
	return ( (nvs1.status == NVS_NOT_INIT) || (nvs1.status == NVS_READY)) ? TRUE : FALSE;
}




inline BOOL nvs_modified(void) {
	return nvs1.modified;
}




void nvs_do_processing(void) {
	if (!nvs_busy()) { return; }
}





#endif  // USE_NON_VOLATILE_STORAGE
