//
// A small project that fetces the current BTC excange rate from
// the coindesk API and displays it on a 4-digit LED display
//
// The code is available at https://github.com/SmallRoomLabs/espbtc
//

#include "c_types.h"
#include "os_type.h"
#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"

#include "4bittube.h"
#include "network.h"
#include "jsonutils.h"

// Functions missing in SDK declarations :-(
void ets_delay_us(int ms);
void ets_timer_disarm(ETSTimer *a);
void ets_timer_setfn(ETSTimer *t, ETSTimerFunc *fn, void *pArg);
void ets_timer_arm_new(ETSTimer *a, int b, int c, int isMstimer);
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));

// Wifi settings
#define SSID      "true_homewifi_012"
#define PASSWORD  "00000GZ4"

// GPIO-pins on the NodeMCU connected to the display modules
// and number of modules connected in the chain
#define TUBE_DATA   5   // D1 GPIO5
#define TUBE_LATCH  4   // D2 GPIO4
#define TUBE_CLOCK  0   // D3 GPIO0
#define TUBE_COUNT  1   // We have one modules in the chain

// Display buffer - memory allocated by the Init_4bittube() function
uint8_t *disp;

// Defines for the main processing task
#define procTaskPrio        0
#define procTaskQueueLen    1
os_event_t procTaskQueue[procTaskQueueLen];
void procTask(os_event_t *events);

// Timer handling network connections and requests
static os_timer_t networkTimer;


#define MSG_IDLE 0b10000000,0b10000000,0b10000000,0b10000000 // ....
#define MSG_PASS 0b01110011,0b01110111,0b01101101,0b01101101 // PASS
#define MSG_SSID 0b01101101,0b01101101,0b00000100,0b01011110 // SSid
#define MSG_NOIP 0b01010100,0b01011100,0b00000100,0b01110011 // noiP
#define MSG_FAIL 0b01110001,0b01110111,0b00000100,0b00111000 // FAiL
#define MSG_DNS  0b01011110,0b01010100,0b01101101,0b00000000 // dnS 

//
// Timer callback for network function
//
void ICACHE_FLASH_ATTR NetworkTimerCB(void *arg) {
  static int cnt;
  int netStatus;

    netStatus=wifi_station_get_connect_status();
    switch (netStatus) {
      case STATION_IDLE:
        Display(MSG_IDLE);
        cnt=0;
        break;
      case STATION_CONNECTING:
        Display(MSG_NOIP);
        cnt=0;
        break;
      case STATION_WRONG_PASSWORD:
        Display(MSG_PASS);
        cnt=0;
        break;
      case STATION_NO_AP_FOUND:
        Display(MSG_SSID);
        cnt=0;
        break;
      case STATION_CONNECT_FAIL:
        Display(MSG_FAIL);
        cnt=0;
        break;
      case STATION_GOT_IP:
        if (cnt==0) {
          // If we don't already have the IP then send a DNS request'
          if (hostIp.addr==0) {
            Display(MSG_DNS);
            ResolveHost("api.coindesk.com");
            break;
          } 
          HttpRequest(hostIp,"api.coindesk.com","/v1/bpi/currentprice.json");
        }
        // Do the API lookup every 5 seconds
        if (cnt++>10*5) cnt=0;
        break;
    }

}

//
// The main procsssing task. Currently empty.
//
void ICACHE_FLASH_ATTR procTask(os_event_t *events) {
  system_os_post(procTaskPrio, 0, 0 );
  os_delay_us(10);
}


//
// Init/Setup function that gets run once at boot.
//
void ICACHE_FLASH_ATTR user_init() {

  // Start the Wifi/network
  StartNetwork(SSID, PASSWORD);

  // Initialize the display module
  disp=Init_4bittube(TUBE_COUNT, TUBE_DATA, TUBE_CLOCK, TUBE_LATCH);
  
  // Start a 100ms timer handling network connections and requests
  os_timer_disarm(&networkTimer);
  os_timer_setfn(&networkTimer, (os_timer_func_t *)NetworkTimerCB, NULL);
  os_timer_arm(&networkTimer, 100, 1);

  // Start the main processing task that does nothing in this example
  system_os_task(procTask,
                 procTaskPrio,
                 procTaskQueue,
                 procTaskQueueLen);
  system_os_post(procTaskPrio, 0, 0 );

}



