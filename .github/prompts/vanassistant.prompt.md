---
mode: agent
name: vanassistant
description: Contexte complet pour travailler sur le projet VanAssistant (application C++ multiplateforme construite sur DrzEngine)
---

# VanAssistant — Prompt de contexte projet

## 1. But du projet

VanAssistant est une application embarquée d'assistance pour un van aménagé.
Elle affiche plusieurs pages (menu, road/dashboard, assistant animé, park) sur
un petit écran (résolution native 320×240), reçoit des données via port série
(dashboard du van, IMU, capteurs Victron) et peut vocaliser du texte via SAM
(TTS). Cible principale : un Raspberry Pi Zero (framebuffer direct) ou
Buildroot minimal. Cibles secondaires : desktop Linux (X11), Windows, WebAssembly.

## 2. Architecture générale

```
┌─────────────────────────────────────────────────────────────────┐
│                        VanAssistantApp                          │
│  (IDrzEngineApp + ISerialProtocolReceiver)                      │
│                                                                 │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌───────────┐  │
│  │  PageMenu   │ │  PageRoad   │ │PageAssistant│ │ PagePark  │  │
│  └─────────────┘ └─────────────┘ └─────────────┘ └───────────┘  │
│         │              │               │              │         │
│         └──── DisplayPageManager (routing / lifecycle) ─────────┘
└─────────────────────────────────────────────────────────────────┘
         │                          │                    │
         ▼                          ▼                    ▼
   IDrzGraphics              IDrzInputs             IDrzSerial
         │                          │                    │
         ▼                          ▼                    ▼
      DrzEngine (bibliothèque externe : /home/drzoid/dev/drzengine)
         │
         ├── Backend PGE   (olcPixelGameEngine — X11 / WASM / Win)
         └── Backend PGEFB (framebuffer direct — Pi / Buildroot)
```

- **DrzEngine** est un projet frère (chemin absolu codé en dur dans le CMake :
  `DRZENGINE_LIB_DIR = /home/drzoid/dev/drzengine`). Il fournit les interfaces
  `IDrzEngine`, `IDrzGraphics`, `IDrzInputs`, `IDrzSam`, `IDrzSerial`, la
  gestion des pages (`DisplayPageManager`, `DisplayPage`, `Widget`) et deux
  backends de rendu (`drzenginepge`, `drzenginepgefb`).
- **VanAssistant** consomme ces interfaces via des façades statiques :
  `DrzEngine::Get()`, `DrzGraphics::Get()`, `DrzInputs::Get()`, `DrzSerial::Get()`.

## 3. Mapping PLATFORM → BACKEND (voir CMakeLists.txt racine)

| PLATFORM     | BACKEND | Notes                                                                 |
| ------------ | ------- | --------------------------------------------------------------------- |
| `LINUX_X11`  | PGE     | Par défaut. OpenGL + X11 + PNG + Threads.                             |
| `PI_X11`     | PGE     | Cross‑compile pour Pi Zero avec X11 (toolchain requis).               |
| `WASM`       | PGE     | emscripten. Génère `vanassistant.html`.                               |
| `WIN`        | PGE     | mingw64. GDI+ / Shlwapi / Dwmapi, linkage statique libgcc/libstdc++.  |
| `LINUX_FB`   | PGEFB   | Framebuffer Linux (rare, dev direct).                                 |
| `PI_FB`      | PGEFB   | Cross‑compile Pi Zero framebuffer, cible principale historique.       |
| `BUILDROOT`  | PGEFB   | Cross‑compile via toolchain Buildroot, cible de production sur Pi.    |

Le nom du binaire de sortie est `vanassistant.<platform lowercase>` (ex :
`vanassistant.linux_x11`, `vanassistant.pi_fb`, `vanassistant.buildroot`).
Sur WASM la sortie est `vanassistant.html`.

## 4. Arborescence essentielle

