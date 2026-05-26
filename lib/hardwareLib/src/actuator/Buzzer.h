#pragma once

#include "Runner.h"

class Buzzer : public Task {
protected:
/* #region (attributs internes) */
    uint8_t _pin;
    uint8_t _repeat; //compteur de répétition
    uint16_t _frequency;
    uint16_t _duration;
/* #endregion */
/* #region (implémentation interne) */
public:
    void init() override {
        pinMode(_pin, OUTPUT);
    }
    void run() override {
        if(_repeat==0) setEnabled(false); //arrêt de la répétition 
        else {
            tone(_pin, _frequency, _duration);
            _repeat--;
        }
    }
/* #endregion */
public: // API
    SETNAME("Buzzer")
    Buzzer(uint8_t pin) : Task(500, false), _pin(pin), _repeat(0), _frequency(0), _duration(0){}

    //Les sons
    void play(uint16_t frequency, uint16_t duration = 500){ //configurable
        tone(_pin, frequency, duration);
    }

    void bip(uint16_t duree=500, uint8_t repeat=1, uint16_t delay=1000){   //standard
        tone(_pin, 1500, duree);
        if(repeat>1){ //programmation de la répétition
            _frequency = 225;
            _duration = duree;
            _repeat=repeat-1;
            setPeriod(duree+delay);
            setEnabled(true);
        }
    }
    void notif(uint16_t duree=500, uint8_t repeat=1, uint16_t delay=1000){  //aigu
        tone(_pin, 2750, duree);
        if(repeat>1){ //programmation de la répétition
            _frequency = 225;
            _duration = duree;
            _repeat=repeat-1;
            setPeriod(duree+delay);
            setEnabled(true);
        }
    }
    void buzz(uint16_t duree=500, uint8_t repeat=1, uint16_t delay=1000){   //grave
        tone(_pin, 225, duree);
        if(repeat>1){ //programmation de la répétition
            _frequency = 225;
            _duration = duree;
            _repeat=repeat-1;
            setPeriod(duree+delay);
            setEnabled(true);
        }
    }
};