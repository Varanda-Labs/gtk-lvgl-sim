#pragma once

//#define MUSIC_DEMO

#ifdef MUSIC_DEMO
  #define LCD_WIDTH 480
  #define LCD_HEIGHT 272
#else
  #define LCD_WIDTH 480
  #define LCD_HEIGHT 320
#endif