```
/home/drzoid/dev/vanassistant/
├── CMakeLists.txt              # Configuration unique multi-plateforme
├── README.md                   # Instructions de build par plateforme
├── src/
│   ├── main.cpp                # Point d'entrée (setup engine + app + serial CLI)
│   └── vanassistant/
│       ├── VanAssistantApp.{h,cpp}   # IDrzEngineApp — crée les pages, route série
│       ├── SerialProtocol.h          # Protocole binaire J7 + Victron
│       ├── PageMenu.{h,cpp}          # Menu principal (4 items)
│       ├── PageRoad.{h,cpp}          # Dashboard route (gauges, compass, speed…)
│       ├── PageAssistant.{h,cpp}     # Scout 3D animé + TTS
│       ├── PagePark.{h,cpp}          # Page park (placeholder)
│       ├── fonts/                    # Fonts Adafruit GFX convertis en headers
│       ├── ressources/               # Ressources embarquées (asmtext.h…)
│       └── pages/
│           ├── common/widgets/       # Widgets partagés (Starfield, MenuItem, Speed…)
│           ├── assistant/            # Scout 3D (models/, widgets/, AsmText.h)
│           ├── menu/                 # J7 3D (models/, widgets/)
│           └── road/
│               ├── sprites/          # Sprites raw en headers (jauges, compas…)
│               └── widgets/          # Gauge, Compass, Distance, Speedometer…
├── include/
│   ├── miniaudio.h                   # Audio single-header
│   ├── olcPixelGameEngine.h          # PGE (dupliqué / historique)
│   ├── olcPGEX_QuickGUI.h
│   └── sam/                          # Headers SAM
├── lib/sam/src/                      # Sources SAM (TTS)
├── samlibs/                          # SAM upstream (tests locaux)
├── models/                           # Modèles OBJ (scout, J7…)
├── gfx/                              # Sources graphiques (aseprite, blend, png)
├── embedded/                         # Firmwares compagnons (PlatformIO)
│   ├── atmega2560-va-serialhub/      # Hub série ATmega2560
│   ├── esp32-s3-va-imu-sender/       # ESP32-S3 émetteur IMU
│   └── esp32-s3-va-imu-calibration/  # ESP32-S3 calibration IMU
├── tools/                            # Utilitaires dev (CMake séparé)
│   ├── png-to-header/                # PNG → header C
│   ├── font-to-header/               # TTF → header C (Adafruit GFX)
│   ├── object-to-header/             # OBJ → header C
│   ├── obj-viewer/                   # Visualiseur OBJ (X11)
│   ├── serial-gui/                   # GUI diagnostic série
│   └── project-generator/            # Générateur de projets DrzEngine
├── test_projects/                    # Projets de test isolés (PGE, gfx3d, widgets…)
├── wasm/                             # Shell HTML + serveur Python pour WASM
├── build.buildroot/  build.pifb/     # Répertoires de build (non versionnés)
└── .github/prompts/                  # Prompts pour agents IA
```

## 5. Cycle de vie d'une page (DisplayPage)

Chaque page hérite de `DisplayPage` (défini dans DrzEngine) et implémente :

```cpp
class DisplayPage {
public:
  virtual void Load() = 0;                  // Chargement initial (une fois)
  virtual void Activate(DisplayPage* last); // À l'entrée sur la page
  virtual void ReadInputs() = 0;            // Lecture inputs à chaque frame
  virtual void Update(float dt) = 0;        // Mise à jour logique
  virtual void Render() = 0;                // Rendu graphique
};
```

Routage : `DisplayPageManager::GoToPage("menu")` ou pointeur. `Back()` revient
à la précédente. Les pages sont ajoutées dans `VanAssistantApp::Setup()`.

Actuellement la page par défaut à l'ouverture est `roadPage` (voir
`VanAssistantApp::Setup`, ligne 77).

## 6. Protocole série (SerialProtocol.h)

En-tête binaire commun : `J7PacketHeader { uint8_t type; uint32_t len; }`.

Types de paquets (`J7PacketType`) :
- `J7_SAY_TEXT` — TTS (`char text[256]`).
- `J7_DASHBOARD` — jauge carburant, vitesse km/h, batterie ×10 V, RPM ×100,
  température coolant ×10 °C, odomètre km, trip 1/10 km, + booléens
  lampes (préchauffage, non-charge, huile, clignotants, feux, warnings, problème).
- `J7_IMU` — accéléro/gyro/magnéto (9 × float).
- `VICTRON_SMART_SHUNT` — timeToGo, batt V, aux V, batt A, consumedAh, state, alarm.
- `VICTRON_ORION` — chargeur DC-DC (état, tensions in/out, erreurs texte).

