---
name: vanassistant-buildroot
description: Génère la distribution Buildroot embarquée VanAssistant pour Raspberry Pi Zero W — cross-compile le binaire, compile le kernel custom, met à jour le bootlogo, produit sdcard.img, flashe une carte SD. À utiliser dès que l'utilisateur demande "build buildroot", "génère l'image SD", "recompile pour le pi", "flash la carte", "mets à jour le firmware du van", "rebuild kernel", "change le logo de boot", ou tout flux d'assemblage de la distribution embarquée.
---

# Skill — Génération de la distribution Buildroot VanAssistant

Cette skill orchestre la chaîne de build de l'image SD Raspberry Pi Zero W de
VanAssistant (framebuffer 320×240, boot silencieux, logo J7 custom, app
lancée par init). Elle repose sur trois emplacements documentés dans
`.github/prompts/vanassistant.prompt.md §13`.

**Toujours** passer par le wrapper `.claude/scripts/va_buildroot.sh`.
Ne pas invoquer `make`, `dd`, `cmake` ou les scripts de `~/sdk/rpi-zero/`
directement — le wrapper centralise les chemins, la validation, les logs
colorés et la confirmation sur les opérations destructives.

## Chemins de référence

| Variable          | Valeur par défaut                                                        |
| ----------------- | ------------------------------------------------------------------------ |
| `VA_REPO`         | `~/dev/vanassistant`                                                     |
| `VA_BUILD_DIR`    | `~/dev/vanassistant/build.buildroot`                                     |
| `BR_ROOT`         | `~/sdk/buildroot` (symlink → `buildroot-2025.02.4`)                      |
| `BR_OVERLAY`      | `~/sdk/buildroot/vanassistant_overlay`                                   |
| `BR_OVERLAY_APP`  | `~/sdk/buildroot/vanassistant_overlay/opt/vanassistant`                  |
| `BR_DEFCONFIG`    | `vanassistant_rpi0w_defconfig`                                           |
| `BR_TOOLCHAINFILE`| `~/sdk/buildroot/output/host/share/buildroot/toolchainfile.cmake`        |
| `BR_SDIMG`        | `~/sdk/buildroot/output/images/sdcard.img`                               |
| `KERNEL_DIR`      | `~/sdk/rpi-kernel` (sources kernel rpi-6.6.y patchées)                   |
| `RPI_ZERO_DIR`    | `~/sdk/rpi-zero` (Makefile.kernel + logo.jpg + kernel.img)               |

Toutes surchargeables par variables d'environnement (utile pour tests).

## Sous-commandes disponibles

```bash
.claude/scripts/va_buildroot.sh <sous-commande> [args…]
```

| Sous-commande             | Rôle                                                                             |
| ------------------------- | -------------------------------------------------------------------------------- |
| `info`                    | Affiche chemins + versions détectées (BR2_DEFCONFIG, GCC, kernel, hostname)      |
| `check`                   | Vérifie que tous les chemins/toolchains sont présents. Idempotent.               |
| `build-app`               | cmake configure (si besoin) + make ARM cross → copie dans overlay                |
| `build-kernel`            | Wrap `~/sdk/rpi-zero/Makefile.kernel` : zImage + dtbs + modules_install          |
| `update-logo <fichier.jpg>` | Convertit un JPG en `logo_linux_clut224.ppm` (224 couleurs) et l'injecte dans le kernel. Rebuild kernel obligatoire ensuite. |
| `build-image`             | `make -jN` de Buildroot → `sdcard.img`                                           |
| `full`                    | Enchaîne kernel + app + image                                                    |
| `quick`                   | app + image (skip kernel — cas fréquent après modif code app)                    |
| `flash /dev/sdX`          | `dd sdcard.img` avec confirmation `OUI` explicite obligatoire                    |

## Workflows

### W1 — Modif appli, rebuild rapide

L'utilisateur a modifié du code C++ dans `src/vanassistant/`, veut une nouvelle
image SD sans retoucher le kernel.

```bash
.claude/scripts/va_buildroot.sh quick
```
Enchaîne `build-app` puis `build-image`. Ne recompile PAS le kernel.
Résultat : `~/sdk/buildroot/output/images/sdcard.img` mis à jour.

### W2 — Génération complète (première image, après modif kernel/logo)

```bash
.claude/scripts/va_buildroot.sh full
```
Enchaîne `build-kernel` → `build-app` → `build-image`.
Peut prendre 30–60 min (compilation kernel + Buildroot).

### W3 — Nouveau logo de boot

```bash
.claude/scripts/va_buildroot.sh update-logo ~/sdk/rpi-zero/logo.jpg
.claude/scripts/va_buildroot.sh build-kernel
.claude/scripts/va_buildroot.sh build-image
```
Le nouveau logo est intégré en dur dans le kernel (patch bootlogo). Idéalement
image source **160×160** pour matcher le patch `0001-fullscreen-always-shown-logo`.

