/*******************************************************************
    TinyPICO Arduino Realtime Float Arithmetic Plasma
                              (could be done faster)

    Not the most optimal code, but very easy to follow

    Written by @tetra3dprint
    Twiter: https://twitter.com/tetra3dprint

    Adapted from https://github.com/witnessmenow/WiFi-Tetris-Clock
    by Brian Lough
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/
//------------------------------------------------------------------------------------------------------------------
// Pallet of colors
#include "pallet.h"
//------------------------------------------------------------------------------------------------------------------
// Ticker is used to call the draw function
#include <Ticker.h>
Ticker display_ticker;
//
//------------------------------------------------------------------------------------------------------------------
// enable double buffer for PxMatrix
#define double_buffer
#include <PxMatrix.h>
//
// I'm using Brian's version adapted for the TinyPICO
// https://github.com/witnessmenow/PxMatrix
//
// If you are using a regular ESP32 you may be able to use
// the library manager version https://github.com/2dom/PxMatrix
//
// Adafruit GFX library is a dependancy for the PxMatrix Library
// Can be installed from the library manager
// https://github.com/adafruit/Adafruit-GFX-Library
//------------------------------------------------------------------------------------------------------------------
//
#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 64
#define ROW_PATTERN 32  // 64 x 64 is 1/32
#define SHOW_TIME 5
static float degToRad = 0.01745329251; // deg to rad multiplier
static float f360 = 360;
//------------------------------------------------------------------------------------------------------------------
// Lookup tables
//
float _sin[3600];
float _cos[3600];
//------------------------------------------------------------------------------------------------------------------
PxMATRIX display(MATRIX_WIDTH, MATRIX_HEIGHT, /*P_LAT*/22, /*P_OE*/26/*TinyPICO*/, /*P_A*/19, /*P_B*/23, /*P_C*/18, /*P_D*/5, /*P_E*/15);
//------------------------------------------------------------------------------------------------------------------
void draw() {
  display.display(SHOW_TIME);
}
//------------------------------------------------------------------------------------------------------------------
// OTT FPS counter
#define SHOW_FPS
#ifdef SHOW_FPS
#include "FPS.h"
FPS fps;
#endif
//------------------------------------------------------------------------------------------------------------------
void setup() {
  //Serial.begin(115200);
  setupLookups();

  display.begin(ROW_PATTERN, SPI_BUS_CLK, 27, SPI_BUS_MISO, SPI_BUS_SS); // TinyPICO

  display.setFastUpdate(true);
  display.clearDisplay();
  display.setBrightness(200);
  display.setTextColor(0x0000);
  display.setTextSize(1);
  display_ticker.attach(0.002, draw);

  delay(1000);
#ifdef SHOW_FPS
  fps.getReady();
#endif
}
//------------------------------------------------------------------------------------------------------------------
// Loop variables
float fx, fy, ty, tx, ang1, ang2, ang3, ang4, angOff;
int c;
//------------------------------------------------------------------------------------------------------------------
//#define BASIC_FPS_TEST // Guesstimates max realistic fps in current enviroment and implementation
//
void loop() {
  //------------------------------------------------------------------------------------------------------------------
#ifdef BASIC_FPS_TEST
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      display.drawPixel(x, y, pallet[y]);
    }
  }
  //------------------------------------------------------------------------------------------------------------------
#else
  fx = ((Cos(ang1) + Sin(ang2)) * 20.0 );
  fy = ((Sin(ang4) - Cos(ang3)) * 20.0 );
  for (float y = 0; y < MATRIX_HEIGHT; y++) {
    for (float x = 0; x < MATRIX_WIDTH; x++) {
      tx = (x + fy) * Cos(ty) + Sin(ty) ;
      ty = y * Sin(tx) - Cos(tx) + fx ;
      c = (180 + (int)((Cos(tx) * Sin(tx * 2) * Cos(ty * 3) * Sin(ty * 4) * f360 ) + angOff )) % 1800;
      if (c < 0) c = 0;
      display.drawPixel(x, y, pallet[c % 360]);
    }
  }
  ang1 += 0.5;
  ang2 += 0.7;
  ang3 += 1.3;
  ang4 += 2.0;
  angOff = angOff + 3;
  if (ang1 >= f360) ang1 -= f360;
  if (ang2 >= f360) ang2 -= f360;
  if (ang3 >= f360) ang3 -= f360;
  if (ang4 >= f360) ang4 -= f360;
  if (angOff >= f360) angOff -= f360;
#endif
  //------------------------------------------------------------------------------------------------------------------
#ifdef SHOW_FPS
  fps.tick();
  display.setCursor(0, 0);
  display.println("FPS:" + fps.getStringFPS());
#endif
  //------------------------------------------------------------------------------------------------------------------
  // flip the buffer
  display.showBuffer();
}
//------------------------------------------------------------------------------------------------------------------
void setupLookups() {
  for (int i = 0; i < 3600; i++) {
    _sin[i] = sin((float)i / 10 * degToRad);
    _cos[i] = cos((float)i / 10 * degToRad);
  }
}
//------------------------------------------------------------------------------------------------------------------
// sin()
float Sin(float deg) {
  // horrid while loops
  while (deg < 0) deg += f360;
  while (deg >= 360) deg -= f360;
  return _sin[(int)(deg * 10)];
}
//------------------------------------------------------------------------------------------------------------------
// cos()
float Cos(float deg) {
  // horrid while loops
  while (deg < 0) deg += f360;
  while (deg >= 360) deg -= f360;
  return _cos[(int)(deg * 10)];
}
//------------------------------------------------------------------------------------------------------------------
