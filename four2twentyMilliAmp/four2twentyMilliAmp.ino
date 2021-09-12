/*
  4-20mA T click calibration
 */
#include <SPI.h>

// Arduino UNO with Mikroe Arduino Uno Click shield
// 4-20mA is placed in socket #2
// CS   is pin 9
// SCK  is pin 13
// MISO is pin 12
// MOSI is pin 11
#define DAC_CS 21

// Used to receive data from serial port
int received;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize serial
  Serial.begin(115200);
  // initialize SPI
  SPI.begin();
  pinMode(DAC_CS, OUTPUT);

  Serial.println("started");
}

void loop() {
  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(1000);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //delay(1000);                       // wait for a second

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




  // static int fisken = 0;
  // delay(1);
  // if (++fisken >= 2048) fisken = 0;
  // //fisken = constrain(++fisken, 0 , 2048);
  // set_DAC(fisken);


  // static int printCount = 0;
  // printCount++;
  // if (printCount % 200 == 3)
  // {
  //   Serial.println(fisken);
  // }
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
