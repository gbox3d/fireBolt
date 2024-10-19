#include <Arduino.h>
#include <Wire.h>
#include <HX711.h>

const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 13;

const float calibration_factor = -10000.0;

HX711 scale;

float move_dist = 0.25; // cm 단위
float move_time = 2; // millisec 단위

float get_energy(float weight) {
  float distance_m = move_dist / 100.0; // cm에서 m로 변환
  float mass_kg = weight / 1000.0; // g에서 kg로 변환
  float time_s = move_time / 1000.0; // millisec에서 sec로 변환

  float velocity = distance_m / time_s;
  float energy = 0.5 * mass_kg * pow(velocity, 2);

  return energy;
}

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 scale initializing....");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
  Serial.println("HX711 scale initialization ok");

  Serial.println("Enter move_dist (cm) and move_time (ms) separated by a comma (e.g., 0.25,2):");
  while (Serial.available() < 1) {
    delay(100);
  }
  move_dist = Serial.parseFloat();
  Serial.read(); // 쉼표(,)를 읽고 건너뜁니다.
  move_time = Serial.parseFloat();

  Serial.print("move_dist: ");
  Serial.print(move_dist);
  Serial.println(" cm");

  Serial.print("move_time: ");
  Serial.print(move_time);
  Serial.println(" ms");
}

void loop() {
  float weight = scale.get_units();

  float energy = get_energy(weight);

  if (energy > 0.001) {
    
    
    Serial.print("Energy: ");
    Serial.print(energy, 8);
    Serial.println(" J");
  }

  delay(10);
}
