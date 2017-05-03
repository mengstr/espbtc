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


//
//
//
void ICACHE_FLASH_ATTR DispPass() {
  disp[0]=0b01110011; //P
  disp[1]=0b01110111; //A
  disp[2]=0b01101101; //S
  disp[3]=0b01101101; //S
}

//
//
//
void ICACHE_FLASH_ATTR DispCoin() {
  disp[0]=0b00111001; //C
  disp[1]=0b01011100; //o
  disp[2]=0b00000100; //i
  disp[3]=0b01010100; //n
}

//
//
//
void ICACHE_FLASH_ATTR Disp4digitNumber(uint32_t num) {
  // Cap number a maximum of 9999
  if (num>9999) num=9999;

  // The three first digits might be blank for leading
  // zero suppression
  disp[2]=0x00;
  disp[1]=0x00;
  disp[0]=0x00;

  // Only show numbers > 0 for the first thee digits
  disp[3]=font4bit[num%10];
  if (num>9) disp[2]=font4bit[(num/10)%10];
  if (num>99) disp[1]=font4bit[(num/100)%10];
  if (num>999) disp[0]=font4bit[(num/1000)%10];
}

// The main procsssing task. Currently empty.
void ICACHE_FLASH_ATTR procTask(os_event_t *events) {
//  static uint32_t cnt;
//  Disp4digitNumber(cnt /25);
//  cnt++;
//  if (cnt&(1024)) DispPass(); else DispCoin();
//  if (cnt>10000*25) cnt=0;
  system_os_post(procTaskPrio, 0, 0 );
  os_delay_us(1000);
}

//
// Init/Setup function that gets run once at boot.
//
void ICACHE_FLASH_ATTR user_init() {
  uint8_t i;
  uint32_t rate;

  // Initialize the display module
  disp=Init_4bittube(TUBE_COUNT, TUBE_DATA, TUBE_CLOCK, TUBE_LATCH);
  for (i=0; i<TUBE_COUNT*4; i++) {
    disp[i]=0x40;
  }

  // Startup the feching of the feed from coindesk
  rate=Init_feed();
  Disp4digitNumber(rate);
  
  // Start the main processing task that does nothing in this example
  system_os_task(procTask,
                 procTaskPrio,
                 procTaskQueue,
                 procTaskQueueLen);
  system_os_post(procTaskPrio, 0, 0 );

}
