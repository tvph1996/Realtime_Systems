#include "Waveforms.h"

#define oneHzSample 1000000 / maxSamplesNum 

int wave0 = 0; 
int wave1 = 0; 

int i = 0; 
int sample = 78;

void setup() {
  Serial.begin(9600);
  Serial.println("Type 'a' to change DAC1, 'b' to change DAC0.");
  Serial.println("Type 'f' for FASTER, 's' for SLOWER.");

  analogWriteResolution(12); 
}

void loop() {
  checkSerialInput();

  analogWrite(DAC0, waveformsTable[wave0][i]); 
  analogWrite(DAC1, waveformsTable[wave1][i]); 

  i++;
  if (i == maxSamplesNum) 
    i = 0;

  delayMicroseconds(sample); 
}

void checkSerialInput() {
  if (Serial.available() > 0) {
    char inChar = Serial.read();

    switch (inChar) {
      case 'a':
        wave1++;
        if (wave1 == 4)
          wave1 = 0;
        Serial.print("DAC1 set to wave: ");
        Serial.println(wave1);
        break;
        
      case 'b':
        wave0++;
        if (wave0 == 4)
          wave0 = 0;
        Serial.print("DAC0 set to wave: ");
        Serial.println(wave0);
        break;

      case 'f': 
        sample = sample / 2;
        if (sample < 1) { 
          sample = 1;
        }
        Serial.print("Sample delay set to: ");
        Serial.print(sample);
        Serial.println(" us");
        break;

      case 's': 
        if (sample < 1) sample = 1; 
        sample = sample * 2;
        if (sample > oneHzSample) { 
          sample = oneHzSample;
        }
        Serial.print("Sample delay set to: ");
        Serial.print(sample);
        Serial.println(" us");
        break;
    }
  }
}
