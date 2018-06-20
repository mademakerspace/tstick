#include <ADC.h>

ADC *adc = new ADC();

const int muxCtrlPin1 = 0;
const int muxCtrlPin2 = 1;
const int muxCtrlPin3 = 2;
const int muxCtrlPin4 = 3;

const int muxReadA = 15;
const int muxReadB = 16;
const int muxReadC = 17;
const int muxes[] = {muxReadA, muxReadB, muxReadC};

const int pressure = 20;
const int piezo = 21;
const int ultrasound = 22;

#define NUM_TOUCH 48
#define TOUCH_PER_MPLEX 16
#define NUM_MUX 3

#define TOUCH_SENS 1000

void setup() {
  pinMode(muxCtrlPin1, OUTPUT);
  pinMode(muxCtrlPin2, OUTPUT);
  pinMode(muxCtrlPin3, OUTPUT);
  pinMode(muxCtrlPin4, OUTPUT);
  Serial.begin(9600);

  adc->setAveraging(8, ADC_0); // set number of averages
  adc->setResolution(12, ADC_0); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_0); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_0); // change the sampling speed

  // always call the compare functions after changing the resolution!
  //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V
}

int last_touch_raw[NUM_TOUCH] = {0,};
int current_touch_raw[NUM_TOUCH] = {0,};
int last_touch[NUM_TOUCH] = {0,};
int current_touch[NUM_TOUCH] = {0,};

int current_pressure = 0;
int current_piezo = 0;
int current_us = 0;

int t_vol;
int last_t_vol = 0;

#define VOL_MAX 4000
#define VOL_MIN 3780

void read_keys() {
  current_pressure = analogRead(pressure);
  current_piezo = analogRead(piezo);
  current_us = analogRead(ultrasound);
  for (int i = 0; i < TOUCH_PER_MPLEX; i++) {
    digitalWrite(muxCtrlPin1, bitRead(i, 0));
    digitalWrite(muxCtrlPin2, bitRead(i, 1));
    digitalWrite(muxCtrlPin3, bitRead(i, 2));
    digitalWrite(muxCtrlPin4, bitRead(i, 3));

    for (int j = 0; j < NUM_MUX; j++) {
      int var = (touchRead(muxes[j]));
      current_touch_raw[j * TOUCH_PER_MPLEX + i] = var;
    }
  }
}

void loop() {
  read_keys();

  Serial.print("pr: ");
  Serial.println(current_pressure);
  Serial.print("pz: ");
  Serial.println(current_piezo);
  Serial.print("us: ");
  Serial.println(current_us);
  //Serial.print("ks: ");
  //for (int i = 0; i < TOUCH_PER_MPLEX * NUM_MUX; i++) {
  //    Serial.print(current_touch_raw[i]);
  //    Serial.print("-");
  //}
  //Serial.println();
  delay(100);
}


