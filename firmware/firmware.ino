#include <Bitcoin.h>
#include <stdint.h>

/*
#include <LiquidCrystal.h>

const int PIN_RS = 7, PIN_E = 6, PIN_D4 = 5, PIN_D5 = 4, PIN_D6 = 3, PIN_D7 = 2;
LiquidCrystal lcd(PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

void setup() {
  lcd.begin(20,4);
  lcd.setCursor(0, 0);
  //         01234567890123456789
  lcd.print("BTC Wallet Gen");
  lcd.setCursor(0, 1);
  lcd.print("B1:gen B2:next");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print(pkwif);
  lcd.print(pubkstr)
  delay(1000);
}
*/

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
    // setup buttons

#ifdef DEBUG
    // set up serial if in debug mode
    Serial.begin(115200);
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
    const int pklen = 256 / 8;
    uint8_t pkbytes[pklen];

    for (int i = 0; i < pklen; i++)
    {
        uint8_t rn = generate_random_8bit();
        pkbytes[i] = rn;
    }

    pkstr = "";
    for (int i = 0; i < pklen; i++)
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

void loop()
{
    if (no_wallet_created_so_far)
    {
        // show info on how to create a wallet
    }
    else
    {
        // display private key (in wif): pkwif (pkstr - optional)
        // display public address for user: pubkstr
        // on lcd display
    }

    // catch buttons

    // create wallet button
    if (false)
    {

        no_wallet_created_so_far = false;
    }

    // next page on lcd button
    if (false)
    {
    }
}
