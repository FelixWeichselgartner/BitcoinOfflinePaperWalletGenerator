#include <Bitcoin.h>
#include <stdint.h>

// test version is only using pseudo random numbers as proof of concept
uint8_t generate_random_8bit()
{
    return random(255);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("starting");

    randomSeed(analogRead(0));

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

    Serial.println("private key: " + pkstr);

    PrivateKey pk(pkbytes);
    PublicKey pubk = pk.publicKey();

    Serial.println("public key (legacy, segwit, nestedSwegwit):");
    Serial.println(pubk.legacyAddress());
    Serial.println(pubk.segwitAddress());
    Serial.println(pubk.nestedSegwitAddress());
    Serial.println("end");
}

void loop()
{

}
