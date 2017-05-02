//
//
//

#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "json/jsonparse.h"

#include "coindesk.h"

// Functions missing in SDK declarations :-(
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
void ICACHE_FLASH_ATTR Init_feed() {

  // Create and start a timer for refreshing the LED display 
  // 5 ms gives 1000/(5*4) Hz = 50Hz refresh for a reasonably flicker-
  // free display
//  os_timer_disarm(&ledRefreshTimer);
//  os_timer_setfn(&ledRefreshTimer, (os_timer_func_t *)ledRefreshCB, NULL);
//  os_timer_arm(&ledRefreshTimer, 5, 1);
  struct jsonparse_state parser;
  const char *data="{\"time\":{\"updated\":\"May 2, 2017 17:08:00 UTC\",\"updatedISO\":\"2017-05-02T17:08:00+00:00\",\"updateduk\":\"May 2, 2017 at 18:08 BST\"},\"disclaimer\":\"This data was produced from the CoinDesk Bitcoin Price Index (USD). Non-USD currency data converted using hourly conversion rate from openexchangerates.org\",\"chartName\":\"Bitcoin\",\"bpi\":{\"USD\":{\"code\":\"USD\",\"symbol\":\"&#36;\",\"rate\":\"1,449.0550\",\"description\":\"United States Dollar\",\"rate_float\":1449.055},\"GBP\":{\"code\":\"GBP\",\"symbol\":\"&pound;\",\"rate\":\"1,121.4295\",\"description\":\"British Pound Sterling\",\"rate_float\":1121.4295},\"EUR\":{\"code\":\"EUR\",\"symbol\":\"&euro;\",\"rate\":\"1,328.6443\",\"description\":\"Euro\",\"rate_float\":1328.6443}}}";
  int data_len;
  data_len=strlen(data);
  jsonparse_setup(&parser, data, data_len);
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

*/

