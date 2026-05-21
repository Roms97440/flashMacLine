//#define APP_LOG_LEVEL 3
#include "Scheduler.h"     //chargement du moteur d'exécutiond de la librairie
#include "task/RMonitor.h"
#include "Logger.h"




//(A) ... Vous définirez ici vos données globales  ...

//(B) ... vous pourrez définir ici vos unités d'initialisation ...

//(C) ... Vous définirez le code de vos tâches ici  ...
//-> sous forme de fonctions, ou mieux : sous forme de classes

// void start(){ //fonction portant le code à exécuter lors de l'initialisation
//     //... code d'initialisation ...
    
// }
// NeedInit initStart(start); //création de l'objet NeedInit pour cette fonction


// void disBonjour(){
//   Serial.println("Bonjour4");
// }
// TaskCB task1(disBonjour,1000);

// //Setup et loop ne va jamais bouger 
// void setup() {
//     //(D) ... vous écrirez ici les traitements d'initialisation que vous n'avez
//     //     pas écrit dans les méthodes init(...) des classes définies en (B) ...
//     Runner.init();  //On initialise le moteur d'exécution du scheduler
// }

// void loop() {
//     //(E) ... vous écrirez ici les traitements réguliers que vous n'avez pas
//     //    encore réussi en transcrire sous forme de tâches dans la zone (B) ...    
//     Runner.execute(); //On demande au moteur d'exécution de traiter les tâches
// }

class Bonjour : public Task { //la classe spécifique de cette tâche
  protected :
    //... données internes de cette tâche ...
  public : 
    SETNAME("BonjourT")
    Bonjour() : Task(1000){ //on fixe ici la rythmique 
                //-> l'activité va se déclencher toutes les PERIODE millisecondes
        //... valeurs de départ des données internes ...
    }
    void init() override {
      //Log.lock();//désactive les logs
        //... traitement d'initialisation (sera exécuté dans la foction setup(...)) ...
        // Serial.begin(115200);
    }
    void run() override {
        //... traitement de la tâche ... -> sera exécuté dans la fonction loop(...)
        // MAIS seulement toutes les PERIODE millisecondes
        // Serial.println("Bonjour5");
        //Log.ln("Bonjour6");
        LOG_DEBUG("Bonjour8","opopop");
    }
};
Bonjour bjr; //activation de la tâche

// class Toto : public Task { //la classe spécifique de cette tâche
//   protected :
//     //... données internes de cette tâche ...
//   public :
//     Toto() : Task(3000){ //on fixe ici la rythmique 
//                 //-> l'activité va se déclencher toutes les PERIODE millisecondes
//         //... valeurs de départ des données internes ...
//     }
//     void init() override {
//         //... traitement d'initialisation (sera exécuté dans la foction setup(...)) ...
//         Serial.begin(115200);
//     }
//     void run() override {
//         //... traitement de la tâche ... -> sera exécuté dans la fonction loop(...)
//         // MAIS seulement toutes les PERIODE millisecondes
//         Serial.println("TOTO");
//     }
// };
// //Toto toto; //activation de la tâche

RMonitor moniteur;//Tache de suivi des ressources RAM/CPU