Le récepteur est `VanAssistantApp` (implémente `ISerialProtocolReceiver`). Les
handlers sont actuellement `//TODO` : à câbler vers les pages concernées
(dashboard → PageRoad, IMU → animation Scout, TTS → PageAssistant::Say).

Le port série est passé en CLI : `--serial /dev/ttyAMA0 --bauds 115200`.

## 7. Système graphique

- Résolution logique : **320 × 240** (constantes `SCREEN_W` / `SCREEN_H` dans
  `main.cpp`), scale ×2 en desktop.
- Palette : couleurs prédéfinies dans `drz::graphics` (`BLACK`, `WHITE`,
  `RED`, `GREEN`, `CYAN`, `MAGENTA`, `DARK_*`, `VERY_DARK_*`, `BLANK`…).
- Fonts : format Adafruit GFX (`struct font { bitmap, glyph, first, last, yAdvance }`),
  chargées via `gfx->LoadFont("nom", &Xxx7b)`. Fonts disponibles :
  `solidmono4/6/8`, `b612mono6`, `sevenseg30`, et d'autres dans `src/vanassistant/fonts/`.
- Sprites : soit convertis en headers C via `tools/png-to-header`, soit chargés
  depuis `Sprite(w, h, data*)`.
- 3D : modèles OBJ convertis via `tools/object-to-header` → header C. Rendu par
  la classe `Model` de DrzEngine (`gfx3d.h` / `DrzGraphics.h`), pipeline
  triangles avec matrices rotation/translation.

## 8. Inputs

`IDrzInputs::GetKey(Key::K1)` renvoie un `HardwareButton { isPressed, isReleased, isHeld }`.
Clés physiques utilisées sur le Pi : `K1`, `K2`, `K3`, `K4` (4 boutons GPIO).
Sur desktop les mêmes touches numériques du clavier sont mappées.

## 9. Build & run (rappel condensé)

**Vérifier que ça compile partout après une modif** : utiliser la skill
`vanassistant-build-all` (wrapper `.claude/scripts/va_build_all.sh`) qui
détecte les toolchains dispo et build toutes les cibles dans `build.check.<target>/` :

```bash
.claude/scripts/va_build_all.sh quick    # smoke test LINUX_X11 (~1 min)
.claude/scripts/va_build_all.sh list     # affiche cibles + statut toolchain
.claude/scripts/va_build_all.sh all      # build de toutes les cibles + résumé
```

Pour la génération d'image SD embarquée : skill `vanassistant-buildroot`
(wrapper `.claude/scripts/va_buildroot.sh`), voir §13.4.

