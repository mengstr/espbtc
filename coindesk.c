//
//
//

#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "json/jsonparse.h"

#include "coindesk.h"

// Functions missing in SDK declarations :-(
int atoi(const char *nptr);
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));
void *pvPortZalloc(size_t, const char * file, int line);
void ets_timer_disarm(ETSTimer *a);
void ets_timer_setfn(ETSTimer *t, ETSTimerFunc *fn, void *pArg);
void ets_timer_arm_new(ETSTimer *a, int b, int c, int isMstimer);

// Local static storage

// Timer for refreshing the displays
//static os_timer_t ledRefreshTimer;

//
//
//
//void ledRefreshCB(void *arg) {
//}


//
// Inittalize and setup the feed.
//
uint32_t ICACHE_FLASH_ATTR Init_feed() {
  uint32_t rate=0;

  // Create and start a timer for refreshing the LED display 
  // 5 ms gives 1000/(5*4) Hz = 50Hz refresh for a reasonably flicker-
  // free display
//  os_timer_disarm(&ledRefreshTimer);
//  os_timer_setfn(&ledRefreshTimer, (os_timer_func_t *)ledRefreshCB, NULL);
//  os_timer_arm(&ledRefreshTimer, 5, 1);

  struct jsonparse_state parser;
  int       jsonType;
  char      jBuf[300];
  char      key[100];
  char      *pKey;
  uint8_t   removeatnext=0;

  const char *data="{\"time\":{\"updated\":\"May 2, 2017 17:08:00 UTC\",\"updatedISO\":\"2017-05-02T17:08:00+00:00\",\"updateduk\":\"May 2, 2017 at 18:08 BST\"},\"disclaimer\":\"This data was produced from the CoinDesk Bitcoin Price Index (USD). Non-USD currency data converted using hourly conversion rate from openexchangerates.org\",\"chartName\":\"Bitcoin\",\"bpi\":{\"USD\":{\"code\":\"USD\",\"symbol\":\"&#36;\",\"rate\":\"1,449.0550\",\"description\":\"United States Dollar\",\"rate_float\":1449.055},\"GBP\":{\"code\":\"GBP\",\"symbol\":\"&pound;\",\"rate\":\"1,121.4295\",\"description\":\"British Pound Sterling\",\"rate_float\":1121.4295},\"EUR\":{\"code\":\"EUR\",\"symbol\":\"&euro;\",\"rate\":\"1,328.6443\",\"description\":\"Euro\",\"rate_float\":1328.6443}}}";
  int data_len;
  data_len=strlen(data);
#define JSON_TYPE_OBJECT_END '}'
#define JSON_TYPE_COMMA      ','

  // Initialize the key collection string to empty
  key[0]=0;
  // Start json parsing and keep track of the full key path
  jsonparse_setup(&parser, data, data_len);
  while ((jsonType=jsonparse_next(&parser))!=0) {
    //os_printf("jsonType=%d ",jsonType);
    switch (jsonType) {
      case JSON_TYPE_OBJECT:
        removeatnext=0;
        break;
      case JSON_TYPE_COMMA:
        removeatnext=1;
        break;
      case JSON_TYPE_OBJECT_END:
        pKey=strrchr(key,':');
        if (pKey) *pKey=0;
        break;
      case JSON_TYPE_PAIR_NAME:
        jsonparse_copy_value(&parser,jBuf,sizeof(jBuf));
        pKey=strrchr(key,':');
        if (removeatnext && pKey) *pKey=0;
        removeatnext=0;
        strcat(key,":");
        strcat(key,jBuf);
        jsonparse_next(&parser);
        jsonparse_next(&parser);
        jsonparse_copy_value(&parser, jBuf, sizeof(jBuf));
        // os_printf("%s\t'%s'\n",key, jBuf);
        break;
      }
      // Compare the key-variable for the full key-paths for the elements
      // we're interested in. The value of the key is available in the 
      // jBuf variable
      if (strcmp(key,":bpi:USD:rate_float")==0) {
        rate=atoi(jBuf);
      }
  }

  return rate;
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


123 78            =:time
58 123 78         =:time:updated
58 34 44 78       =:time:updatedISO
58 34 44 78       =:time:updateduk
58 34 125 44 78   =:disclaimer
58 34 44 78       =:chartName
58 34 44 78       =:bpi
58 123 78         =:bpi:USD
58 123 78         =:bpi:USD:code
58 34 44 78       =:bpi:USD:symbol
58 34 44 78       =:bpi:USD:rate
58 34 44 78       =:bpi:USD:description
58 34 44 78       =:bpi:USD:rate_float
58 48 125 44 78   =:bpi:GBP
58 123 78         =:bpi:GBP:code
58 34 44 78       =:bpi:GBP:symbol
58 34 44 78       =:bpi:GBP:rate
58 34 44 78       =:bpi:GBP:description
58 34 44 78       =:bpi:GBP:rate_float
58 48 125 44 78   =:bpi:EUR
58 123 78         =:bpi:EUR:code
58 34 44 78       =:bpi:EUR:symbol
58 34 44 78       =:bpi:EUR:rate
58 34 44 78       =:bpi:EUR:description
58 34 44 78       =:bpi:EUR:rate_float
58 48 125 125 125 


*/

