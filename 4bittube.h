#ifndef _4BITTUBE_H_
#define _4BITTUBE_H_

extern const uint8_t font4bit[];

uint8_t *Init_4bittube(
        uint8_t _displayCount,
        uint8_t _data,
        uint8_t _clock,
        uint8_t _latch);

// Functions added specifically for this project
void DisplayNumber(uint32_t num);
void Display(uint8_t d0,uint8_t d1,uint8_t d2,uint8_t d3);
void DisplayDots(uint8_t on);

#endif
