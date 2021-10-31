#include <Bitcoin.h>
#include <stdint.h>

//#define LCD_DISPLAY

#ifdef LCD_DISPLAY
#include <LiquidCrystal.h>
// For testbench 1:
int button1 = D4;
int button2 = D0;
const int PIN_RS = D3, PIN_E = D2, PIN_D4 = D5, PIN_D5 = D6, PIN_D6 = D7, PIN_D7 = D8;
LiquidCrystal lcd(PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7);
#else
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
int button1 = D4;
int button2 = D0;
#endif

#ifdef ESP8266
#include <ESP8266TrueRandom.h>
#endif

#define DEBUG

PrivateKey pk;
PublicKey pubk;
bool no_wallet_created_so_far = true;
String pkstr = "", pkwif = "", pubkstr = "";

int led_board = LED_BUILTIN;

uint8_t generate_random_8bit()
{
#ifdef ESP32
    // this is actually untested.
    return (uint8_t)esp_random();
#elif defined(ESP8266)
    return (uint8_t)ESP8266TrueRandom.randomByte();
#else
    // attention: this is pseudo random - do not use for actual wallet generation.
    return random(256);
#endif
}

#ifdef LCD_DISPLAY
#else
void led_write_text(String text) {
    display.clearDisplay();

    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font

    display.write(text.c_str());
    //display.write("Test by Felix Weichselgartner, Habe nun, ach! Philosophie, Juristerei and Medizin, Und leider auch Theologie Durchaus studiert, mit heißem Bemühn. Da steh' ich nun, X");

    display.display();
}
#endif

void setup()
{
    // setup leds
    pinMode(led_board, OUTPUT);
    // setup buttons
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);

#ifdef LCD_DISPLAY
    // setup lcd
    lcd.begin(20, 4);
#else
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    display.clearDisplay();
    display.display();
#endif


#ifdef DEBUG
    // set up serial if in debug mode
    Serial.begin(9600);
    // wait until serial is connected.
    while (!Serial)
        ;
    Serial.println("starting program");
#endif

    // setup random number generation
#ifdef ESP32
    bootloader_random_enable();
#else
    randomSeed(analogRead(0));
#endif
}

// generate private key and public address
void generate_wallet()
{
    const uint8_t pklen = 32;
    uint8_t pkbytes[pklen];

    for (uint8_t i = 0; i < pklen; i++)
    {
        pkbytes[i] = generate_random_8bit();
    }

    pkstr = "";
    for (uint8_t i = 0; i < pklen; i++)
    {
        if (pkbytes[i] <= 0x0f)
        {
            pkstr += "0";
        }
        pkstr += String(pkbytes[i], HEX);
    }

    pk = PrivateKey(pkbytes);

    pkwif = pk.wif();
    pubk = pk.publicKey();
    pubkstr = pubk.nestedSegwitAddress();

#ifdef DEBUG
    // send data to serial bus if on debug mode
    Serial.println("private key: " + pkstr);
    Serial.println("private key wif: " + pk.wif());
    Serial.println("public key nestedSwegwit:");
    //Serial.println(pubk.legacyAddress());
    //Serial.println(pubk.segwitAddress());
    Serial.println(pubk.nestedSegwitAddress());
    Serial.println("wallet created.");
#endif
}

int action = 1;
// action = 0 ==> don't update lcd
// action = 1 ==> init show info
// action = 2 ==> next page
// action = 3 ==> update wallet
int page = 1;
int edge_detector_create_wallet = LOW, edge_detector_lcd = LOW;

void loop()
{
    switch (action)
    {
    case 0:
        break;
    case 1:
#ifdef LCD_DISPLAY
        lcd.clear();
        lcd.setCursor(0, 0);
        //         01234567890123456789
        lcd.print("BTC Wallet Gen");
        lcd.setCursor(0, 1);
        lcd.print("B1:gen B2:next");
        lcd.setCursor(0, 2);
        lcd.print("by Felix");
        lcd.setCursor(0, 3);
        lcd.print("Weichselgartner");
#else
        led_write_text("BTC Wallet Gen\nB1:gen B2:next\nby Felix\nWeichselgartner");
#endif
        break;
    case 2:
        if (page == 1)
        {
            page = 2;
        }
        else
        {
            page = 1;
        }
    case 3:
        // display private key (in wif): pkwif (pkstr - optional)
        // display public address for user: pubkstr
        // on lcd display
        if (!no_wallet_created_so_far)
        {
            if (page == 1)
            {
#ifdef LCD_DISPLAY
                lcd.clear();
                lcd.setCursor(0, 0);
                //         01234567890123456789
                //         --------------------
                //         --------------------
                //         ------------
                lcd.print("Wallet Import Format");
                lcd.setCursor(0, 1);
                String line1 = pkwif.substring(0, 20);
                lcd.print(line1);
                lcd.setCursor(0, 2);
                String line2 = pkwif.substring(20, 40);
                lcd.print(line2);
                lcd.setCursor(0, 3);
                String line3 = pkwif.substring(40);
                lcd.print(line3);
#else
                led_write_text("Wallet Import Format\n" + pkwif);
#endif
            }
            else
            {
#ifdef LCD_DISPLAY
                lcd.clear();
                lcd.setCursor(0, 0);
                //         01234567890123456789
                //         --------------------
                //         --------------
                //         L5jRJp7NnLn6Koc51Ly6
                //         FiGdtBA8dkNd4XCFHLbK
                //         wXkrHs3BTA5n

                lcd.print("Public Key");
                lcd.setCursor(0, 1);
                String line1p = pubkstr.substring(0, 20);
                lcd.print(line1p);
                lcd.setCursor(0, 2);
                String line2p = pubkstr.substring(20);
                lcd.print(line2p);
#else
                led_write_text("Public Key\n" + pubkstr);
#endif
            }
        }

        break;
    }

    action = 0;

    // catch buttons

    int button2_state = digitalRead(button2);
    // create wallet button
    if (button2_state != edge_detector_create_wallet && button2_state == HIGH)
    {
        no_wallet_created_so_far = false;
        generate_wallet();
        action = 3;
    }

    edge_detector_create_wallet = button2_state;

    int button1_state = digitalRead(button1);
    // next page on lcd button
    if (button1_state != edge_detector_lcd && button1_state == HIGH)
    {
        action = 2;
    }

    edge_detector_lcd = button1_state;
}
