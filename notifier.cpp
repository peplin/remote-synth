#include "Arduino.h"
#include "auduino.h"

void setup() {
    Serial.begin(115200);
    pinMode(PWM_PIN,OUTPUT);
    audioOn();
}

String inputBuffer;
bool playing = false;
int pitch = 0;

void loop() {
    if(Serial.available() > 0) {
        inputBuffer += (char)Serial.read();
    }

    int endOfMessage = inputBuffer.indexOf("\r");
    if(endOfMessage != -1) {
        if(inputBuffer.substring(0, 1) == " ") {
            playing = !playing;
        }
        int message = inputBuffer.substring(0, endOfMessage).toInt();
        pitch = map(message, 0, 100, 0, 1023);
        inputBuffer = inputBuffer.substring(endOfMessage + 1);
    }

    if(playing) {
        // Smooth frequency mapping
        //syncPhaseInc = mapPhaseInc(analogRead(SYNC_CONTROL)) / 4;

        // Stepped mapping to MIDI notes: C, Db, D, Eb, E, F...
        //syncPhaseInc = mapMidi(analogRead(SYNC_CONTROL));

        // Stepped pentatonic mapping: D, E, G, A, B
        syncPhaseInc = mapPhaseInc(pitch) / 4;
        grainPhaseInc  = mapPhaseInc(pitch) / 2;
        grainDecay     = analogRead(pitch) / 8;
        grain2PhaseInc = mapPhaseInc(pitch) / 2;
        grain2Decay    = analogRead(pitch) / 4;
    } else {
        syncPhaseInc = grainPhaseInc = grainDecay =
            grain2PhaseInc = grain2Decay = 0;
    }
}

// TODO if this is defined anywhere else, it doesn't get called. also, if we try
// to call a function from auduino in here, that doesn't work either?
SIGNAL(PWM_INTERRUPT) {
    uint8_t value;
    uint16_t output;
    i++;

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
