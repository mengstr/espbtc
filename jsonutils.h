#ifndef _JSONUTILS_H_
#define _JSONUTILS_H_

uint8_t ICACHE_FLASH_ATTR ExtractFromJson(
      char *json, 
      char *searchKey, 
      char *result, 
      uint16_t resultLen);


#endif
