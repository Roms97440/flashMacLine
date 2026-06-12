#pragma once
#ifdef _MAIN_FILE
    #include "Scheduler.h"
#else
    #include "Runner.h"
#endif

/* Permet de fixer la configuration générale
==> au tout début du code source, juste après #include "Scheduler.h". ou #include "Runner.h" , faire :
#include "00_config.h"
*/
/* #region(close) constantes de pré-réglage (inutile de commenter/décommenter) */
    //pour le choix du mode boost
#define _MONO   0
#define _CUMUL  1
    //pour le choix du type des moteurs
#define _BASIC  2
#define _SMOTH  3
#define _REACTIF 4
/* #endregion */

#include "Pindef.h"         //definitions des pins du robot

/* - Décommenter ci-dessous pour désactiver tous les logs (et RMonitor) */
//#define SET_LOG_LEVEL LOG_LEVEL_NONE

/* - Décommenter ci-dessous pour activer le mode boost (choisir entre _MONO et _MULTI ) */
//NB: si la tâche Gardian est activiée (voir ci-dessous) il est préférable de désactiver ce mode boost
// car il est basée sur un seuil d'intensité de courant, alors que le Gardian dispose d'un mode de boost
// basé sur une détection d'immobilisation du robot. 
//#define ENABLE_BOOST  _MONO  
//#define ENABLE_BOOST  _MULTI  

/* - Choix du type de moteur à utiliser  (choisir entre _BASIC , _SMOTH et _REACTIF) */
//NB: si les 2 sont commentés, ce sera des moteurs SMOTH qui seront utilisé
#define USE_MOTOR _BASIC
//#define USE_MOTOR _SMOTH 
//#define USE_MOTOR _REACTIF

/* - Décommenter ci-dessous pour activer la tâche Gardian (voir `06_gardian.cpp`) */
//#define ENABLE_GARDIAN  

/* - Décommenter ci-dessous pour activer la tâche Sniffer (voir `07_sniffer.cpp`) */
//#define ENABLE_SNIFFER
