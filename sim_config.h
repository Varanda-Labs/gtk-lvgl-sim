#pragma once

#define LVGL_PERIOD_TIME    10

// MUSIC_DEMO is defined in cmake (via mk_demo_music.sh)

#ifdef MUSIC_DEMO
  #define LCD_WIDTH 480
  #define LCD_HEIGHT 272
  
#else
  //-----------------------------------------------------------
  // Use the following lines to set the screen size for our App
  //-----------------------------------------------------------
  #define LCD_WIDTH 480
  #define LCD_HEIGHT 320
#endif