//Fichier source secondaire: 
#include "Runner.h"

/*  ===> Tâche de réglage du coefficent delta du moteur faible (si nécessaire)

  -> cette tâche fait avancer le robot en ligne droite. 
  Si une déviation (importante) est constatée, il faut ajuster le coefficient deltaF du moteur le plus faible.
  (celui placé du coté ou va la déviation de trajectoire)

  Protocole de test :
- placer le robot au sol, bien droit avec un champ avant déchaé sur 2m
- appuyer sur le bouton BLEU
- le robot va essayer d'avancer en ligne droite pendant 6 secondes

NB: pour rendre ce calibrage plus efficace, au départ, il faut bien placer la roue libre droite dans le sens du mouvement.
Idéalement, il faudrait même installer la cale de blocage de cette roue durant l'ensemble du test.
*/

#include "Pindef.h" //definitions des pins du robot
#include "sensor/Button.h"
#include "actuator/Motor.h"
#include "actuator/Buzzer.h"

extern BiButton biButton;
extern Buzzer buzzer;
extern BiMotor motors; //les moteurs sont configurés dans le fichier 01_main.cpp
constexpr uint8_t SPEED_FORWARD = 100; //constante de vitesse pour le TestForward

class TestForward : public Task { //pour faire avancer le robot pendant 10 secondes.
 protected :
 uint8_t _count;
   //... données internes de cette tâche ...
 public :
   SETNAME("TestForward") //nom affiché dans le bilan de lancement de RMonitor
   TestForward() : Task(100), _count(0) {} //on fixe ici la rythmique 
       //-> l'activité va se déclencher toutes les PERIODE ms
   void run() override {
    if(_count!=0){
      _count--;
      if(_count==0) stop();
    }
   }
   void start(uint8_t duree = 20) { //durre en seconde, 20 secondes maximum
    if(_count==0){ //on lance le test
      buzzer.notif(500,4,600);
      _count=duree<20 ? duree*10 : 200;
      motors.setSmoth(1,100);//on va augmenter la vitesse très lentenemnt => 10 unité par seconde
      motors.move(FORWARD,SPEED_FORWARD);
      //motors.move(BACKWARD,SPEED_FORWARD);
    } else stop(); //on stoppe le test
   }
   void stop(){
      _count=0;
      motors.resetSmoth();
      motors.stop(); //fin de test
      buzzer.buzz(500, 2);    
   }
};
TestForward testForward; //activation de la tâche (remplacer aussi `_name_`)

SET_ACTION(setActionTestForward, biButton, ALL_BT, []{ //action sur l'activation simultanée des deux boutons
  //buzzer.bip(500, 2, 500);
  testForward.start(6); //6 secondes en ligne droite
});