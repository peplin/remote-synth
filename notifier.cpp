#include "Arduino.h"
#include "auduino.h"
#include "cJSON.h"

String inputBuffer;

void setup() {
    Serial.begin(115200);
    pinMode(PWM_PIN,OUTPUT);
    // audioOn();
}

void updateSynthControls(int newSyncPhaseInc, int newGrainPhaseInc,
        int newGrainDecay, int newGrain2PhaseInc, int newGrain2Decay) {
    syncPhaseInc = mapPhaseInc(newSyncPhaseInc) / 4;
    grainPhaseInc  = mapPhaseInc(newGrainPhaseInc) / 2;
    grainDecay     = newGrainDecay / 8;
    grain2PhaseInc = mapPhaseInc(newGrain2PhaseInc) / 2;
    grain2Decay    = newGrain2Decay / 4;
}

void loop() {
    if(Serial.available() > 0) {
        inputBuffer += (char)Serial.read();
    }

    int endOfMessage = inputBuffer.indexOf("\r");
    if(endOfMessage != -1) {
        char message[endOfMessage + 1];
        inputBuffer.substring(0, endOfMessage).toCharArray(message,
                sizeof(message));
        Serial.print("Chars are: ");
        for(int i = 0; i < sizeof(message); i++) {
            Serial.println(message[i], HEX);
        }

        cJSON* root = cJSON_Parse(message);
        Serial.println("a");
        if(root != NULL) {
            Serial.println("b");
            cJSON* commandObj = cJSON_GetObjectItem(root, "command");
            Serial.println("c");
            if(commandObj != NULL) {
                char* command = commandObj->valuestring;
                if(!strcmp(command, "play")) {
                    cJSON* options = cJSON_GetObjectItem(root, "options");
                    if(options != NULL) {
                        int newSyncPhaseInc = cJSON_GetArrayItem(options, 0)->valueint;
                        Serial.println("1");
                        int newGrainPhaseInc = cJSON_GetArrayItem(options, 1)->valueint;
                        Serial.println("2");
                        int newGrainDecay = cJSON_GetArrayItem(options, 2)->valueint;
                        Serial.println("3");
                        int newGrain2PhaseInc = cJSON_GetArrayItem(options, 3)->valueint;
                        Serial.println("4");
                        int newGrain2Decay = cJSON_GetArrayItem(options, 4)->valueint;
                        Serial.println("5");
                        updateSynthControls(newSyncPhaseInc, newGrainPhaseInc,
                                newGrainDecay, newGrain2PhaseInc, newGrain2Decay);
                    }
                } else if(!strcmp(command, "stop")) {
                    syncPhaseInc = grainPhaseInc = grainDecay =
                        grain2PhaseInc = grain2Decay = 0;
                }
            }
        }
        inputBuffer = "";
    }
}

// TODO if this is defined anywhere else, it doesn't get called. also, if we try
// to call a function from auduino in here, that doesn't work either?
SIGNAL(PWM_INTERRUPT) {
    uint8_t value;
    uint16_t output;

    syncPhaseAcc += syncPhaseInc;
    if (syncPhaseAcc < syncPhaseInc) {
        // Time to start the next grain
        grainPhaseAcc = 0;
        grainAmp = 0x7fff;
        grain2PhaseAcc = 0;
        grain2Amp = 0x7fff;
        LED_PORT ^= 1 << LED_BIT; // Faster than using digitalWrite
    }

    // Increment the phase of the grain oscillators
    grainPhaseAcc += grainPhaseInc;
    grain2PhaseAcc += grain2PhaseInc;

    // Convert phase into a triangle wave
    value = (grainPhaseAcc >> 7) & 0xff;
    if (grainPhaseAcc & 0x8000) value = ~value;
    // Multiply by current grain amplitude to get sample
    output = value * (grainAmp >> 8);

    // Repeat for second grain
    value = (grain2PhaseAcc >> 7) & 0xff;
    if (grain2PhaseAcc & 0x8000) value = ~value;
    output += value * (grain2Amp >> 8);

    // Make the grain amplitudes decay by a factor every sample (exponential decay)
    grainAmp -= (grainAmp >> 8) * grainDecay;
    grain2Amp -= (grain2Amp >> 8) * grain2Decay;

    // Scale output to the available range, clipping if necessary
    output >>= 9;
    if (output > 255) output = 255;

    // Output to PWM (this is faster than using analogWrite)
    PWM_VALUE = output;
}