**Invocations manuelles** (rappel — cf. skills ci-dessus pour l'usage courant) :

```bash
# Desktop Linux / WSL2
mkdir build.x11 && cd build.x11
cmake .. -DPLATFORM=LINUX_X11 && make
./vanassistant.linux_x11 --serial /dev/ttyUSB0 --bauds 115200   # série optionnelle

# WASM
mkdir build.wasm && cd build.wasm
cmake .. -DPLATFORM=WASM -DCMAKE_TOOLCHAIN_FILE=$EMSDK/…/emscripten.cmake && make
python3 ../wasm/wasmserve.py   # puis http://localhost:8000/build.wasm/vanassistant.html

# Pi Zero framebuffer (cross-compile)
mkdir build.pifb && cd build.pifb
cmake .. -DPLATFORM=PI_FB -DCMAKE_TOOLCHAIN_FILE=/home/drzoid/sdk/crosspigcc/toolchain.cmake && make
make deploy                    # scp + relance sur pizerodev

# Buildroot (production)
mkdir build.buildroot && cd build.buildroot
cmake .. -DPLATFORM=BUILDROOT -DCMAKE_TOOLCHAIN_FILE=/home/drzoid/sdk/buildroot/output/host/share/buildroot/toolchainfile.cmake && make
make image                     # génère vanassistant.sdcard.img
```

Types de build : `Debug` (défaut) si `CMAKE_BUILD_TYPE` non défini.
`compile_commands.json` toujours généré (utile pour clangd).

## 10. Prérequis système (dev desktop)

```bash
sudo apt install build-essential libglu1-mesa-dev libpng-dev libx11-dev
```

WASM : emsdk activé (`source ~/sdk/emsdk/emsdk_env.sh` — install via
`git clone https://github.com/emscripten-core/emsdk ~/sdk/emsdk && ./emsdk install latest && ./emsdk activate latest`).
Cross‑compilation Pi : toolchain rsync‑ée + `~/sdk/crosspigcc/toolchain.cmake` prêt.
Windows : `apt install g++-mingw-w64-x86-64` + toolchain fourni dans
`.claude/scripts/toolchains/mingw64.cmake`.
Buildroot : arbre déjà buildé au moins une fois pour disposer du toolchainfile.

## 11. Conventions du code

- **C++17**, headers en `.h`, `#pragma once`.
- Namespace principal utilisé : `drz` (avec `using namespace drz;`).
- Widgets et pages : classes header‑only sauf les 4 pages principales
  (`PageMenu`, `PageRoad`, `PageAssistant`, `PagePark`) qui ont `.h + .cpp`.
- Assets embarqués : convertis en headers C (`extern const font …`, sprites raw).
- Pas de dynamic_cast — architecture connue à la compilation.
- `std::cout` pour le logging (pas de logger dédié).
- Le CMake liste les sources vanassistant via `file(GLOB src/vanassistant/*.cpp)`
  ⚠️ ne cherche PAS récursivement — un nouveau `.cpp` dans un sous‑dossier ne
  sera pas compilé sans mise à jour manuelle du CMake.

## 12. Contraintes / pièges connus

- Chemin absolu vers `drzengine` codé en dur dans `CMakeLists.txt` L13.
  Si drzengine n'existe pas à cet emplacement, le build échoue.
- La cible `BUILDROOT` copie automatiquement le binaire dans
  `/home/drzoid/sdk/buildroot/vanassistant_overlay/opt/vanassistant/` — chemin
  codé en dur (`BUILDROOT_COPY_DEST_DIR`).
- Les cibles `deploy` (Pi X11/FB) supposent un host SSH nommé `pizerodev` avec
  binaire installé dans `/opt/vanassistant`.
- Le résolveur `find_package(PNG)` est nécessaire sur toutes les plateformes
  sauf `LINUX_FB` (allégé).
- Les handlers de paquets série (`ProcessJ7*`, `ProcessVictron*`) sont vides —
  toute nouvelle donnée à afficher passe forcément par un câblage vers la page.
- Les tests dans `test_projects/` sont indépendants (chacun son CMake), ce ne
  sont pas des tests unitaires du projet principal.
- Il n'existe pas de framework de tests unitaires — la validation se fait par
  build multi-plateforme et smoke test visuel.
- Fichiers supprimés en git (`include/fb/*`, `include/gfx3d.h`,
  `include/olcPGEX_Graphics3D.h`) : ces headers sont désormais fournis par
  drzengine, ne pas les recréer.

## 13. Distribution Buildroot embarquée (hors repo)

Le build `PLATFORM=BUILDROOT` ne se suffit pas à lui‑même : il livre juste
un binaire ARM (`vanassistant.buildroot`) qu'une distribution Buildroot
séparée doit intégrer dans une image SD. Cette distribution vit **en dehors
du repo vanassistant** et se répartit sur trois emplacements sur la machine
de dev.

### 13.1 Emplacements

```
~/dev/vanassistant-pizero-buildroot/   # squelette wrapper BR2_EXTERNAL (INACTIF)
  ├── configure                        # setup make BR2_EXTERNAL=…
  ├── buildroot         → ~/sdk/buildroot-2024.02.4/  (symlink OBSOLÈTE)
  ├── vanassistant      → ~/dev/vanassistant/        (symlink)
  ├── vanassistant-tree/               # vide (BR2_EXTERNAL non peuplé)
  ├── vanassistant-overlay/            # vide
  └── build.{buildroot,vanassistant}/  # vides

~/sdk/buildroot-2025.02.4/             # ACTIF (= ~/sdk/buildroot)
  ├── configs/vanassistant_rpi0w_defconfig
  ├── board/vanassistant-rpi0w/        # scripts / fichiers boot
  ├── vanassistant_overlay/            # rootfs overlay injecté à l'image
  └── output/                          # build (host/, images/, target/, staging/)

~/sdk/rpi-zero/                        # kernel Pi Zero W custom (hors Buildroot)
  ├── Makefile.kernel  make_kernel.sh
  ├── update_vanassistant.sh  update_kernellogo.sh
  ├── kernel.img  bcm2835-rpi-zero-w.dtb  overlays/
  └── logo.jpg  → drivers/video/logo/logo_linux_clut224.ppm patché

~/sdk/rpi-kernel/                      # sources kernel Raspberry Pi (rpi-6.6.y)
  └── 0001-fullscreen-always-shown-logo.patch  (patch bootlogo custom)
```

⚠️ Le répertoire `vanassistant-pizero-buildroot/` est une **tentative
d'architecture propre BR2_EXTERNAL abandonnée** : `configure` initialise un
build dans `build.buildroot/` mais l'arbre externe (`vanassistant-tree/`)
n'a jamais été peuplé (`Config.in` et `external.desc` vides dans
`~/sdk/vanassistant-buildroot-external/`), le symlink `buildroot` pointe
encore vers 2024.02.4, et aucun defconfig n'y est référencé. La
configuration **effective** est celle qui vit directement sous
`~/sdk/buildroot-2025.02.4/` (in‑tree, pas BR2_EXTERNAL).

