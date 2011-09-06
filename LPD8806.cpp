#include "LPD8806.h"

//Example to control LPD8806-based RGB LED Strips
// (c) Adafruit industries
// MIT license

/*****************************************************************************/

Pixel::Pixel() {
  stepsRemaining = 0;
  id = 0;

  setColor(0, 0, 0);
}

bool Pixel::transitioning() {
  if (stepsRemaining == 0) {
    return false;
  } else {
    return true;
  }
}

Pixel::Pixel(Pixel *p) {
  red = p->red;
  green = p->green;
  blue = p->blue;
  
  id = p->id;

  stepsRemaining = p->stepsRemaining;

  transitionRed = p->transitionRed;
  transitionGreen = p->transitionGreen;
  transitionBlue = p->transitionBlue;

  transitionRedStep = p->transitionRedStep;
  transitionGreenStep = p->transitionGreenStep;
  transitionBlueStep = p->transitionBlueStep;
}

void Pixel::transition() {
  if (stepsRemaining == 0) return;
  stepsRemaining = stepsRemaining - 1;

  red = transitionRed + (transitionRedStep * stepsRemaining);
  green = transitionGreen + (transitionGreenStep * stepsRemaining);
  blue = transitionBlue + (transitionBlueStep * stepsRemaining);
}

void Pixel::transitionTo(uint8_t steps, byte r, byte g, byte b) {
  stepsRemaining = steps;

  transitionRed = r;
  transitionGreen = g;
  transitionBlue = b;

  transitionRedStep = (float) (red - transitionRed) / stepsRemaining;
  transitionGreenStep = (float) (green - transitionGreen) / stepsRemaining;
  transitionBlueStep = (float) (blue - transitionBlue) / stepsRemaining;
}

void Pixel::transitionTo(uint8_t steps, Color color) {
  //transitionTo(steps, color.red, color.green, color.blue);
  transitionTo(steps, random(20), random(20), random(20));
}

Color Pixel::color() {
  return Color(red, green, blue);
}

void Pixel::setColor(Color *c) {
  green = c->green;
  red = c->red;
  blue = c->blue;
}

void Pixel::setColor(byte r, byte g, byte b) {
  red = r;
  green = g;
  blue = b;
}

void Pixel::copy(Pixel *p) {
  setColor(p->red, p->green, p->blue);

  stepsRemaining = p->stepsRemaining;

  transitionRed = p->transitionRed;
  transitionGreen = p->transitionGreen;
  transitionBlue = p->transitionBlue;

  transitionRedStep = p->transitionRedStep;
  transitionGreenStep = p->transitionGreenStep;
  transitionBlueStep = p->transitionBlueStep;
}

LPD8806::LPD8806(uint16_t n, uint8_t dpin, uint8_t cpin) {
  dataPin = dpin;
  clockPin = cpin;
  numLEDs = n;

  // malloc 3 bytes per pixel so we dont have to hardcode the length
  pixel_data = (uint8_t *)malloc(numLEDs * 3); // 3 bytes per pixel
}

void LPD8806::begin(void) {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

uint16_t LPD8806::numPixels(void) {
  return numLEDs;
}

// Basic, push SPI data out
void LPD8806::write8(uint8_t d) {
  for (uint8_t i=0; i<8; i++) {
     if (d & _BV(7-i))
       digitalWrite(dataPin, HIGH);
     else
       digitalWrite(dataPin, LOW);
     digitalWrite(clockPin, HIGH);
     digitalWrite(clockPin, LOW); 
  }
}

// This is how data is pushed to the strip. 
// Unfortunately, the company that makes the chip didnt release the 
// protocol document or you need to sign an NDA or something stupid
// like that, but we reverse engineered this from a strip
// controller and it seems to work very nicely!
void LPD8806::show(void) {
  uint16_t i;
  
  // get the strip's attention
  write8(0);
  write8(0);
  write8(0);
  write8(0);

  // write 24 bits per pixel
  for (i=0; i<numLEDs; i++ ) {
    pixels[i].transition();
    write8(pixels[i].green | 0x80);
    write8(pixels[i].red | 0x80);
    write8(pixels[i].blue | 0x80);
  }
  
  // to 'latch' the data, we send just zeros
  for (i=0; i < (numLEDs*2); i++ ) {
    write8(0); 
    write8(0); 
    write8(0);     
  }
  
  // we need to have a delay here, 10ms seems to do the job
  // shorter may be OK as well - need to experiment :(
  delay(10);
}

// store the rgb component in our array
void LPD8806::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if (n > numLEDs) return;

  pixels[n].green = g | 0x80;
  pixels[n].red = r | 0x80;
  pixels[n].blue = b | 0x80;
}

uint16_t LPD8806::index(uint16_t i) {
  return i % numPixels();
}

Color::Color(byte r, byte g, byte b) {
  red = r;
  blue = b;
  green = g;
}

Color::Color() {
  byte red, green, blue;
  red = random(1, 21);
  green = random(21 - red);
  blue = 20 - red - green;
  Color(red, green, blue);
}
