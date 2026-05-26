#pragma once
#include <Arduino.h>

// Constantes principales du robot Flash-McLine
    //NB: pour définir des constantes, la syntaxe :
    //  constexpr type nom_de_la_constante = valeur;
    //est bien meilleure que :
    //  #define  nom_de_la_constante  valeur

/* #region Les PIN */
//Les leds :
constexpr uint8_t PIN_LED_ROUGE = 8; //D8
constexpr uint8_t PIN_LED_JAUNE = 9; //D9
//PIN_LED_VERT : connectée directement au Vcc 
//  -> (elle est allumée quand l'Arduino est alimenté)

//Les boutons :
constexpr uint8_t PIN_BT_BLEU = 3;   //D3
constexpr uint8_t PIN_BT_GRIS = 2;   //D2

//Le buzzer :
constexpr uint8_t PIN_BUZZER = 12;   //D12

// Définition des broches du shield DRI0001
/*
NB:
pilotage des roues (Broches D4 à D7)
La direction (Broches D4 et D7) : Un simple état HIGH (avant) ou LOW (arrière).
La vitesse (Broches D5 et D6) : Un signal PWM de 0 (arrêt) à 255 (vitesse maximale).

Voici l'attribution matérielle figée sur le circuit imprimé du shield :
-Moteur 1 (généralement Gauche) : Direction = D4 | Vitesse = D5
-Moteur 2 (généralement Droit) : Direction = D7 | Vitesse = D6
*/
constexpr uint8_t PIN_M1_DIR = 4;
constexpr uint8_t PIN_M1_PWM = 5;
constexpr uint8_t PIN_M2_PWM = 6;
constexpr uint8_t PIN_M2_DIR = 7;
/* #endregion */

/* #region Constantes générales */
constexpr bool RIGHT = true;
constexpr bool LEFT = false;
constexpr bool FORWARD = true;
constexpr bool BACKWARD = false;
/* #endregion */

/* #region Les fonctions utiles */

inline uint8_t safe_float_to_uint8(float f) { //convertion (+arrondi) float -> uint8
    // 1. On arrondit à l'entier le plus proche (utilise le round standard de math.h inclus dans Arduino)
    float rounded = round(f);
    
    // 2. On limite la valeur entre 0 et 255 avec constrain() (propre à Arduino)
    float clamped = constrain(rounded, 0.0f, 255.0f);
    
    // 3. Conversion sécurisée
    return static_cast<uint8_t>(clamped);
}
/* #endregion */