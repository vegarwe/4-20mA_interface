#include <SPI.h>

#define ADC_CS (21)

int loop_current;
int received_data;

const int ADC_4mA  = 755;
const int ADC_20mA = 3954;


const int data_min_range = 0;
const int data_max_range = 1023;


void setup() {
  pinMode (ADC_CS, OUTPUT);
  digitalWrite(ADC_CS, 0);
  delay(100);
  digitalWrite(ADC_CS, 1);

  // initialize serial
  Serial.begin(115200);
  // initialize SPI
  SPI.begin();

  Serial.println("started");
}

void loop() {
  // Read the loop current
  loop_current = ReadFrom420mA();
  // Error checking
  if (loop_current == -1)
    Serial.println("Error: open loop");
  else if (loop_current == -2)
    Serial.println("Error: current loop is in short circuit");
  // All is OK, remapping to initial data range
  else { 
    received_data = map(loop_current, ADC_4mA, ADC_20mA, data_min_range, data_max_range);
    Serial.print("V: ");
    Serial.print(" ");
    Serial.print(received_data);
    Serial.print(" ");
    Serial.println(loop_current);
  }
}

unsigned int get_ADC(void) {
  unsigned int result;
  unsigned int first_byte;
  unsigned int second_byte;

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  digitalWrite(ADC_CS, 0);
  first_byte = SPI.transfer(0);
  second_byte = SPI.transfer(0);
  digitalWrite(ADC_CS, 1);
  SPI.endTransaction();

  result = ((first_byte & 0x1F) << 8) | second_byte;
  result = result >> 1;
  return result;
}

int ReadFrom420mA(void) {
  int result;
  int ADC_result;
  float ADC_avrg = 0;
  const int count = 30;
  for (int i = 0; i < count; i++) {
    ADC_result = get_ADC();
    // Measure every 1ms
    delay(1);
    ADC_avrg = ADC_avrg + ADC_result;
  }
  result = (int)(ADC_avrg / count);

  // if (result < (ADC_4mA - 50)){
  //   return -1;
  // }
  // if (result > (ADC_20mA + 50)){
  //   return -2;
  // }
  return result;
}