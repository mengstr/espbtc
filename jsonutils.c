//
//
//

#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "json/jsonparse.h"

#include "jsonutils.h"

// Functions missing in SDK declarations :-(
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));

#define JSON_TYPE_OBJECT_END '}'
#define JSON_TYPE_COMMA      ','

//
// Parse a json buffer and return value of the selected key. See end of file 
// for example of an json file and the corresponding key-paths
//
// Inputs:
//    char *json          - The json string
//    char *searchKey     - Full key-path to search for
//    char *result,       - Pointer to pre-allocated buffer to store result
//    uint16_t resultLen  - Length of the bufer
//
// Returns:
//    0 - Key not found, 1 - Key found, data is in *result
//
uint8_t ICACHE_FLASH_ATTR ExtractFromJson(
      char *json, 
      char *searchKey, 
      char *result, 
      uint16_t resultLen) {

  struct jsonparse_state parser;
  int       jsonType;
  char      buf[300];
  char      keys[100];
  char      *pKeys;
  uint8_t   truncateAtNext=0;

  // Initialize the strings to empty
  keys[0]=0;
  buf[0]=0;
  result[0]=0;

  // Start json parsing and keep track of the full key path
  // as it grows and shrinks while traversing the json tree
  jsonparse_setup(&parser, json, strlen(json));
  while ((jsonType=jsonparse_next(&parser))!=0) {
    //os_printf("jsonType=%d ",jsonType);
    switch (jsonType) {
      case JSON_TYPE_OBJECT:
        truncateAtNext=0;
        break;
      case JSON_TYPE_COMMA:
        truncateAtNext=1;
        break;
      case JSON_TYPE_OBJECT_END:
        pKeys=strrchr(keys,':');
        if (pKeys) *pKeys=0;
        break;
      case JSON_TYPE_PAIR_NAME:
        jsonparse_copy_value(&parser,buf,sizeof(buf));
        pKeys=strrchr(keys,':');
        if (truncateAtNext && pKeys) *pKeys=0;
        truncateAtNext=0;
        strcat(keys,":");
        strcat(keys,buf);
        jsonparse_next(&parser);
        jsonparse_next(&parser);
        jsonparse_copy_value(&parser, result, resultLen);
        // os_printf("%s\t'%s'\n",keys, result);
        break;
      }
      // Compare the key-variable for the full key-paths for the element
      // we're interested in. If we have the correct key then return with
      // a success result code
      if (strcmp(keys,searchKey)==0) {
        return 1;
      }
  }

  result[0]=0;
  return 0;
}


/*

http://api.coindesk.com/v1/bpi/currentprice.json

{
  "time":{
    "updated":"May 2, 2017 17:08:00 UTC",
    "updatedISO":"2017-05-02T17:08:00+00:00",
    "updateduk":"May 2, 2017 at 18:08 BST"
  },
  "disclaimer":"This data was produced from the CoinDesk Bitcoin Price Index (USD). Non-USD currency data converted using hourly conversion rate from openexchangerates.org",
  "chartName":"Bitcoin",
  "bpi":{
    "USD":{
      "code":"USD",
      "symbol":"&#36;",
      "rate":"1,449.0550",
      "description":"United States Dollar",
      "rate_float":1449.055
    },
    "GBP":{
      "code":"GBP",
      "symbol":"&pound;",
      "rate":"1,121.4295",
      "description":"British Pound Sterling",
      "rate_float":1121.4295
    },
    "EUR":{
      "code":"EUR",
      "symbol":"&euro;",
      "rate":"1,328.6443",
      "description":"Euro",
      "rate_float":1328.6443
    }
  }
}

KEY                   VALUE
---------------------------------------------------------
:time                 'time'
:time:updated         'May 2, 2017 17:08:00 UTC'
:time:updatedISO      '2017-05-02T17:08:00+00:00'
:time:updateduk       'May 2, 2017 at 18:08 BST'
:disclaimer           'This data was produced ...'
:chartName            'Bitcoin'
:bpi                  'bpi'
:bpi:USD              'USD'
:bpi:USD:code         'USD'
:bpi:USD:symbol       '&#36;'
:bpi:USD:rate         '1,449.0550'
:bpi:USD:description  'United States Dollar'
:bpi:USD:rate_float   '1449.055'
:bpi:GBP              'GBP'
:bpi:GBP:code         'GBP'
:bpi:GBP:symbol       '&pound;'
:bpi:GBP:rate         '1,121.4295'
:bpi:GBP:description  'British Pound Sterling'
:bpi:GBP:rate_float   '1121.4295'
:bpi:EUR              'EUR'
:bpi:EUR:code         'EUR'
:bpi:EUR:symbol       '&euro;'
:bpi:EUR:rate         '1,328.6443'
:bpi:EUR:description  'Euro'
:bpi:EUR:rate_float   '1328.6443'


123 78            =: time
58 123 78         =: time:updated
58 34 44 78       =: time:updatedISO
58 34 44 78       =: time:updateduk
58 34 125 44 78   =: disclaimer
58 34 44 78       =: chartName
58 34 44 78       =: bpi
58 123 78         =: bpi:USD
58 123 78         =: bpi:USD:code
58 34 44 78       =: bpi:USD:symbol
58 34 44 78       =: bpi:USD:rate
58 34 44 78       =: bpi:USD:description
58 34 44 78       =: bpi:USD:rate_float
58 48 125 44 78   =: bpi:GBP
58 123 78         =: bpi:GBP:code
58 34 44 78       =: bpi:GBP:symbol
58 34 44 78       =: bpi:GBP:rate
58 34 44 78       =: bpi:GBP:description
58 34 44 78       =: bpi:GBP:rate_float
58 48 125 44 78   =: bpi:EUR
58 123 78         =: bpi:EUR:code
58 34 44 78       =: bpi:EUR:symbol
58 34 44 78       =: bpi:EUR:rate
58 34 44 78       =: bpi:EUR:description
58 34 44 78       =: bpi:EUR:rate_float
58 48 125 125 125 


*/

