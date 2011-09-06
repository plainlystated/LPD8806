#include <WProgram.h>

class Color {
  public:
    Color(byte r, byte g, byte b);
    Color();
    uint8_t red, green, blue;
};

class Pixel {
  public:
    Pixel();
    Pixel(Pixel *p);

    void setColor(byte r, byte g, byte b);
    void setColor(Color *c);
    void copy(Pixel *pixel);
    Color color();
    uint8_t red, green, blue;
    uint8_t transitionRed, transitionGreen, transitionBlue;
    float transitionRedStep, transitionGreenStep, transitionBlueStep;
    uint8_t id;

    void transitionTo(uint8_t steps, byte r, byte g, byte b);
    void transitionTo(uint8_t steps, Color color);
    uint8_t stepsRemaining;
    void transition();
    bool transitioning();
};

class LPD8806 {
 private:
  void write8(byte);
  // the arrays of bytes that hold each LED's 24 bit color values
  uint8_t *pixel_data;
  uint16_t numLEDs, sizeofPixel;
  uint8_t dataPin, clockPin;

 public:
  //Pixel pixels[];
  Pixel pixels[32];
  LPD8806(uint16_t n, uint8_t dpin, uint8_t cpin);
  void begin();
  void show();
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint16_t n, uint32_t c);
  uint16_t numPixels(void);
  uint16_t index(uint16_t i);
};