### 13.2 Composants clés

**defconfig `vanassistant_rpi0w_defconfig`** (dérivé de `raspberrypi0w_defconfig`) :
- ARM ARM1176JZF-S (ARMv6), VFPv2, `hard float`, glibc
- Toolchain interne Buildroot, GCC 13.3.0, C++ activé
- Kernel : tarball custom pinné (`raspberrypi/linux@576cc10e…` ≈ rpi‑6.6.y),
  defconfig `bcmrpi`, DTS `broadcom/bcm2708-rpi-zero-w`
- Overlay rootfs : `~/sdk/buildroot/vanassistant_overlay`
- Post‑build : `board/vanassistant-rpi0w/post-build.sh`
- Post‑image : `board/vanassistant-rpi0w/post-image.sh`
- Rootfs ext4, 120 MiB (`.config` réel : 128 MiB)
- Firmware Pi + `config.txt` custom, `busybox`, `xz`, `kmod`
- `hostname=vanassistant`, `root passwd=toor` (SHA-256)

**`board/vanassistant-rpi0w/config.txt`** — configuration boot Pi :
```
boot_delay=0
disable_splash=1
disable_overscan=1
enable_uart=1                      ← nécessaire pour /dev/ttyAMA0
framebuffer_width=320              ← résolution écran
framebuffer_height=240
kernel=zImage
gpu_mem_256=100
dtoverlay=disable-bt               ← libère UART0 (BT désactivé sur Zero W)
```

**`board/vanassistant-rpi0w/cmdline.txt`** — boot silencieux :
```
root=/dev/mmcblk0p2 rootwait quiet console=tty3 loglevel=3
v6.disable=1 fsck.mode=skip vt.global_cursor_default=0
```
Effet : console redirigée sur tty3 (pas tty1), aucun message affiché,
curseur invisible, fsck désactivé → écran totalement dispo pour VanAssistant.

**`board/vanassistant-rpi0w/post-build.sh`** — nettoyage du rootfs :
- vide `/etc/issue` et `/etc/motd`
- supprime `S01syslogd`, `S01seedrng`, `S02sysctl`
- (bloc getty tty1 commenté — pas de shell interactif exposé)

**`board/vanassistant-rpi0w/post-image.sh`** — assemble l'image :
- Copie `zImage` → `pi_kernel.img`
- Copie `~/sdk/rpi-zero/kernel.img` → `va_kernel.img` (kernel custom)
- Copie `cmdline.txt` + `config.txt` dans `rpi-firmware/`
- Génère `sdcard.img` via `genimage` (partition boot FAT32 32 MiB + rootfs ext4)

