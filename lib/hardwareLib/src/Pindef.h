#pragma once
#include <Arduino.h>

/* #region  Constantes principales du robo Flash-McLine */
    //NB: pour définir des constantes, la syntaxe :
    //  constexpr type nom_de_la_constante = valeur;
    //est bien meilleure que :
    //  #define  nom_de_la_constante  valeur
constexpr uint8_t PIN_LED_ROUGE = 8; //D8
constexpr uint8_t PIN_LED_JAUNE = 9; //D9
//PIN_LED_VERT : connectée directement au Vcc (elle est allumée quand l'Arduino est alimenté)
constexpr uint8_t PIN_BT_BLEU = 3;   //D3
constexpr uint8_t PIN_BT_GRIS = 2;   //D2
constexpr uint8_t PIN_BUZZER = 12;   //D12


// Définition des broches du shield DRI0001
constexpr uint8_t PIN_M1_DIR = 4;
constexpr uint8_t PIN_M1_PWM = 5;

constexpr uint8_t PIN_M2_PWM = 6;
constexpr uint8_t PIN_M2_DIR = 7;
/* #endregion */