### W4 — Flash de la carte SD

Toujours en dernier, jamais sans confirmation utilisateur explicite.

```bash
# 1. Identifier le device (JAMAIS deviner)
lsblk

# 2. L'utilisateur confirme le chemin exact (ex. /dev/sdb, /dev/mmcblk0)
.claude/scripts/va_buildroot.sh flash /dev/sdX
```
Le wrapper demande une confirmation `OUI` en majuscules avant le `dd`.
**Ne jamais** invoquer `flash` sur un device que l'utilisateur n'a pas
explicitement nommé — risque de destruction du disque système.

### W5 — Diagnostic

Avant tout build en cas de doute :
```bash
.claude/scripts/va_buildroot.sh info    # visualise la config
.claude/scripts/va_buildroot.sh check   # valide les pré-requis
```

## Règles non-négociables

1. **Toujours** passer par le wrapper — pas de `make`, `cmake`, `dd` ad-hoc.
2. **`flash` uniquement sur device confirmé par l'utilisateur**. Ne jamais
   deviner ni utiliser `/dev/sda` par défaut.
3. **Ne pas modifier** `~/dev/vanassistant-pizero-buildroot/` (skeleton
   obsolète, cf. §13.1 du prompt). Le vrai setup vit dans `~/sdk/buildroot`.
4. **Modification defconfig** = modifier `~/sdk/buildroot/configs/vanassistant_rpi0w_defconfig`
   PUIS `make vanassistant_rpi0w_defconfig` dans `$BR_ROOT` pour régénérer `.config`.
   Modifier directement `.config` = perdu au prochain reconfigure.
5. **Ajout d'un fichier au rootfs** = ajouter à `~/sdk/buildroot/vanassistant_overlay/`
   PUIS `build-image` (l'overlay est réappliqué à chaque build image).
6. **Modification config kernel** (defconfig `bcmrpi` ou patches) = requiert
   `build-kernel` pour propager.
7. **Le kernel Buildroot** (tarball GitHub) et **le kernel custom** (`~/sdk/rpi-zero/`)
   coexistent dans `rpi-firmware/`. Le boot utilise celui pointé par
   `kernel=zImage` dans `config.txt`. Pour utiliser le kernel custom patché
   (logo J7), renommer `va_kernel.img` → `zImage` dans l'image de sortie, ou
   adapter `post-image.sh`.
8. **Coupures/reprises** : `make` de Buildroot est incrémental — une coupure
   se reprend proprement. `build-app` est incrémental via cmake/make.
   `build-kernel` est incrémental sauf après un `clean`.

## Erreurs courantes → diagnostic

| Symptôme                                            | Cause probable / fix                                                 |
| --------------------------------------------------- | -------------------------------------------------------------------- |
| `toolchainfile absent`                              | `cd $BR_ROOT && make vanassistant_rpi0w_defconfig && make toolchain` |
| `arm-linux-gnueabihf-gcc: not found`                | crosspigcc absent du PATH — sourcer `~/sdk/crosspigcc/setup.sh`      |
| `jpegtopnm: not found`                              | `sudo apt install netpbm`                                            |
| `permission denied` sur `sudo dd`                   | l'utilisateur doit être dans le groupe sudo                          |
| Boot silencieux mais écran noir                     | vérifier `S01a_vanassistant` dans overlay + binaire présent + exec   |
| Écran affiche console tty au lieu du logo J7        | kernel Buildroot utilisé au lieu du kernel custom patché (règle 7)   |
| App ne trouve pas `/dev/ttyAMA0`                    | vérifier `enable_uart=1` et `dtoverlay=disable-bt` dans config.txt   |

## Après flash — vérifications sur le Pi

Booter le Pi et vérifier via UART (`/dev/ttyAMA0`, 115200 bauds) :
```
picocom /dev/ttyUSB0 -b 115200
```
- Voir "Launching vanassistant..." → l'init a bien exécuté `S01a_vanassistant`
- Ping des paquets J7 en console → le port série reçoit bien
- `Ctrl+A Ctrl+X` pour quitter picocom

En cas de crash, réactiver un getty pour shell interactif :
```bash
# Éditer ~/sdk/buildroot/vanassistant_overlay/etc/inittab
# Décommenter : tty1::respawn:/sbin/getty -L tty1 0 vt100
# Puis: .claude/scripts/va_buildroot.sh build-image
```

## Références

- Prompt projet : `.github/prompts/vanassistant.prompt.md` §13 (Distribution Buildroot embarquée)
- CMake VanAssistant : `CMakeLists.txt` (cible BUILDROOT L193-227)
- Board dir : `~/sdk/buildroot/board/vanassistant-rpi0w/`
- Overlay : `~/sdk/buildroot/vanassistant_overlay/`
- Kernel : `~/sdk/rpi-zero/README.md`
