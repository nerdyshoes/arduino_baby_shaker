#include <Adafruit_CircuitPlayground.h>
#include "MIDIUSB.h"
#include <math.h>

float X, Y, Z;
# define BUFFER_SIZE 32
// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

float calculate_magnitude(float x, float y, float z) {
  float mag_squared = x*x+y*y+z*z;
  return sqrt(mag_squared);
}

int average_magnitude(float *mag_array, int size) {
  float sum = 0;
  for (int i=0; i<size; i++) {
    sum += mag_array[i];
  }
  float average_mag = round(sum / BUFFER_SIZE);
  return int(average_mag);
}

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();

}

int counter = 0;
float prev_mags[BUFFER_SIZE] = {0};


void loop() {

  if (counter == BUFFER_SIZE) {
    int average_mag = average_magnitude(prev_mags, BUFFER_SIZE);

    Serial.println(average_mag);

    controlChange(0, 14, average_mag);
    MidiUSB.flush();

    counter = 0;

  }

  X = CircuitPlayground.motionX();
  Y = CircuitPlayground.motionY();
  Z = CircuitPlayground.motionZ();

  float magnitude = calculate_magnitude(X, Y, Z);
  prev_mags[counter] = magnitude;
  counter++;

  delay(5);

}
