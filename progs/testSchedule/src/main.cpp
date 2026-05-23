//#define APP_LOG_LEVEL 3
#include "Scheduler.h"     //chargement du moteur d'exécutiond de la librairie
#include "task/RMonitor.h"
#include "Logger.h"

RMonitor moniteur;//Tache de suivi des ressources RAM/CPU

class Bonjour : public Task { //la classe spécifique à une tâche
  protected :
    //... données internes de cette tâche ...
  public : 
    SETNAME("Bonjour")    //Nom afficher dans le bilan de lancement de RMonitor
    Bonjour() : Task(1000){ //on fixe ici la rythmique 
                //-> l'activité va se déclencher toutes les PERIODE millisecondes
        //... traitement lors de l'instanciation de l'obet (généralement vide) ...
    }
    void init() override {
        //... traitement d'initialisation de la tâche (sera exécuté dans la foction setup(...)) ...
        //Serial.begin(115200);
    }
    void run() override {
        //... traitement de la tâche ... -> sera exécuté dans la fonction loop(...)
        // MAIS seulement toutes les PERIODE millisecondes
        // Serial.println("Bonjour5");
        //Log.ln("Bonjour6");
        LOG_DEBUG("Bonjour8","opopop"); //exemple d'utilisation d'une macro de log ("Logger.h")
    }
};
Bonjour bjr; //activation de la tâche 
//--> créer l'objet suffit pour créer la tâche
//   --> elle s'enregistrement automatiquement auprès du scheduleur

//Une autre tâche (placé dans un second fichier -> Toto.cpp)
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