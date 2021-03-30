#include <stdint.h>

#ifdef ESP8266
#include <ESP8266TrueRandom.h>
#endif


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
}

void loop()
{
    uint8_t rn = generate_random_8bit();
    Serial.println(rn);
}
