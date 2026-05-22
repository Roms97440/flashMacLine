#include "Scheduler.h"
//#include "Runner.h"
#include "task/RMonitor.h"
#include "Logger.h"
#include "sensor/Ina219.h"
#include "actuator/Led.h"
#include "actuator/Buzzer.h"
#include "sensor/Button.h"
#include "Pindef.h" //definitions des pins du robot
#include "sensor/Qtr-3RC.h"


int EN1 = 6;
int EN2 = 5;  //Roboduino Motor shield uses Pin 9
int IN1 = 7;
int IN2 = 4; //Latest version use pin 4 instead of pin 8
unsigned long dtpred = 0;

unsigned int sensors[3];
int RIGHT = 2;
int CENTER = 1;
int LEFT = 0;
float GAIN = 1;
int MAX_SPEED = 255;
int BASE_SPEED = 100;

float Kp=3,Ki=0.015,Kd=0.2;

float error = 0;
float integral = 0;
float lastError = 0;

void Motor1(int pwm, boolean reverse) {
  analogWrite(EN1, pwm); //set pwm control, 0 for stop, and 255 for maximum speed
  if (reverse)  {
    digitalWrite(IN1, HIGH);
  }
  else  {
    digitalWrite(IN1, LOW);
  }
}

void Motor2(int pwm, boolean reverse) {
  analogWrite(EN2, pwm);
  if (reverse)  {
    digitalWrite(IN2, HIGH);
  }
  else  {
    digitalWrite(IN2, LOW);
  }
}

void capteur(unsigned int capteur[3]){
  bool c1 = false,c2 = false,c3 = false;
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  delayMicroseconds(10);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  unsigned long start1 = micros();
  unsigned long start2 = micros();
  unsigned long start3 = micros();
  while(!(c1&&c2&&c3)){
    if((!c1 && digitalRead(11) == LOW) ||(micros()-start1 >2000)){
      capteur[0] = micros()-start1;
      c1 = true;
    }
    if((!c2 && digitalRead(12) == LOW) ||(micros()-start2 >2000)){
      capteur[1] = micros()-start2;
      c2 = true;
    }
    if((!c3 && digitalRead(13) == LOW) ||(micros()-start3 >2000)){
      capteur[2] = micros()-start3;
      c3 = true;
    }
  }
  capteur[0]=capteur[0]/7.3;
  capteur[1]=capteur[1]/4.2;
  capteur[2]=capteur[2]-20;

  capteur[0]=capteur[0]>50;
  capteur[1]=capteur[1]>50;
  capteur[2]=capteur[2]>50;
}

int clamp(int value, int min, int max){
  return min(max(value,min),max);
}

// void setup() {
//   Serial.begin(9600);
//   for (int i = 4; i <= 7; i++) //For Arduino Motor Shield
//     pinMode(i, OUTPUT);  //set pin 4,5,6,7 to output mode
// }

//Version scheduleur : 
class Algo : public Task { //la classe spécifique de cette tâche
  protected :
    //... données internes de cette tâche ...
  public : 
    SETNAME("BonjourT")
    Algo() : Task(250){ //on fixe ici la rythmique 
                //-> l'activité va se déclencher toutes les PERIODE millisecondes
        //... valeurs de départ des données internes ...
    }
    void init() override {
      Serial.begin(9600);
      for (int i = 4; i <= 7; i++) //For Arduino Motor Shield
      pinMode(i, OUTPUT);  //set pin 4,5,6,7 to output mode
    }
    void run() override {
      unsigned long dt = micros()-dtpred;
      dtpred = dt;
      int x, delay_en;
      char val;

      /*
      while (1)  {
        val = Serial.read();
        if (val != -1)    {
          switch (val)      {
            case 'z'://Move ahead
              Motor1(255, false); //You can change the speed, such as Motor(50,true)
              Motor2(255, true);
              break;
            case 's'://move back
              Motor1(255, true);
              Motor2(255, false);
              break;
            case 'q'://turn left
              Motor1(255, false);
              Motor2(255, false);
              break;
            case 'd'://turn right
              Motor1(255, true);
              Motor2(255, true);
              break;
            case 'x'://stop
              Motor1(0, false);
              Motor2(0, false);
              break;
          }
        }
        

      
      }
      */

      capteur(sensors);
      error = (float)(sensors[RIGHT] - sensors[LEFT]) / (sensors[LEFT] + sensors[CENTER] + sensors[RIGHT]);
      
      // Terme Proportionnel : réaction immédiate à l'écart
      float P = Kp * error;

      // Terme Intégral : corrige les erreurs persistantes (biais)
      integral += error * (float)dt/1000000;
      integral = clamp(integral, -10, 10); // anti-windup
      float I = Ki * integral;

      // Terme Dérivé : anticipe et amortit les oscillations
      float D = Kd * (error - lastError) / dt;
      lastError = error;
      float correction = P + I + D;

      // Appliquer aux moteurs
      int motorLeft  = clamp(BASE_SPEED + correction * GAIN, 0, MAX_SPEED);
      int motorRight = clamp(BASE_SPEED - correction * GAIN, 0, MAX_SPEED);

      Motor2(motorLeft,false);
      Motor1(motorRight,true);
      

      for (uint8_t i = 0; i < 3; i++)
      {
        Serial.print(sensors[i]);
        Serial.print('\t');
      }
      Serial.println(" |");
    }
};
Algo algo; //activation de la tâche

// void loop() {
//   unsigned long dt = micros()-dtpred;
//   dtpred = dt;
//   int x, delay_en;
//   char val;

//   /*
//   while (1)  {
//     val = Serial.read();
//     if (val != -1)    {
//       switch (val)      {
//         case 'z'://Move ahead
//           Motor1(255, false); //You can change the speed, such as Motor(50,true)
//           Motor2(255, true);
//           break;
//         case 's'://move back
//           Motor1(255, true);
//           Motor2(255, false);
//           break;
//         case 'q'://turn left
//           Motor1(255, false);
//           Motor2(255, false);
//           break;
//         case 'd'://turn right
//           Motor1(255, true);
//           Motor2(255, true);
//           break;
//         case 'x'://stop
//           Motor1(0, false);
//           Motor2(0, false);
//           break;
//       }
//     }
  
//   }
//   */

//   capteur(sensors);
//   error = (float)(sensors[RIGHT] - sensors[LEFT]) / (sensors[LEFT] + sensors[CENTER] + sensors[RIGHT]);
  
//   // Terme Proportionnel : réaction immédiate à l'écart
//   float P = Kp * error;

//   // Terme Intégral : corrige les erreurs persistantes (biais)
//   integral += error * (float)dt/1000000;
//   integral = clamp(integral, -10, 10); // anti-windup
//   float I = Ki * integral;

//   // Terme Dérivé : anticipe et amortit les oscillations
//   float D = Kd * (error - lastError) / dt;
//   lastError = error;
//   float correction = P + I + D;

//   // Appliquer aux moteurs
//   int motorLeft  = clamp(BASE_SPEED + correction * GAIN, 0, MAX_SPEED);
//   int motorRight = clamp(BASE_SPEED - correction * GAIN, 0, MAX_SPEED);

//   Motor2(motorLeft,false);
//   Motor1(motorRight,true);

//   for (uint8_t i = 0; i < 3; i++)
//   {
//     Serial.print(sensors[i]);
//     Serial.print('\t');
//   }
//   Serial.println(" |");

//   delay(250);
  
// }