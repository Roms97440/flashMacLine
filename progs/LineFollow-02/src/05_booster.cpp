//Fichier source secondaire: 
#include "00_config.h"

/*  ===> Patch en cas de blocage du robot (normalement inutile si le terrain est régulier)
  -> à activer si nécessaire dans `00_config.h`
*/

//=====================
#ifdef ENABLE_BOOST

/* #region(close) les #include */
#include "sensor/Ina219.h"
#include "actuator/Buzzer.h"
#include "actuator/Motor.h"
/* #endregion */

/* #region(close) les objets définis dans les autres fichiers */
extern SensorINA219 capteurINA219;
extern Buzzer buzzer;
extern BiMotor motors; //les moteurs sont configurés dans le fichier 01_main.cpp
/* #endregion */

// ==== constantes de réglage =============
constexpr uint8_t boost = 20; 
constexpr int seuilBoost = 300; 
  //spécifique au boost cumulatif :
constexpr uint8_t maxBoost = 3;
constexpr int seuilCumulate = seuilBoost-20;  
// ====  ====  ====  ====  ====  ====  ==== 

//Code mono-boost
void monoBoost(){
  if(capteurINA219.courant()>=seuilBoost){ 
      buzzer.notif(400);
      motors.boost(boost);
  }    
}

//Code boost cumulatif
uint8_t count_boost=0;
void cumulBoost(){
  if(capteurINA219.courant()>= (count_boost ? seuilCumulate : seuilBoost)){
    if(count_boost<maxBoost) count_boost++;
      buzzer.notif(400);
      motors.boost(count_boost * 20);
  } else if(count_boost!=0) count_boost--; 
}

//Choix du mode
 #if ENABLE_BOOST == _CUMUL
  TaskCB boostTask(cumulBoost,1000);
 #else
  TaskCB boostTask(monoBoost,1000);
 #endif

#endif
//=====================