#include <Bitcoin.h>
#include <stdint.h>

#ifdef ESP8266
#include <ESP8266TrueRandom.h>
#endif


PrivateKey pk;
PublicKey pubk;


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
    Serial.begin(115200);
    Serial.println("starting");

#ifdef ESP32
    bootloader_random_enable();
#else
    randomSeed(analogRead(0));
#endif

    const int pklen = 256 / 8;
    uint8_t pkbytes[pklen];

    for (int i = 0; i < pklen; i++)
    {
        uint8_t rn = generate_random_8bit();
        pkbytes[i] = rn;
    }

    String pkstr = "";
    for (int i = 0; i < pklen; i++) {
        if (pkbytes[i] <= 0x0f) {
            pkstr += "0";
        }
        pkstr += String(pkbytes[i], HEX);
    }

    pk = PrivateKey(pkbytes);

    //Serial.println("private key: " + pkstr);
    Serial.println("private key wif: " + pk.wif());
    
    pubk = pk.publicKey();

    Serial.println("public key nestedSwegwit:");
    //Serial.println(pubk.legacyAddress());
    //Serial.println(pubk.segwitAddress());
    Serial.println(pubk.nestedSegwitAddress());
    Serial.println("end");
}

void loop()
{

}
