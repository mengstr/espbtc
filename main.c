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

#include "4bittube.h"
#include "coindesk.h"

// Functions missing in SDK declarations :-(
void ets_delay_us(int ms);

// GPIO-pins on the NodeMCU connected to the display modules
#define TUBE_DATA   5   // D1 GPIO5
#define TUBE_LATCH  4   // D2 GPIO4
#define TUBE_CLOCK  0   // D3 GPIO0
// Number of modules connected in the chain
#define TUBE_COUNT  1   // We have one modules in the chain

// Display buffer - memory allocated by the Init_4bittube() function
uint8_t *disp; 

// Defines for the main processing task
#define procTaskPrio        0
#define procTaskQueueLen    1
os_event_t procTaskQueue[procTaskQueueLen];
void procTask(os_event_t *events);

// The main procsssing task. Currently empty.
void ICACHE_FLASH_ATTR procTask(os_event_t *events) {
  system_os_post(procTaskPrio, 0, 0 );
  os_delay_us(10);
}

//
// Init/Setup function that gets run once at boot.
//
void ICACHE_FLASH_ATTR user_init() {
  uint8_t i;

  // Initialize the display module
  disp=Init_4bittube(TUBE_COUNT, TUBE_DATA, TUBE_CLOCK, TUBE_LATCH);
  for (i=0; i<TUBE_COUNT*4; i++) {
    disp[i]=0x40;
  }

  // Startup the feching of the feed from coindesk
  Init_feed();

  // Start the main processing task that does nothing in this example
  system_os_task(procTask, 
                 procTaskPrio,
                 procTaskQueue, 
                 procTaskQueueLen);
  system_os_post(procTaskPrio, 0, 0 );

}
