#include <atmega328_16mhz_ac_phase_control.h>

// 10-step profile mapped directly to Timer1 ticks (500 = Full ON, 19500 = OFF)
int powerStages[10] = {
  500,   // Stage 0: 500 ticks (~250 µs)
  2278,  // Stage 1 (~1139 µs)
  4056,  // Stage 2 (~2028 µs)
  5834,  // Stage 3 (~2917 µs)
  7612,  // Stage 4 (~3806 µs)
  9390,  // Stage 5 (~4695 µs)
  11168, // Stage 6 (~5584 µs)
  12946, // Stage 7 (~6473 µs)
  14724, // Stage 8 (~7362 µs)
  16500  // Stage 9: 16500 ticks (~8250 µs)
};

void setup() {
  Serial.begin(115200);
  atmega328_16mhz_ac_phase_control.init();
  Serial.println("AC Phase Control Initialized.");
}

void loop() {
  // Step forward through stages (Bright to Dim)
  for(int i = 0; i < 10; i++) {
    atmega328_16mhz_ac_phase_control.set_ac_power(powerStages[i]);
    
    Serial.print("Stage: ");
    Serial.print(i);
    Serial.print(" | Ticks: ");
    Serial.println(powerStages[i]);
    
    delay(1000);
  }

  // Step backward through stages (Dim to Bright)
  for(int i = 9; i >= 0; i--) {
    atmega328_16mhz_ac_phase_control.set_ac_power(powerStages[i]);
    
    Serial.print("Stage: ");
    Serial.print(i);
    Serial.print(" | Ticks: ");
    Serial.println(powerStages[i]);
    
    delay(1000);
  }
}