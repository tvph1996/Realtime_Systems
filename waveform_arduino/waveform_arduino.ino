#include "Waveforms.h"

uint16_t dmaBuffer[120]; 

// Waveform State
volatile int currentWaveform = 3; // 0:Sin, 1:Tri, 2:Saw, 3:Sqr
volatile int sampleMode = 0;      // Starts at 30 samples

const int sinSamples[] = {8, 15, 30, 60, 120};
const int triSamples[] = {8, 15, 30, 60, 120};
const int sawSamples[] = {8, 15, 30, 60, 120};
const int sqrSamples[] = {8, 15, 30, 60, 120};

const int* samplesConfig[4] = {sinSamples, triSamples, sawSamples, sqrSamples};
const char* waveNames[4] = {"Sine", "Triangle", "Sawtooth", "Square"};


void setup() {
  Serial.begin(115200);
  while(!Serial);

  updateWaveformBuffer();

  // 1. Enable Clock
  pmc_enable_periph_clk(ID_DACC);

  // 2. Disable PIO on PB16 (Connect DAC to Pin)
  PIOB->PIO_PDR = PIO_PB16; //disable PIO control of this PIN so DAC can use

  // 3. Reset DACC
  dacc_reset(DACC);

  // 4. Configure DACC Mode 
  // changed STARTUP_8 to STARTUP_0 to try and improve Square wave rise time
  DACC->DACC_MR = DACC_MR_TRGEN_DIS | //disable event Triggering for DAC, enter Free Running mode
                  DACC_MR_USER_SEL_CHANNEL1 |
                  DACC_MR_REFRESH(1) | //force faster refresh 
                  DACC_MR_STARTUP_0; //force faster startup

  // 5. Enable DAC1 Pin
  DACC->DACC_CHER = DACC_CHER_CH1;

  // 6. Configure DMA (PDC) Initial State
  DACC->DACC_TPR  = (uint32_t)dmaBuffer; //target pointer register
  DACC->DACC_TCR  = samplesConfig[currentWaveform][sampleMode]; //target counter register 
  
  DACC->DACC_TNPR = (uint32_t)dmaBuffer; //target next pointer register
  DACC->DACC_TNCR = samplesConfig[currentWaveform][sampleMode]; //target next counter register

  // 7. Enable Interrupt (End of Transmit)
  DACC->DACC_IER = DACC_IER_ENDTX; //raise interrupt when a TX buffer ended
  NVIC_EnableIRQ(DACC_IRQn); // force cpu to listen to DAC interrupt -> 

  // 8. Enable PDC Transmit
  DACC->DACC_PTCR = DACC_PTCR_TXTEN; // PDC (Peripheral DMA Controller)

  printStatus();
}

// ================= LOOP =================
void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    bool changed = false;

    if (input == 't') {
      currentWaveform++;
      if (currentWaveform > 3) currentWaveform = 0;
      changed = true;
    } 
    else if (input == 'n') {
      sampleMode++;
      // Now cycles through 0, 1, 2, 3, 4 (5 modes)
      if (sampleMode > 4) sampleMode = 0;
      changed = true;
    }

    if (changed) {
      reconfigureDMA();
      printStatus();
    }
  }
}


void DACC_Handler() { // default function name is arduino
  if ((dacc_get_interrupt_status(DACC) & DACC_ISR_ENDTX) == DACC_ISR_ENDTX) {
    DACC->DACC_TNPR = (uint32_t)dmaBuffer;
    DACC->DACC_TNCR = samplesConfig[currentWaveform][sampleMode];
  }
}



void updateWaveformBuffer() {
  int samplesNeeded = samplesConfig[currentWaveform][sampleMode];
  int step = 120 / samplesNeeded; 

  for (int i = 0; i < samplesNeeded; i++) {
    int tableIndex = i * step;
    if (tableIndex >= 120) tableIndex = 119;

    // Get the raw value from the header file (0 - 4095)
    uint16_t rawSample = waveformsTable[currentWaveform][tableIndex];

    if (currentWaveform == 3) {
      rawSample = rawSample / 8; //downscale the voltage to 1/8 to reduce the delay
    }

    dmaBuffer[i] = rawSample;
  }
}


void reconfigureDMA() {
  NVIC_DisableIRQ(DACC_IRQn);
  DACC->DACC_PTCR = DACC_PTCR_TXTDIS;

  updateWaveformBuffer();
  int newSize = samplesConfig[currentWaveform][sampleMode];

  DACC->DACC_TPR  = (uint32_t)dmaBuffer;
  DACC->DACC_TCR  = newSize;
  
  DACC->DACC_TNPR = (uint32_t)dmaBuffer;
  DACC->DACC_TNCR = newSize;

  DACC->DACC_PTCR = DACC_PTCR_TXTEN;
  NVIC_EnableIRQ(DACC_IRQn);
}

void printStatus() {
  int currentSamples = samplesConfig[currentWaveform][sampleMode];
  Serial.print("Waveform: ");
  Serial.print(waveNames[currentWaveform]);
  Serial.print(" | Samples: ");
  Serial.print(currentSamples);
  
  if (currentSamples == 8) Serial.print(" (Max Freq)");
  else if (currentSamples == 120) Serial.print(" (Min Freq)");
  
  Serial.println();
}
