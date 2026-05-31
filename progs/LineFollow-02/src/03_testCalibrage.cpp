//Fichier source secondaire: 
#include "00_config.h"

/*  ===> Tâche de vérification du calibrage

  Protocole de test :
- appuyez sur les 2 boutons en même temps pour activer cette tâche (un tripple bip retentit)
- notez quand dans cette tâche les roues ne tournent jamais.
- placez le robot au sol, proche de la ligne noire, en le tenant face à vous (vous regardez les 2 triangles en bas du capot avant).
- positionnez la ligne noire entre les traingles, ou volontairement désaxé par rapport à eux.
- appuyez sur le bouton BLEU pour demander une mesure, un son de notification long retentit et les led jaune et/ou rouge s'allument pour indiquer la valeur de déviation (voir tableau ci-dessous).
- faites des nouvelles mesures en décalant la ligne noire
- pour effacer la mesure (les led vont s'etteindre toutes les deux) : appuyez sur le bouton GRIS (deux son de notification retentissent)
- pour quitter cette tâche : appuyez en même temps sur les 2 boutons (un buzz retentit).

Vue face à la voiture (l'avant du capot nous fait face) : la ligne noire est XXX (=> XXX):
[-1000; -700[ -> jaune blink                => trop à gauche
[-700; -400[  -> jaune fixe                 => à gauche
[-400; -100[  -> jaune fixe + rouge blink   => légèrement à gauche
[-100; 100]   -> jaune fixe + rouge fixe    => au centre
]100; 400]    -> jaune blink + rouge fixe   => légèrement à droite
]400; 700]    -> rouge fixe                 => à droite
]700; 1000]   -> rouge blink                => trop à droite

NB: pendant l'exécution de cette tâche, la tâche LedBat (avertisseur de batterie) est désactivée pour libérer l'usage de la led rouge.
*/
/* #region(close) les #include */
#include "sensor/Button.h"
#include "actuator/Buzzer.h"
#include "sensor/Ina219.h"
#include "actuator/Led.h"
#include "sensor/Qtr-3RC.h"
/* #endregion */

extern SensorQTR_3RC capteur;
extern BiButton biButton;
extern Buzzer buzzer;
extern LedBat ledBat; //pour la led rouge (définit dans 01_main.cpp)
extern Led ledJaune;  //pour la led jaune (définit dans 02_calibragtion.cpp)

class TestCalibrage : public Task { //pour faire avancer le robot pendant 10 secondes.
 protected :
 int16_t _value; //dernière valeur lue (-2000 si aucune)
 public :
   SETNAME("TestCalibrage") //nom affiché dans le bilan de lancement de RMonitor
   TestCalibrage() : Task(500, false), _value(-2000) {} 
   void run() override {
      if(_value==-2000){ //aucune valeur lue
        ledBat.setOn(false);  
        ledJaune.setOn(false);  
      } else if(_value<-700){ //[-1000; -700[ -> jaune blink                => trop à gauche
        ledBat.setOn(false);
        ledJaune.swap();
      } else if(_value<-400){ //[-700; -400[  -> jaune fixe                 => à gauche
        ledBat.setOn(false);
        ledJaune.setOn(true);
      } else if(_value<-100){ //[-400; -100[  -> jaune fixe + rouge blink   => légèrement à gauche
        ledBat.swap();
        ledJaune.setOn(true);
      } else if(_value<=100){ //[-100; 100]   -> jaune fixe + rouge fixe    => au centre
        ledBat.setOn(true);
        ledJaune.setOn(true);
      } else if(_value<400){ //]100; 400]    -> jaune blink + rouge fixe   => légèrement à droite
        ledBat.setOn(true);
        ledJaune.swap();
      } else if(_value<700){ //]400; 700]    -> rouge fixe                 => à droite
        ledBat.setOn(true);  
        ledJaune.setOn(false);  
      } else {               //]700; 1000]   -> rouge blink                => trop à droite 
        ledBat.swap();  
        ledJaune.setOn(false);  
      }
   }
   void scan(){
      _value = capteur.deviation();
      buzzer.notif(1500);
   }
   void clean(){
      _value=-2000;
      buzzer.notif(400,2,300);
   }
   void start() { 
      ledBat.setEnabled(false); //pour obtenir l'usage esclusive de la led rouge
      _value=-2000;
      buzzer.bip(400,3,300);
      setEnabled(true);
   }
   void stop(){
      setEnabled(false);
      ledBat.setOn(false);  
      ledJaune.setOn(false); 
      ledBat.setEnabled(true); //pour libérer la tâche de surveillance de la batterie
      buzzer.buzz();
   }
};
TestCalibrage testCalibrage; //activation de la tâche (remplacer aussi `_name_`)

SET_ACTION(setActionTaskTestCaligrage, biButton, ALL_BT, [](bool launch, uint8_t bt){ //action sur l'activation du bouton bleu
    if(launch) testCalibrage.start();
    else switch(bt){
      case BiButton::BT1 : //bouton Gris -> clean
        testCalibrage.clean();
        break;
      case BiButton::BT2 : //bouton Bleu -> nouvelle mesure
        testCalibrage.scan();
        break;
      default : //2 bouton -> fin de tâche
        testCalibrage.stop();
        return false;
    }
    return true;
});
