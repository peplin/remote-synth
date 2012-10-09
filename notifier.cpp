#include "Arduino.h"

const int SYNTH_PIN = 3;

void setup() {
    Serial.begin(115200);
}

String inputBuffer;

void loop() {
    if(Serial.available() > 0) {
        inputBuffer += (char)Serial.read();
    }

    int endOfMessage = inputBuffer.indexOf("\r");
    if(endOfMessage != -1) {
        int sensorReading = inputBuffer.substring(0, endOfMessage).toInt();
        int pitch = map(sensorReading, 0, 100, 120, 1500);
        tone(SYNTH_PIN, pitch, 200);
        inputBuffer = inputBuffer.substring(endOfMessage + 1);
        delay(1);
    }

}
