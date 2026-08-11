---
name: vanassistant-build-all
description: Vérifie que VanAssistant compile sur toutes les cibles disponibles (LINUX_X11, LINUX_FB, PI_X11, PI_FB, BUILDROOT, WASM, WIN) — smoke test multi-plateforme après modifications. À utiliser quand l'utilisateur demande "build all", "ça build partout ?", "smoke test", "vérifie que ça compile", "check les builds", "j'ai modifié X, est-ce que ça casse rien", "matrice de build", ou après tout refactor touchant à du code partagé entre plateformes.
---

# Skill — Build multi-cible VanAssistant

Cette skill compile VanAssistant sur toutes les plateformes disponibles sur
la machine de dev pour attraper les régressions cross-plateforme (headers
absents, symboles Windows-only, incompatibilités WASM…).

**Toujours** passer par le wrapper `.claude/scripts/va_build_all.sh`. Chaque
cible est buildée dans un répertoire isolé `build.check.<target>/` (jamais
`build/`, `build.buildroot/`, `build.pifb/` qui appartiennent aux workflows
de release/deploy).

## Cibles gérées

| Nom (arg CLI) | PLATFORM CMake | Backend | Toolchain requise                    |
| ------------- | -------------- | ------- | ------------------------------------ |
| `linux_x11`   | `LINUX_X11`    | PGE     | gcc/g++ + libx11-dev + libpng-dev + libglu1-mesa-dev |
| `linux_fb`    | `LINUX_FB`     | PGEFB   | gcc/g++ standard                     |
| `pi_x11`      | `PI_X11`       | PGE     | crosspigcc (`~/sdk/crosspigcc/toolchain.cmake`) |
| `pi_fb`       | `PI_FB`        | PGEFB   | crosspigcc (idem)                    |
| `buildroot`   | `BUILDROOT`    | PGEFB   | Buildroot toolchainfile (voir skill `vanassistant-buildroot`) |
| `wasm`        | `WASM`         | PGE     | emsdk activé (`source $EMSDK/emsdk_env.sh`) |
| `win`         | `WIN`          | PGE     | mingw-w64 + `.claude/scripts/toolchains/mingw64.cmake` |

