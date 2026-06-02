#pragma once
#ifdef _MAIN_FILE
    #include "Scheduler.h"
#else
    #include "Runner.h"
#endif

/* Permet de fixer la configuration générale
==> juste après #include "Scheduler.h". ou #include "Runner.h" , faire :
#include "00_config.h"
*/
/* #region(close) constantes de pré-réglage (inutile de commenter/décommenter) */
    //pour le choix du mode boost
#define _MONO   0
#define _CUMUL  1
/* #endregion */

#include "Pindef.h"         //definitions des pins du robot

/* - Décommenter ci-dessous pour désactiver tous les logs (et RMonitor) */
//#define SET_LOG_LEVEL LOG_LEVEL_NONE

/* - Décommenter ci-dessous pour activer le mode boost (choisir entre _MONO et _MULTI )*/
//#define ENABLE_BOOST  _MONO  
//#define ENABLE_BOOST  _MULTI  

/* - Décommenter ci-dessous pour activer l'objet Gardian (voir `06_gardian.cpp`) */
#define ENABLE_GARDIAN  

/* - Décommenter ci-dessous pour activer l'objet Sniffer (voir `07_sniffer.cpp`) */
#define ENABLE_SNIFFER
