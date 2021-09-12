#include <SPI.h>

// Arduino UNO with Mikroe Arduino Uno Click shield
// 4-20mA is placed in socket #2
// CS   is pin 9
// SCK  is pin 13
// MISO is pin 12
// MOSI is pin 11
#define DAC_CS 21

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  SPI.begin();
  pinMode(DAC_CS, OUTPUT);

  Serial.println("started");
}

void loop() {
  while (Serial.available() > 0) {
    int received = Serial.parseInt();
    if (Serial.read() == '\n') {
      // constrain the values to 0 - 255 and invert
      received = constrain(received, 0, 4095);

      Serial.print("DAC is set to value : ");
      Serial.println(received);

      set_DAC(received);
    }
  }
}

void set_DAC(int set_value){
  byte first_byte;
  byte second_byte;

  first_byte = (set_value >> 8) & 0x0F;
  first_byte = first_byte | 0x30;
  second_byte = set_value & 0xFF;

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(DAC_CS, 0);
  SPI.transfer(first_byte);
  SPI.transfer(second_byte);
  digitalWrite(DAC_CS, 1);
  SPI.endTransaction();
}