Chaque cible est détectée automatiquement — celles dont la toolchain manque
sont marquées `SKIP` (pas d'échec).

## Sous-commandes

```bash
.claude/scripts/va_build_all.sh <sous-commande> [args…]
```

| Sous-commande         | Rôle                                                             |
| --------------------- | ---------------------------------------------------------------- |
| `list`                | Statut des toolchains (OK / SKIP + raison). N'invoque rien.      |
| `quick`               | Build LINUX_X11 seul — smoke test rapide (30–60 s)               |
| `build <target>`      | Build une seule cible                                            |
| `all`                 | Build toutes les cibles disponibles séquentiellement + résumé    |
| `log <target>`        | Tail 100 lignes du log de la dernière build de la cible          |
| `clean`               | Supprime tous les `build.check.*/`                               |

Exit code : `0` si tout OK, `N` = nombre de cibles en `FAIL` (les `SKIP` ne comptent pas).

## Workflows

### W1 — Smoke test rapide après une modif locale

Cas dominant : l'utilisateur a modifié 1–2 fichiers, veut confirmer que la
compil desktop passe toujours. **Toujours commencer par ça.**

```bash
.claude/scripts/va_build_all.sh quick
```

30–60 secondes. En cas d'échec : `.claude/scripts/va_build_all.sh log linux_x11`
pour voir la sortie compilateur.

### W2 — Vérification cross-plateforme avant PR / merge

Modif touchant à du code partagé (headers `include/`, `src/vanassistant/`,
`DrzEngine` externe, protocole série…) → risque de casser une cible cross-compile
(WASM strict, Windows GDI+, Buildroot glibc arm hard-float).

```bash
.claude/scripts/va_build_all.sh list        # aperçu de ce qui va tourner
.claude/scripts/va_build_all.sh all         # build séquentiel de tout ce qui est OK
```

À la fin, un tableau résumé :
```
═══ RÉSUMÉ ═════════════════════════════════════════════
  OK    (5) : linux_x11 linux_fb pi_x11 pi_fb win
  SKIP  (1) : wasm
  FAIL  (1) : buildroot
═════════════════════════════════════════════════════════
```

Puis pour chaque `FAIL` : `.claude/scripts/va_build_all.sh log <target>`.

### W3 — Débug d'une cible spécifique

```bash
.claude/scripts/va_build_all.sh build wasm     # tente wasm en isolé
.claude/scripts/va_build_all.sh log wasm       # sortie complète
```

Le répertoire `build.check.wasm/` est conservé pour investigation manuelle
(re-run cmake, ajouter `-DCMAKE_VERBOSE_MAKEFILE=ON`, etc.).

### W4 — Repartir sur base propre

```bash
.claude/scripts/va_build_all.sh clean
.claude/scripts/va_build_all.sh all
```

Utile si un renaming de fichiers ou une modif du CMake laisse des artefacts
incohérents.

## Détection des toolchains — troubleshooting

| Cible       | Sonde                                                            | Fix si absent                                             |
| ----------- | ---------------------------------------------------------------- | --------------------------------------------------------- |
| `linux_x11` | `dpkg -l libx11-dev libpng-dev libglu1-mesa-dev`                 | `sudo apt install libx11-dev libpng-dev libglu1-mesa-dev` |
| `linux_fb`  | `cmake`                                                          | `sudo apt install cmake`                                  |
| `pi_x11/fb` | `~/sdk/crosspigcc/toolchain.cmake` + `arm-linux-gnueabihf-g++`   | installer crosspigcc, PATH à jour                         |
| `buildroot` | `~/sdk/buildroot/output/host/share/buildroot/toolchainfile.cmake` | `cd ~/sdk/buildroot && make vanassistant_rpi0w_defconfig && make toolchain` |
| `wasm`      | var `EMSDK` définie ou `EMSCRIPTEN_TOOLCHAINFILE` défini + `emcmake` | `source ~/emsdk/emsdk_env.sh` (installer emsdk d'abord)   |
| `win`       | `.claude/scripts/toolchains/mingw64.cmake` + `x86_64-w64-mingw32-g++-posix` | `sudo apt install g++-mingw-w64-x86-64` |

Les chemins sont surchargeables :
```bash
BR_TOOLCHAINFILE=... PI_TOOLCHAINFILE=... .claude/scripts/va_build_all.sh all
```

## Règles non-négociables

1. **Toujours** utiliser le wrapper — pas de `cmake` ni `make` ad-hoc pour vérifier une compil.
2. **Ne pas confondre** `build.check.<target>/` (smoke test, jetable, cette skill)
   avec `build.buildroot/` (release, deploy, skill `vanassistant-buildroot`) ni
   `build/` / `build.pifb/` (builds "de travail" de l'utilisateur).
   `clean` ne touche que `build.check.*`.
3. **Ne pas exécuter le binaire produit** — cette skill vérifie la compil, pas le comportement.
   Les tests visuels/runtime restent manuels.
4. **En cas de FAIL sur une cible cross-compile**, toujours lire d'abord
   `log <target>` avant de proposer un fix — le message d'erreur est souvent
   spécifique à la toolchain (symbole indispo, header manquant, ABI mismatch).
5. **Ne pas modifier `~/sdk/*`** pour "faire passer" un build — signaler à
   l'utilisateur si un pré-requis manque (défaut de setup, pas défaut du code).
6. Si l'utilisateur travaille sur une seule plateforme, préférer `build <target>`
   à `all` (économie de temps CPU).

## Interaction avec la skill vanassistant-buildroot

- `vanassistant-build-all build buildroot` = vérifie que le code compile pour
  l'ARM, dans un dossier isolé, sans copier dans l'overlay.
- `vanassistant-buildroot build-app` = compile ET déploie dans l'overlay pour
  intégration dans l'image SD.

Utiliser cette skill pour valider un changement avant de passer à
`vanassistant-buildroot` pour la génération d'image.

## Références

- Prompt projet : `.github/prompts/vanassistant.prompt.md` §3 (mapping PLATFORM → BACKEND) et §9 (commandes de build)
- CMake VanAssistant : `CMakeLists.txt` L30–68 (mapping) et L101–294 (blocs par plateforme)
- Skill sœur : `.claude/skills/vanassistant-buildroot/SKILL.md`
