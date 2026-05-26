---
title: (FMcL) Base code
---
## Modèle de base de code

<!-- #region(collapsed) (à replier) -->
<details><summary>(si vous consultez ce fichier dans **VSCode**)</summary>

Pour améliorer le confort de lecture :
- clic-droit sur le fichier dans le volet d'exploration (l'explorateur à gauche)
- choisir "Ouvrir l'aperçu" (*Open preview*)
- glisser ensuite la fenêtre d'aperçu sur le bord droit pour la laisser ouverte en parallèle du code

Notez que cette [documentation est consultable en ligne sur cette page](https://doc.payet.top/flashmcline/_plan_.md)
</details>
<!-- #endregion -->

---

### 1 - A la création d'un nouveau programme (projet PlatformIO) :

 - penser à supprimer les dossiers : **include**, **lib** et **test**.

 - contenu du fichier de configuration `platformio.ini` :
 ``` ini
 [env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 115200
build_flags = -std=gnu++17
build_unflags = -std=gnu++11
lib_deps =
   symlink://../../lib/hardwareLib
;   https://gitlab.com/c-arduino/scheduler.git 
    ; -> déjà inclus dans lib/hardwareLib
 ```

 - include du fichier source principale `main.cpp` :
``` cpp
#include "Scheduler.h"
#include "task/RMonitor.h"  //inclus déjà "Logger.h"

//éléments du robot (commenter ceux non nécessaires):
#include "Pindef.h" //definitions des pins du robot
#include "sensor/Button.h"
#include "actuator/Led.h"
#include "actuator/Buzzer.h"
#include "sensor/Ina219.h"
#include "sensor/Qtr-3RC.h"
#include "actuator/Motor.h"

//...
```

 - include des autres fichiers sources `xxxx.cpp` :
 ``` cpp
#define NO_MAIN_FILE  // /!\ pour tous les fichiers secondaires
#include "Scheduler.h"

//...
```

### 2 - Modèle des classes de la librairie Scheduler

- Une tâche : (remplacer les 6 mots signalés par \_xxx\_ )
 ``` cpp
class _NAME_ : public Task { //classe spécifique à une tâche
  protected :
    //... données internes de cette tâche ...
  public :
    SETNAME("_NAME_") //nom affiché dans le bilan de lancement de RMonitor
    _NAME_() : Task(_PERIODE_){} //on fixe ici la rythmique 
        //-> l'activité va se déclencher toutes les PERIODE ms
    void init() override {
        //-> traitement d'initialisation (sera exécuté dans le setup())
    }
    void run() override {
        //-> traitement de la tâche (sera exécuté dans le loop())
        // après chaque délais de PERIODE ms 
    }
};
_NAME_ _name_; //activation de la tâche (remplacer aussi `_name_`)
```

- Une unité d'initialisation : (remplacer les 5 mots signalés par \_xxx\_ )
 ``` cpp
class _NAME_ : public NeedInit { 
  protected :
    //... données internes de cette unité ...
  public :
    SETNAME("_NAME_") //nom affiché dans le bilan de lancement de RMonitor
    _NAME_() : NeedInit(){}
    void init() override {
        //-> traitement d'initialisation (sera exécuté dans le setup())
    }
};
_NAME_ _name_; //activation de l'unité(remplacer aussi `_name_`)
```

<br/>

---
(retour à [la page d'accueil](./_plan_.md))