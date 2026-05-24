/* Cette classe permet d'interfacer le capteur de courant/tension d'Adafruit
    (site web de ce composant : https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout)

## Dépendance :
-> il faut ajouter le chargement de la libraire `Adafruit INA219`, donc dans le fichier plateformio.ini
```
    lib_deps =
        adafruit/Adafruit INA219 @ ^1.2.3           ;pour le capteur de courant/tension
```

## Usage :
Cette classe est une tache (implémente Task), il suffit donc d'instancier un objet (unique) de cette classe
pour activer est exploiter le capteur de tension.
Le constructeur attend 2 valeurs float : tension_max et tension_min.
Ces valeurs doivent correspondre au seuil de tension :
 - pour tension_max : tension à 100% de batterie.
 - pour tension_min : tension à moins de 10% de batterie.

Typiquement, pour 2 accus 18650 monté en série les valeurs sont :
 tension_max = 8.4
 tension_min = 6.4
Ceux sont ces valeurs qui seront utilisées par défaut.
*/
#pragma once

#include "Runner.h"
#include "Logger.h"
#include <Wire.h>
#include <Adafruit_INA219.h>

/* #region Constantes pour le calcul du % de charge et le lissage des valeurs moyennes */
constexpr float COEF_ALPHA_TENSION = 0.05; //coefficient de lissage pour le calcul de la tension moyenne
constexpr float COEF_ALPHA_COURANT = 0.25; //coefficient de lissage pour le calcul du courant moyenne
/* #endregion */

class SensorINA219 : public Task {
/* #region (attributs internes) */  
  protected :
    Adafruit_INA219 _ina219;
    float _tensionMax; //tension à 100% de batterie.
    float _tensionMin; //tension si <10% de batterie.

    bool _logEnabled;
    bool _initialised;
    float _tension; //tension aux bornes des accus (issue du lissage des valeurs de tension)
    float _courant; //courant circulant dans le circuit
    float _conso;   //énergie consommée (en mW)
    float _batPourcent;     //% de batterie restante 
    uint8_t _cycleCounter; // Compteur pour le sous-échantillonnage (calcul de la tension et log toutes les 2 secondes)
/* #endregion */
public : // API
    SETNAME("Sensor INA219")
    SensorINA219(float tensionMax=8.4, float tensionMin=6.4, bool enable_log=true) : Task(500), _tensionMax(tensionMax), _tensionMin(tensionMin), _logEnabled(enable_log), _initialised(false) ,_tension(0), _courant(0), _conso(0), _batPourcent(0){}
 
    void enableLog(bool flag){ _logEnabled=flag; }
    bool isEnableLog() { return _logEnabled; } 

    //Consultation des valeurs
    float tension(){ return _tension; }
    float courant(){ return _courant; }
    float conso(){ return _conso; }
    float batPourcent(){ return _batPourcent; }

/* #region (implémentation interne) */
    void init() override {
        // Démarrage du capteur
        if (!_ina219.begin()) {
            LOG_WARN("SensorINA219 | Impossible de trouver le composant INA219. Vérifiez le câblage !");
            _initialised=false;
        } else {
            LOG_INFO("SensorINA219 | INA219 détecté avec succès.");
            _initialised=true;
            _courant = _ina219.getCurrent_mA(); // Initialisation du courant
            // Initialisation de la tension de référence
            float shuntInit_mV = _ina219.getShuntVoltage_mV();
            float busInit_V = _ina219.getBusVoltage_V();
            _tension = busInit_V + (shuntInit_mV / 1000.0);
        }
    }
    void run() override {
        if(_initialised){
            //1 - Mise à jour du courant moyen
            float courant_instant = _ina219.getCurrent_mA();
            // Lissage du courant avec un coefficient plus réactif (0.25)
            _courant = (COEF_ALPHA_COURANT * courant_instant) + ((1.0 - COEF_ALPHA_COURANT) * _courant);

            //gestion des calculs toutes les 2 secondes
            _cycleCounter++; 
            if (_cycleCounter >= 4) {
                _cycleCounter = 0; // Reset du compteur
                //2 - mise à jour de la tension moyenne
                //Lecture de la tension aux bornes de la résistance Shunt (en mV)
                float tensionShunt_mV = _ina219.getShuntVoltage_mV();
                // Lecture brute instantanée de la tension 
                float tension_instant = _ina219.getBusVoltage_V() + (tensionShunt_mV / 1000.0);
                _tension = (COEF_ALPHA_TENSION * tension_instant) + ((1.0 - COEF_ALPHA_TENSION) * _tension);

                //3 - Mise à jour du % de batterie restant
                _batPourcent = ((_tension - _tensionMin) / (_tensionMax - _tensionMin)) * 100.0;
                    //bridage des valeurs entre 0% et 100% (pour éviter d'afficher 105% ou -5%)
                if (_batPourcent > 100.0) _batPourcent = 100.0;
                else if (_batPourcent < 0.0) _batPourcent = 0.0;

                //4 - Calcul de la puissance consommée (en mW)
                _conso = _tension * _courant;

                if(_logEnabled) { // Affichage des données sur le moniteur série
                    Log.ln(F("\n======== SensorINA219 ========"));
                    Log.ln(F("Tension       : "), _tension, F(" V"));   //Tension réelle (moyenne)
                    Log.ln(F("Courant       : "), _courant, F(" mA"));  //Courant injecté
                    Log.ln(F("Consommation  : "), _conso, F(" mW"));    //Puissance consommée
                    Log.ln(F("Niv. batterie : "), _batPourcent, F(" %"));//niveau de batterie restant
                    Log.ln(F("====== ================ ======\n"));
                }
            }
        }
    }
/* #endregion */
};

/* Pilotage d'une led d'alerte batterie 
    - led éteinte : niveau >= 40%
    - led glinotante : 40% > niveau >= 20%
    - led allumée fixe : niveau < 20%
*/
class LedBat : public Task {
protected:
/* #region (attributs internes) */
    uint8_t _pin;
    uint8_t _count; //la vérification de la batterie sera faite toutes les 5 secondes
    SensorINA219* _ina219;
    bool _blink;
    bool _isOn;
/* #endregion */
public: // API
    SETNAME("Buzzer")
    LedBat(uint8_t pin, SensorINA219& ina219) : Task(1000), _pin(pin), _ina219(&ina219), _count(0), _blink(false),_isOn(false){}
/* #region (implémentation interne) */
    void init() override {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
    }
    void run() override {
        if(_blink){ //gestion du clignotement toutes les secondes
            _isOn=!_isOn;
            digitalWrite(_pin, _isOn ? HIGH : LOW);
        }
        _count--;
        if(_count<=0){ //mise à jour de l'état tutes les 5 secondes
            _count=5;
            _blink=false;
            float bp = _ina219->batPourcent();
            if(bp>=40) {
                if(_isOn){
                    _isOn=false;
                    digitalWrite(_pin, LOW);                    
                }
            } else if(bp>=20) {
                _blink=true;
            } else {
                if(!_isOn){
                    _isOn=true;
                    digitalWrite(_pin, HIGH);                    
                }                
            }
        }
    }
/* #endregion */
};