**`vanassistant_overlay/`** — injecté tel quel dans le rootfs :
| Fichier                                       | Rôle                                      |
| --------------------------------------------- | ----------------------------------------- |
| `etc/inittab`                                 | Init BusyBox, gettys désactivés           |
| `etc/init.d/S01a_vanassistant`                | `exec /opt/vanassistant/vanassistant`     |
| `opt/vanassistant/vanassistant`               | Symlink/copie du binaire principal        |
| `opt/vanassistant/vanassistant.buildroot`     | Binaire brut copié depuis `build.buildroot/` |
| `opt/vanassistant/testpgefb`                  | Binaire de test framebuffer (débug)       |
| `lib/modules/6.1.93+/…`                       | Modules kernel installés par le Makefile custom |

Le préfixe `S01a` fait que VanAssistant démarre **avant** tout le reste
(`S20…`, `S30…`) pour prendre le contrôle du framebuffer immédiatement.

### 13.3 Kernel custom `~/sdk/rpi-zero/`

Le kernel Buildroot par défaut (via tarball GitHub) est **remplacé** par un
kernel compilé manuellement depuis `~/sdk/rpi-kernel/` (branche rpi‑6.6.y).
Raison : patch `0001-fullscreen-always-shown-logo.patch` qui :
- retire le test `logo_shown = FBCON_LOGO_DONTSHOW` sous `CONSOLE_LOGLEVEL_QUIET`
  (le logo reste affiché même en boot silencieux)
- force `fb_center_logo = true` (centrage)
- ajoute `logo_height += (info->var.yres/2)-(logo_height/2)` dans `fbcon.c`
  (centrage vertical)
- remplace `drivers/video/logo/logo_linux_clut224.ppm` par le logo custom
  **J7 160×160** (généré depuis `logo.jpg` via `update_kernellogo.sh` :
  `jpegtopnm | ppmquant 224 | pnmnoraw`)

**`Makefile.kernel`** : `bcmrpi_defconfig` + `zImage dtbs modules`, sortie
`kernel.img` + `bcm2835-rpi-zero-w.dtb` + `overlays/`, install modules dans
`~/sdk/buildroot/vanassistant_overlay/lib/modules/`. Cross‑compile via
`arm-linux-gnueabihf-` (crosspigcc externe, pas la toolchain Buildroot).

### 13.4 Workflow complet de build d'image SD

**Chemin recommandé** — utiliser le wrapper skill (`.claude/scripts/va_buildroot.sh`)
qui centralise chemins, checks et logs :

```bash
.claude/scripts/va_buildroot.sh info          # affiche la config détectée
.claude/scripts/va_buildroot.sh check         # valide les pré-requis
.claude/scripts/va_buildroot.sh quick         # app + image (cas fréquent)
.claude/scripts/va_buildroot.sh full          # kernel + app + image (complet)
.claude/scripts/va_buildroot.sh flash /dev/sdX  # dd avec confirmation OUI
```

Sous-commandes détaillées : `build-app`, `build-kernel`, `update-logo <jpg>`,
`build-image`. Skill Claude Code associée : **vanassistant-buildroot**
(voir `.claude/skills/vanassistant-buildroot/SKILL.md`).

**Chemin bas-niveau** — invocation manuelle si le wrapper n'est pas dispo :

```bash
# 1. (une seule fois ou après modif kernel/patches)
cd ~/sdk/rpi-zero
./make_kernel.sh                    # compile kernel, installe modules dans overlay

# 2. Rebuild VanAssistant + copie dans overlay
cd ~/sdk/rpi-zero
./update_vanassistant.sh            # cmake build.buildroot + cp vers overlay/opt

# 3. Build image Buildroot (rebuild rootfs + génère sdcard.img)
cd ~/sdk/buildroot
make vanassistant_rpi0w_defconfig   # (première fois seulement)
make -j$(nproc)                     # produit output/images/sdcard.img

# 4. Flash
sudo dd if=~/sdk/buildroot/output/images/sdcard.img of=/dev/sdX bs=4M status=progress conv=fsync
```

Alternative via le CMake VanAssistant : dans `build.buildroot/`, la cible
`make image` invoque `make -C ~/sdk/buildroot -j4` puis copie `sdcard.img`
en local (`vanassistant.sdcard.img`).

### 13.5 Séquence de boot cible

