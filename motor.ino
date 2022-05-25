#include <HID.h>

#include <filters.h>
#include <Servo.h>

Servo servo;
float DIFICULDADE = 10; //sobe com 20
float RELAX = 5; //desce com 10
// Se deixar os dois iguais ele sempre sobe ou desce

//MES
float  MValue=0; //MES - Myo Electrical Signal
float Voltage;
float MES;

float angle = 0;
//MES Filter
const float f2   = 40.0;  //Cutoff frequency in Hz 
const float f1   = 10.0;  //Cutoff frequency in Hz
const float sampling_time = 0.01; //Sampling time in seconds.
IIR::ORDER  order  = IIR::ORDER::OD3; // Butterworth - Oder (OD1 to OD4)
IIR::TYPE typeHP =  IIR::TYPE::HIGHPASS;
Filter fHP(f1, sampling_time, order, typeHP); //configure high pass filter (40 Hz)
Filter fLP(f2, sampling_time, order); //configure low pass filter (10 Hz)


float movingAverage(float value) {
  const byte nvalues = 100;             // Moving average window size

  static byte current = 0;            // Index for current value
  static byte cvalues = 0;            // Count of values read (<= nvalues)
  static float sum = 0;               // Rolling sum
  static float values[nvalues];

  sum += value;

  // If the window is full, adjust the sum by deleting the oldest value
  if (cvalues == nvalues)
    sum -= values[current];

  values[current] = value;          // Replace the oldest with the latest

  if (++current >= nvalues)
    current = 0;

  if (cvalues < nvalues)
    cvalues += 1;

  return sum/cvalues;
}

void setup() {
  // put your setup code here, to run once:
MValue = (analogRead(A0)); //MES
Serial.begin(9600); //Initialize Serial Monitor

servo.attach(6);
servo.write(0);  // set servo to mid-point

pinMode(LED_BUILTIN, OUTPUT);

}

int ii = 0;
void loop() {
  // put your main code here, to run repeatedly:

digitalWrite(LED_BUILTIN, LOW);

MValue = (analogRead(A0)); 
Voltage = ((((5.0/ 1024.0) *(MValue))));
float filteredval1 = fLP.filterIn(Voltage);
filteredval1 = fHP.filterIn(filteredval1);
filteredval1 =abs(filteredval1);
filteredval1 =filteredval1*100;
delay(10);
MES=movingAverage(filteredval1);
if (ii>10){
  Serial.println(MES);
  ii = 0;
}
ii++;

if(MES > DIFICULDADE){
  angle = angle + 1;
  digitalWrite(LED_BUILTIN, HIGH);
}
if(MES < RELAX){
  angle = angle - 1;
  digitalWrite(LED_BUILTIN, LOW);
}

if(angle > 179){
  angle = 179;
  
}

if(angle < 0){
  angle = 0;
}




//Serial.println(angle);
servo.write(angle);

}
