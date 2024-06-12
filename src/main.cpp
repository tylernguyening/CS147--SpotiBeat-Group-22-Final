#include <Arduino.h>

#define SENSOR_PIN 25 // Analog pin connected to the junction of the photoresistor and fixed resistor

int rawValue = 0; // Variable to store raw sensor data
int baseline = 0; // Variable to store baseline light level
int threshold = 50; // Example initial threshold for peak detection
unsigned long lastBeat = 0; // Time of the last beat
float beatsPerMinute = 0;
int beatCount = 0;
const int numReadings = 100; // Number of readings for baseline calculation
int readings[numReadings]; // Array to store readings
int readIndex = 0; // Index of the current reading
int total = 0; // Sum of readings
int average = 0; // Average of readings

void setup() {
  Serial.begin(9600);
  Serial.println();
  // Initialize the readings array
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  // Read the sensor value
  rawValue = analogRead(SENSOR_PIN);

  // Subtract the last reading and add the new reading to the total
  total = total - readings[readIndex];
  readings[readIndex] = rawValue;
  total = total + readings[readIndex];
  
  // Advance to the next position in the array
  readIndex = readIndex + 1;
  
  // If we're at the end of the array, wrap around to the beginning
  if (readIndex >= numReadings) {
    readIndex = 0;
  }
  
  // Calculate the average
  average = total / numReadings;
  
  // Calculate the baseline as the average of the readings
  baseline = average;
  
  // Calculate the difference from the baseline
  int difference = rawValue - baseline;
  
  // Adaptive threshold based on baseline
  threshold = baseline * 0.001;                     // Can be adjusted for advanced noise reduction. Lower the multiplier = more sensitive
  
  // Peak detection
  if (difference > threshold) {
    unsigned long currentTime = millis();
    if ((currentTime - lastBeat) > 300) {           // Ignore false positives by checking minimum interval (300ms)
      unsigned long interval = currentTime - lastBeat;
      beatsPerMinute = 60000.0 / interval;
      lastBeat = currentTime;
      beatCount++;
      if(beatsPerMinute < 150){                     //Further reduce inaccurate results. For this prototype, testing is done at rest so anything above
        Serial.print("Beat detected. BPM: ");       //our average heart rate when doing intense exercise is disregarded.
        Serial.println(beatsPerMinute);
      }
    }
  }

  delay(10); // Delay for stability
}
