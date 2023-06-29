#include <FIRFilter.h>
#include <IIRFilter.h>
int UpperThreshold = 560;
int LowerThreshold = 550;
int reading = 0;
float BPM = 0.0;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;
const uint8_t ECG_pin = A0;
const int16_t DC_offset = 450;

// 50 Hz notch
const double b_notch[] = { 1.39972748302835,  -1.79945496605670, 1.39972748302835 };

// 35 Hz Butterworth low-pass
const double b_lp[] = { 0.00113722762905776, 0.00568613814528881, 0.0113722762905776,  0.0113722762905776,  0.00568613814528881, 0.00113722762905776 };
const double a_lp[] = { 1, -3.03124451613593, 3.92924380774061,  -2.65660499035499, 0.928185738776705, -0.133188755896548 };

// 0.3 Hz high-pass
const double b_hp[] = { 1, -1 };
const double a_hp[] = { 1, -0.995 };

FIRFilter notch(b_notch);
IIRFilter lp(b_lp, a_lp);
IIRFilter hp(b_hp, a_hp);

void setup() {
  Serial.begin(9600);
   pinMode(10, INPUT); // Setup for leads off detection LO +
  pinMode(11, INPUT); 
}

void loop() { 
//  if((digitalRead(11) == 1)){
//    Serial.println('!');
//  }
//  else { 
  int reading = analogRead(ECG_pin);
  double filtered = notch.filter(
                      lp.filter(
                      hp.filter(reading - DC_offset)));
  reading = round(filtered) + DC_offset;
  Serial.println(reading);
  if(reading > UpperThreshold && IgnoreReading == false){
    if(FirstPulseDetected == false){
      FirstPulseTime = millis();
      FirstPulseDetected = true;
    }
    else{
      SecondPulseTime = millis();
      PulseInterval = SecondPulseTime - FirstPulseTime;
      FirstPulseTime = SecondPulseTime;
    }
    IgnoreReading = true;
  }

  // Heart beat trailing edge detected.
  if(reading < LowerThreshold && IgnoreReading == true){
    IgnoreReading = false;
  }  
  if(PulseInterval > 0){
    BPM = (1.0/PulseInterval) * 60.0 * 1000;
    PulseInterval = 0;
    Serial.println("BPM" + String(int(BPM)));
  }
//    
//  Serial.print("\n");
//  Serial.print(",");
//  Serial.print("\n");
  
  }