1. `bootcode.bin` (Pi firmware) → `start.elf` → lit `config.txt`
2. Framebuffer 320×240 initialisé, `zImage` (kernel custom) chargé
3. Kernel affiche le logo J7 plein écran (patch) puis passe la console sur tty3
4. BusyBox init exécute `/etc/init.d/rcS`
5. `S01a_vanassistant` lance `/opt/vanassistant/vanassistant` — l'app prend
   le contrôle du framebuffer via le backend PGEFB

Aucun shell n'est exposé sur l'écran (getty commentés). Le débogage passe
par UART (`/dev/ttyAMA0`, 115200) ou en réactivant temporairement le getty
dans `etc/inittab`.

### 13.6 Contraintes / pièges spécifiques Buildroot

- Chemins **codés en dur** partout :
  - `~/sdk/buildroot` (symlink vers 2025.02.4)
  - `~/sdk/rpi-zero`, `~/sdk/rpi-kernel`
  - `~/dev/vanassistant/build.buildroot`
  - `~/sdk/buildroot/vanassistant_overlay/opt/vanassistant`
- La copie du binaire est faite **deux fois** :
  - par `POST_BUILD` du CMake VanAssistant (`vanassistant.buildroot` → overlay)
  - par `update_vanassistant.sh` (renommage en `vanassistant` sans extension)
  → toujours utiliser `update_vanassistant.sh` pour éviter les doublons.
- Le fichier `~/sdk/buildroot-2025.02.4/.config` diffère du defconfig
  (rootfs 128 MiB vs 120 MiB, options CPIO ajoutées). Toujours re‑générer
  via `make vanassistant_rpi0w_defconfig` avant modifications.
- Le kernel Buildroot (tarball GitHub) et le kernel custom (`~/sdk/rpi-zero/`)
  coexistent : `post-image.sh` copie les deux (`pi_kernel.img` + `va_kernel.img`)
  mais **seul le kernel désigné par `kernel=zImage` dans `config.txt` est
  utilisé au boot**. Le kernel custom doit être injecté manuellement en
  renommant `va_kernel.img` → `zImage` dans `rpi-firmware/` si on veut
  utiliser le patch bootlogo.
- Le répertoire wrapper `~/dev/vanassistant-pizero-buildroot/` est
  **obsolète** — ne pas y toucher sans discuter d'abord (peut être un
  chantier de refonte à finir ou à supprimer).
- Modifier un fichier de l'overlay (`inittab`, scripts init, config kernel)
  n'a d'effet qu'après un `make` complet de Buildroot (regénère `rootfs.ext4`
  et `sdcard.img`).

### 13.7 TODO connus (dans `~/sdk/rpi-zero/todo.md`)

- Logo J7 (image ou ASCII ?) — partiellement fait via patch
- Framebuffer 320×240 — fait (config.txt)
- Inclure un programme de test framebuffer — présent : `testpgefb`
- Inclure vanassistant — fait (overlay)
- Permettre le choix entre `testfb` et `vanassistant` — non fait
  (script `S01a_vanassistant` lance directement le binaire)

## 14. Firmwares compagnons (embedded/)

Projets PlatformIO indépendants (build séparé, chacun a son `platformio.ini`) :
- `atmega2560-va-serialhub` — hub série qui agrège les données vers le Pi.
- `esp32-s3-va-imu-sender` — envoie les données IMU sur série.
- `esp32-s3-va-imu-calibration` — routine de calibration IMU (one‑shot).

Ils émettent des paquets au format `J7PacketHeader` défini côté VanAssistant
et compris par `SerialProtocol::ReadSerial()`.

## 15. Outils (tools/)

Chaque outil a un CMake propre. Le CMake racine de `tools/` n'inclut que
ceux qui sont voulus (via `add_subdirectory`).

| Outil                | Rôle                                                   |
| -------------------- | ------------------------------------------------------ |
| `png-to-header`      | PNG → header C avec `Sprite` embarquable               |
| `font-to-header`     | TTF/BDF → header format Adafruit GFX                   |
| `object-to-header`   | Wavefront OBJ → header C (verts + faces)               |
| `obj-viewer`         | Aperçu 3D interactif d'un OBJ (X11)                    |
| `serial-gui`         | GUI de debug/injection de paquets série                |
| `project-generator`  | Instancie un nouveau projet DrzEngine depuis template  |

