#include "Arduino.h"

const int SYNTH_PIN = 3;

void setup() {
    Serial.begin(115200);
    // pinMode(SYNTH_PIN, OUTPUT);
}

void loop() {
    for(int i = 0; i < 255; i++) {
        tone(SYNTH_PIN, i * 50, 200);
    }
}
