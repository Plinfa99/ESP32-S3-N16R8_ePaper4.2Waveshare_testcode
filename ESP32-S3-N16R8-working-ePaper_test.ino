/*
  Waveshare 4.2" E-Paper (400x300, SSD1683) on ESP32-S3

  Displays a few lines of text + a border. Black & White only.
  Required libs (Arduino IDE -> Library Manager):
    - GxEPD2 (Jean-Marc Zingg)
    - Adafruit GFX Library
    - U8g2_for_Adafruit_GFX

  Wiring (recommended):
    BUSY -> GPIO38
    CS   -> GPIO21
    RST  -> GPIO19
    DC   -> GPIO20
    CLK  -> GPIO7   (shared with SD)
    MOSI -> GPIO9   (shared with SD)
    MISO -> (not used)
    VCC  -> 3V3
    GND  -> GND
*/

#include <SPI.h>
#include <GxEPD2_BW.h>                 // https://github.com/ZinggJM/GxEPD2
#include <U8g2_for_Adafruit_GFX.h>     // https://github.com/olikraus/U8g2_for_Adafruit_GFX

// ---------- Display resolution ----------
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 300

// ---------- Pins ----------
static const uint8_t EPD_BUSY = 38; // BUSY (input)
static const uint8_t EPD_CS   = 21; // CS
static const uint8_t EPD_RST  = 19; // RST
static const uint8_t EPD_DC   = 20; // DC

static const uint8_t EPD_SCK  = 7;  // SCK 
static const int8_t  EPD_MISO = -1; // not used
static const uint8_t EPD_MOSI = 9;  // MOSI 

// ---------- Driver class ----------
#define GxEPD2_DRIVER_CLASS GxEPD2_420_GYE042A87  // SSD1683, 400x300

// ---------- Display object (new-style: pass instance) ----------
GxEPD2_BW<GxEPD2_DRIVER_CLASS, GxEPD2_DRIVER_CLASS::HEIGHT>
  display(GxEPD2_DRIVER_CLASS(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

// U8g2 bridge for fonts
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

// ---------- CPU frequency (optional: throttle for power tests) ----------
#define CPU_FREQUENCY_MHZ 10 // 240 | 160 | 80 | 40 | 20 | 10 MHz

// ---------- Helpers ----------
void InitialiseDisplay() {
  // Start SPI with your pins before initializing the display
  SPI.end();
  SPI.begin(EPD_SCK, EPD_MISO, EPD_MOSI, EPD_CS);

  // If SD is on SPI: de-select its CS (CHANGE 13 to your SD-CS if different!)
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Initialize display
  display.init(115200, true, 2, false);
  display.setRotation(1);      // 0..3 depending on desired orientation
  display.setFullWindow();

  // Connect U8g2 fonts to GFX
  u8g2Fonts.begin(display);
  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
  u8g2Fonts.setFont(u8g2_font_helvB10_tf);

  // Fill white
  display.fillScreen(GxEPD_WHITE);
  Serial.println("Initialise Display done");
}

void drawString(int x, int y, String text) {
  int16_t  x1, y1;
  uint16_t w, h;
  display.setTextWrap(false);
  display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  u8g2Fonts.setCursor(x, y + h);
  u8g2Fonts.print(text);
}

// ---------- Arduino boilerplate ----------
void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("ESP32-S3 + Waveshare 4.2\" E-Paper (SSD1683) DisplayInfo");

  setCpuFrequencyMhz(CPU_FREQUENCY_MHZ);
  Serial.printf("CPU Frequency: %d MHz\n", getCpuFrequencyMhz());

  InitialiseDisplay();

  // Texts
  u8g2Fonts.setFont(u8g2_font_helvB14_tf);
  const uint8_t distance = 20;
  const uint8_t startX = 10;
  const uint8_t startY = 10;

  const char *DISPLAY_TYPE      = "Type: Waveshare 4.2\" E-Paper";
  const char *DISPLAY_INTERFACE = "Interface: SPI | Chip: SSD1683";
  const char *DISPLAY_DRIVER    = "Driver: GxEPD2_420_GYE042A87";
  const char *DISPLAY_SIZE      = "Size: 4.2-inch 400 x 300 pixels";
  const char *DISPLAY_COLORS    = "Colors: Black and White";
  const char *PROGRAM_AUTHOR    = "Author: AndroidCrypto";

  drawString(startX, startY + 0 * distance, DISPLAY_TYPE);
  drawString(startX, startY + 1 * distance, DISPLAY_INTERFACE);
  drawString(startX, startY + 2 * distance, DISPLAY_DRIVER);
  drawString(startX, startY + 3 * distance, DISPLAY_SIZE);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  drawString(startX, startY + 4 * distance, DISPLAY_COLORS);
  drawString(startX, startY + 5 * distance, PROGRAM_AUTHOR);

  // Border
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GxEPD_BLACK);

  // Full refresh
  display.display();

  // Power-friendly end state
  // display.powerOff();
  display.hibernate();
}

void loop() {
}