`project-generator` : `./project-generator -n MyProject -p LINUX_X11 -d /parent/dir`
copie `test_projects/template_drzengineproject/` avec substitutions.

Il existe également un script Python : `scripts/make_sprite_header.py file.png out.h`
mais le dossier `scripts/` n'est pas présent à la racine actuelle — vérifier
avant de le référencer.

## 16. Étapes typiques pour ajouter…

### Une nouvelle page
1. Créer `src/vanassistant/PageXxx.{h,cpp}` héritant de `DisplayPage("xxx")`.
2. Ajouter les include et instancier dans `VanAssistantApp::Setup()`.
3. `DisplayPageManager::AddPage(xxxPage);`
4. Câbler la navigation (bouton dans `PageMenu` ou timer).
5. Rebuild — le glob `src/vanassistant/*.cpp` prend le `.cpp` automatiquement.

### Un widget
1. Créer `src/vanassistant/pages/<page>/widgets/MyWidget.h` header‑only,
   héritant éventuellement de `Widget`.
2. L'inclure dans la page cible, l'instancier dans `Load()`, l'appeler dans
   `Update()` et `Render()`.

### Un sprite embarqué
1. `tools/png-to-header monfichier.png sortie.h`
2. Placer le header dans `src/vanassistant/pages/<page>/sprites/`.
3. Inclure et instancier `Sprite s(w, h, data);` puis `gfx->DrawSprite(x, y, &s);`.

### Une font
1. `tools/font-to-header police.ttf 12 sortie.h` (taille en pt).
2. Placer dans `src/vanassistant/fonts/`.
3. `gfx->LoadFont("nom", const_cast<font*>(&NomFont));` dans `Setup()`.

### Un nouveau paquet série
1. Ajouter le type à l'enum `J7PacketType` et le struct dans `SerialProtocol.h`.
2. Ajouter la méthode `WriteXxx` et `ReadXxx` symétriquement.
3. Ajouter le `case` dans `SerialProtocol::ReadSerial()`.
4. Étendre `ISerialProtocolReceiver` (méthode virtuelle pure).
5. Implémenter dans `VanAssistantApp` + dispatcher vers la page concernée.

## 17. Ce qu'il NE faut PAS faire

- Ne pas recréer les headers supprimés (`include/fb/*`, `include/gfx3d.h`,
  `include/olcPGEX_Graphics3D.h`) — ils vivent dans drzengine.
- Ne pas casser le mapping `PLATFORM → BACKEND` (référencé par les tests projets).
- Ne pas assumer la présence d'un logger, d'un framework de tests, ou d'un CI.
  Rien de tout ça n'est en place actuellement.
- Ne pas modifier les chemins absolus (drzengine, buildroot) sans confirmer
  avec l'utilisateur — ils reflètent son environnement de dev.
- Ne pas ajouter de nouveau `.cpp` à un sous‑dossier de `src/vanassistant/`
  sans vérifier que le glob CMake le capture (actuellement il ne le fait
  que sur `src/vanassistant/*.cpp`).
- Ne pas introduire de dépendance système non listée dans le README sans
  mettre à jour le README et le CMake.

## 18. Critères d'acceptation par défaut

Sauf mention contraire :
1. Le build `LINUX_X11` reste fonctionnel (`cmake .. && make`).
2. Aucune régression visible sur les pages existantes (menu / road / assistant).
3. Le code respecte les conventions ci‑dessus (headers, namespace, style).
4. Toute nouvelle plateforme ou toolchain est documentée dans le README.
5. Les modifications du protocole série restent binaire‑compatibles avec les
   firmwares embedded/ (ou ceux‑ci sont mis à jour de concert).

## 19. Format de réponse attendu

Quand une tâche est confiée en s'appuyant sur ce prompt, structurer la
réponse ainsi :

- **Objectif** : reformulation courte de la demande.
- **Contraintes** : plateformes touchées, contraintes de perf/mémoire, portabilité.
- **Plan** : étapes numérotées, fichiers concernés (`file:line`).
- **Implémentation** : diffs ou nouveaux fichiers.
- **Validation** : commandes de build/test à lancer et sorties attendues.
- **Suivi** : refactors ou tâches ouvertes générés par le changement.
