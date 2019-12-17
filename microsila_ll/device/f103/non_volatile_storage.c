
#ifdef USE_NON_VOLATILE_STORAGE

#include "hw_desc.h"
#include "../../periph/non_volatile_storage.h"
#include "stm32f1xx.h"
#include "../../core/utils.h"
#include "../../core/crc.h"

// https://hubstub.ru/stm32/180-stm32-flash.html
// PM0075 - STM32F103 flash memory programming


typedef enum  {
	NVS_NOT_INIT = 0,
	NVS_READY,
	NVS_WRITE_IN_PROGRESS,
	NVS_ERROR
} NvsStatus;


NvsStatus nvs1_status = NVS_NOT_INIT;


BYTE nvs1_data[EMBEDDED_FLASH_SECTOR_SIZE];


inline SIZETYPE nvs_size(void) {
	return EMBEDDED_FLASH_SECTOR_SIZE - 4;
}


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


static inline BOOL flash_ready(void) { 
	return !(FLASH->SR & FLASH_SR_BSY); 
}


static inline BOOL check_EOP(void)
{
	if (FLASH->SR & FLASH_SR_EOP)
	{	
		// Two possible explanations why to set FLASH_SR_EOP afterwards:
		//   1) mistake, must be FLASH->SR &= ~(FLASH_SR_EOP);
		//   2) this bit must be set normally, but it's cleared automatically every time 
		//      someone checks it
		FLASH->SR |= FLASH_SR_EOP; 
		return TRUE;
	}	
	return FALSE;
	
}	

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


//////////////////////////////////////////////////////////////////////////////////////////////


void nvs_erase(void) {
	if (nvs_erased()) { return; }
	nvs1_status = NVS_WRITE_IN_PROGRESS;
	flash_unlock();
	flash_erase_page(LAST_FLASH_SECTOR_ADDR);
	flash_lock();
	nvs1_status = NVS_READY;
}



OP_RESULT nvs_load(void) {
	if (nvs_busy()) { return OPR_BUSY; }
	
	//Set the address of the data
	__IO uint16_t* flashData = (__IO uint16_t*) LAST_FLASH_SECTOR_ADDR;
	
	uint16_t* data = (uint16_t*) nvs1_data;
	//Copy data from the flash to RAM
	for(uint16_t x=0; x<EMBEDDED_FLASH_SECTOR_SIZE/2; ++x){
		data[x] = flashData[x];
	}
	nvs1_status = NVS_READY;
	// Check crc which is stored as last 4 bytes in nvs1_data
	if (crc32_equal( nvs1_data, NVS_SIZE, 
		             (const uint32_t*) (nvs1_data + NVS_SIZE) )) {
		return OPR_OK;
	}
	return OPR_CRC_ERROR;
}



OP_RESULT nvs_store(void) {
	OP_RESULT result = OPR_OK;
	// return if flash data is the same as nvs1_buf
	if (nvs_verify() == OPR_OK) { return OPR_OK; }
	nvs1_status = NVS_WRITE_IN_PROGRESS;
	// fill in crc
	uint32_t* crc_dest = (uint32_t*) (nvs1_data + NVS_SIZE);
	*crc_dest = crc32(nvs1_data, NVS_SIZE, 0);
	
	__IO uint16_t* address = (__IO uint16_t*) LAST_FLASH_SECTOR_ADDR;
	
	const SIZETYPE len = EMBEDDED_FLASH_SECTOR_SIZE / 2;
	uint16_t* data = (uint16_t*) nvs1_data;
	
	while(!flash_ready());
	flash_unlock();
  FLASH->CR |= FLASH_CR_PG; // allow flash programming

	for (SIZETYPE i=0; i<len; ++i) {
		if (data[i] == 0xFFFF) { ++address; continue; } // no need to write the default value
		*(address++) = data[i]; // write uint16_t
		while(!flash_ready()); // wait until ready
		if (!check_EOP()) { 
			FLASH->CR &= ~(FLASH_CR_PG); // disallow flash programming
			flash_lock();
			nvs1_status = NVS_ERROR;
			return OPR_ERROR; 
		}
	}
	
	FLASH->CR &= ~(FLASH_CR_PG); // disallow flash programming
	flash_lock();
	nvs1_status = NVS_READY;
	return result;
}


BOOL nvs_erased(void) {
	if (nvs_busy()) { return FALSE; }
	//Set the address of the data
	__IO uint16_t* flashData = (__IO uint16_t*) LAST_FLASH_SECTOR_ADDR;
	
	for(uint16_t x=0; x<EMBEDDED_FLASH_SECTOR_SIZE/2; ++x){
		if (flashData[x] != 0xFFFF) { return FALSE; }
	}
	return TRUE;
}


OP_RESULT nvs_verify(void) {
	OP_RESULT result = OPR_OK;
	if (nvs_busy()) { return OPR_BUSY; }
	//Set the address of the data
	__IO uint16_t* flashData = (__IO uint16_t*) LAST_FLASH_SECTOR_ADDR;
	
	uint16_t* data = (uint16_t*) nvs1_data;
	for(uint16_t x=0; x<EMBEDDED_FLASH_SECTOR_SIZE/2; ++x){
		if (data[x] != flashData[x]) { result = OPR_ERROR; break; }
	}
	return result;
}



inline BYTE* nvs_get_data(void) {
	return nvs1_data;
}


inline BOOL nvs_busy(void) {
	return ( (nvs1_status == NVS_NOT_INIT) || (nvs1_status == NVS_READY)) ? FALSE : TRUE;
}


#endif  // USE_NON_VOLATILE_STORAGE
