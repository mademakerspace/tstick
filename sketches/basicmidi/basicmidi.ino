#include <ADC.h>

ADC *adc = new ADC();

const int touch_steady[] = {3328, 3337, 3323, 3328, 3322, 3334, 3371, 3369, 3389, 3384, 3371, 3302, 3401, 3368, 3383, 3392, 3001, 3002, 2999, 3000, 3001, 3008, 2999, 2968, 2978, 2980, 2946, 2990, 3032, 3080, 3064, 3086, 3140, 3154, 3121, 3089, 3092, 3146, 3144, 3109, 3129, 3121, 3096, 3092, 3159, 3137, 3047, 3070};

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

  adc->adc0->setAveraging(8); // set number of averages
  adc->adc0->setResolution(12); // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED); // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

  // always call the compare functions after changing the resolution!
  //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V
}

int last_touch_raw[NUM_TOUCH] = {0,};
int current_touch_raw[NUM_TOUCH] = {0,};
int last_touch[NUM_TOUCH] = {0,};
int current_touch[NUM_TOUCH] = {0,};

int current_pressure = 0;

int t_vol;
int last_t_vol = 0;

#define VOL_MAX 4000
#define VOL_MIN 3780

void read_keys() {
  current_pressure = analogRead(pressure);
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

  int port_num;
  int port_active;
  int midi_note;
  int last;

  t_vol = map(current_pressure, VOL_MIN, VOL_MAX, 0, 127);
  if (t_vol < 0) {
    t_vol = 0;
  } else if (t_vol > 127) {
    t_vol = 127;
  }

  if (t_vol != last_t_vol) {
    usbMIDI.sendControlChange(7, t_vol, 1);
    last_t_vol = t_vol;
  }


  //Serial.print(current_pressure);
  //Serial.print(" - ");
  //Serial.print(t_vol);
  //Serial.print(" - ");
  for (int i = 0; i < TOUCH_PER_MPLEX * NUM_MUX; i++) {
      last = last_touch[i];

      port_active = current_touch_raw[i] > touch_steady[i] + 100;

      midi_note = 50 + i;
      if (last == 0 && port_active) {
        usbMIDI.sendNoteOn(midi_note, 100, 1);
      } else if (last == 1 && !port_active) {
        usbMIDI.sendNoteOn(midi_note, 0, 1);
      }
      last_touch[i] = port_active;

      //Serial.print(current_touch_raw[i]);
      //Serial.print(port_active);
      //Serial.print("-");
  }
  //Serial.println();
  delay(2);
}
