#define NO_MAIN_FILE
#include "Scheduler.h"     //chargement du moteur d'exécutiond de la librairie
#include "task/RMonitor.h"
#include "Logger.h"


class Toto : public Task { //la classe spécifique de cette tâche
  protected :
    //... données internes de cette tâche ...
  public :
    Toto() : Task(3000){ //on fixe ici la rythmique 
                //-> l'activité va se déclencher toutes les PERIODE millisecondes
        //... valeurs de départ des données internes ...
    }
    void init() override {
        //... traitement d'initialisation (sera exécuté dans la foction setup(...)) ...
        Serial.begin(115200);
    }
    void run() override {
        //... traitement de la tâche ... -> sera exécuté dans la fonction loop(...)
        // MAIS seulement toutes les PERIODE millisecondes
        Serial.println("TOTO");
    }
};
Toto toto; //activation de la tâche