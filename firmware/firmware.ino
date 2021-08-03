#include <Bitcoin.h>
#include <LiquidCrystal.h>
#include <stdint.h>

// For testbench 1:
int button1 = D1;
int button2 = D0;
int led_board = LED_BUILTIN;
const int PIN_RS = D3, PIN_E = D2, PIN_D4 = D5, PIN_D5 = D6, PIN_D6 = D7, PIN_D7 = D8;
LiquidCrystal lcd(PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

#ifdef ESP8266
#include <ESP8266TrueRandom.h>
#endif

#define DEBUG

PrivateKey pk;
PublicKey pubk;
bool no_wallet_created_so_far = true;
String pkstr = "", pkwif = "", pubkstr = "";

uint8_t generate_random_8bit()
{
#ifdef ESP32
    return (uint8_t)esp_random();
#elif defined(ESP8266)
    return (uint8_t)ESP8266TrueRandom.randomByte();
#else
    // attention: this is pseudo random - do not use for actual wallet generation.
    return random(256);
#endif
}

void setup()
{
    // setup leds
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    pinMode(led_board, OUTPUT);
    // setup buttons

    lcd.begin(20, 4);

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
        lcd.setCursor(0, 0);
        //         01234567890123456789
        lcd.print("BTC Wallet Gen");
        lcd.setCursor(0, 1);
        lcd.print("B1:gen B2:next");
        lcd.setCursor(0, 2);
        lcd.print("by Felix");
        lcd.setCursor(0, 3);
        lcd.print("Weichselgartner");
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
        if (page == 1)
        {

            lcd.setCursor(0, 0);
            //         01234567890123456789
            lcd.print("Wallet Import Format");
            lcd.setCursor(0, 1);
            lcd.print(pkwif);
        }
        else
        {
            lcd.setCursor(0, 0);
            //         01234567890123456789
            lcd.print("Public Key");
            lcd.setCursor(0, 1);
            lcd.print(pubkstr);
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
        Serial.println("next page");
        action = 2;
    }

    edge_detector_lcd = button1_state;
}
