#include <stdint.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// d4 has to be disconnected for uploading and start up.
int button1 = 14;
int button2 = 16;

#ifdef LCD_DISPLAY
#else
void led_write_text(String text) {
    display.clearDisplay();

    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font

    display.write(text.c_str());

    display.display();
}
#endif

void setup()
{
    Serial.begin(115200);
    // wait until serial is connected.
    while (!Serial)
        ;
    yield();
    
    Serial.println("starting program");
    
    pinMode(2, OUTPUT);
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);

    Serial.println("starting display");
    
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        while(true);
    }

    Serial.println("display started");

    display.clearDisplay();
    display.display();

    Serial.println("starting loop");
}

int i = 0;

void loop()
{
  led_write_text("BTC Wallet Gen Tester\nB1:" + (String)digitalRead(button1) + " B2:" + (String)digitalRead(button2) + "\nby Felix\nWeichselgartner\n" + String(i));
  i += 1;
  digitalWrite(2, (i % 2) == 0);
  Serial.println(i);
  delay(1000);
}
