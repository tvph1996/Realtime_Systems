#include "Waveforms.h"

#define oneHzSample 1000000 / maxSamplesNum //maxSamplesNum=120 defined in Waveforms.h

int wave0 = 0;
int wave1 = 0;

int i = 0;
int sample = 78; // default to ~106hz (78*120samples = 9360us cycle time = ~9.4ms cycle time = ~106.8Hz)

const char* waveNames[4] = {"Sine", "Triangular", "Sawtooth", "Square"};
bool maxFreqMode = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Type 'a' to change DAC1, 'b' to change DAC0.");
  Serial.println("Type 'f' for FASTER, 's' for SLOWER.");
  Serial.println("Type 'm' for MAX frequency mode (toggle).");

  analogWriteResolution(12); // only for DAC, not related to this
}

void loop() {
  checkSerialInput();

  analogWrite(DAC0, waveformsTable[wave0][i]);
  analogWrite(DAC1, waveformsTable[wave1][i]);

  i++;
  if (i == maxSamplesNum)
    i = 0;

  if (!maxFreqMode) {
    delayMicroseconds(sample); // control frequency
  }
}

void printFreqInfo() {
  Serial.print("Sample delay: ");
  Serial.print(sample);
  Serial.print(" us | Approx Freq: ");
  float freq = 1000000.0 / (sample * maxSamplesNum);
  Serial.print(freq);
  Serial.println(" Hz");
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
        Serial.println(waveNames[wave1]);
        break;

      case 'b':
        wave0++;
        if (wave0 == 4)
          wave0 = 0;
        Serial.print("DAC0 set to wave: ");
        Serial.println(waveNames[wave0]);
        break;

      case 'f':
        sample = sample / 2;
        if (sample < 1) {
          sample = 1;
        }
        maxFreqMode = false;
        printFreqInfo();
        break;

      case 's':
        if (sample < 1) sample = 1;
        sample = sample * 2;
        if (sample > oneHzSample) {
          sample = oneHzSample;
        }
        maxFreqMode = false;
        printFreqInfo();
        break;
        
      case 'm':
        maxFreqMode = !maxFreqMode;
        if (maxFreqMode) {
          Serial.println("Max Frequency: ON (no delay)");
        } else {
          Serial.println("Max Frequency: OFF");
          printFreqInfo();
        }
        break;
    }
  }
}