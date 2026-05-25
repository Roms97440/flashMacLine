/* Cette classe permet d'interfacer le capteur de ligne QTR-3RC de la marque Pololu     
(site web de ce composant : https://www.pololu.com/product/2457)

## Dépendance :
-> il faut ajouter le chargement de la libraire `QTRSensors`, donc dans le fichier plateformio.ini
```
    lib_deps =
        pololu/QTRSensors @ ^4.0.0           ;pour le capteur de ligne
```

## Cablage :
Les broches 1 à 3 du capteur doivent être connecté au pin A1, A2 et A3 de l'Arduino.

## Usage :
Cette classe est une tache (implémente Task), il suffit donc d'instancier un objet (unique) de cette classe
pour activer et exploiter le capteur de ligne.
Voici un résumé de son API :

    - le constructeur ne prend pas de paramètre

    - les méthodes de consultation :
        - bool ready() : retourne true si la calibration a été réalisé (le capteur est prêt)
        - StateQTR state() : retourne l'état du capteur 
            -> SensorQTR_3RC::NEED_CALIBRATE, SensorQTR_3RC::CALIBRATION, ou SensorQTR_3RC::READY
        - int16_t deviation() : retourne la valeur de déviation courante.
            C'est un nombre entre -1000 et 1000 :
                -> 0 : alignement parfait
                -> négatif (<0) : le robot est trop à droite, la ligne est à gauche.
                -> positif (>0) : le robot est trop à gauche, la ligne est à droite
        -  uint16_t values(int n) : permet d'obtenir la valeur d'une des photoresistances
            -> n : le numéro de la photoresistances (0, 1 ou 2)
            -> la valeur retourné est un nombre de 0 à 1000 traduisant l'intensité de noir captée
                NB: une valeur <200 est considérée comme du blanc.
        - bool lostLine() : //retourne true si le capteur n'est plus sur la ligne (la ligne est perdue)
    
    - les méthodes d'action :
        - void calibrate() : lance un cycle de calibration. Ce cyle dure 10 secondes, et pendant
            cette durée il est nécessaire que la ligne passe plusieurs fois sous le capteur pour
            que chaque photoresistance perçoit du blanc et du noir à plusieurs reprises.
*/
#pragma once

#include "Runner.h"
#include "Logger.h"
#include <QTRSensors.h>

// Définition des broches numériques
constexpr uint8_t QTR_SENSOR_COUNT = 3; //nombre de photoresistance sur ce capteur
constexpr uint8_t PIN_QTR_GAUCHE = A1;
constexpr uint8_t PIN_QTR_CENTRE = A2;
constexpr uint8_t PIN_QTR_DROITE = A3;

class SensorQTR_3RC : public Task {
public :
    SETNAME("Sensor QTR_3RC")
    enum StateQTR { NEED_CALIBRATE, CALIBRATION, READY };
/* #region (attributs internes) */  
  protected :
    QTRSensors _qtr;
    StateQTR _state;    //true si le capteur est ready
    int16_t _deviation; //valeur [-1000; 1000] d'éloignement par raaport au centre de la ligne 
    uint16_t _values[QTR_SENSOR_COUNT]; //pour la valeur individuel de chacunes des 3 photoresistances (0 à 1000)
    int16_t _countCalibration;
/* #endregion */

public : // API
    SensorQTR_3RC() : Task(20), _state(NEED_CALIBRATE), _deviation(0), _countCalibration(0){}
    bool ready() { return _state==READY; }
    StateQTR state() { return _state; }
    int16_t deviation() { return _deviation; }
    uint16_t values(int n){ return n>=0 && n<QTR_SENSOR_COUNT ? _values[n] : 0; }
    bool lostLine() { //retourne true si le capteur n'est plus sur la ligne
        // <=> les 3 capteurs renvoient moins de 200 (seuil de blanc calibré)
        return (_values[0] < 200 && _values[1] < 200 && _values[2] < 200);
    }

    void calibrate(int16_t count=500){ //lance un cyle de calibration
        LOG_INFO("SensorQTR_3RC | début de calibration...");
        _qtr.resetCalibration();
        _state=CALIBRATION;
        _countCalibration=count;
    }

/* #region (implémentation interne) */
    void init() override {
        _qtr.setTypeRC(); //configuration du capteur (Type RC)
        // Assignation des broches (dans l'ordre physique : gauche à droite)
        _qtr.setSensorPins((const uint8_t[]){PIN_QTR_GAUCHE, PIN_QTR_CENTRE, PIN_QTR_DROITE}, QTR_SENSOR_COUNT);
    }
    void run() override {
        if(_countCalibration>0){ //une calibration est en court
            _countCalibration--;
            _qtr.calibrate();
            if(_countCalibration<=0){ //fin de calibration
               _state=READY;
               LOG_INFO("SensorQTR_3RC | ... fin de calibration.");
            }
        } else {
            //Lecture de la position de la ligne noire
            //-> cette fonction remplit aussi le tableau valeursCapteurs[] avec les valeurs individuelles (0 à 1000)
            uint16_t positionLigne = _qtr.readLineBlack(_values);
            _deviation = positionLigne - 1000;
        }
    }
/* #endregion */
};