# Changelog

Toutes les modifications notables du projet VanAssistant sont documentées
dans ce fichier.

Format inspiré de [Keep a Changelog](https://keepachangelog.com/fr/1.1.0/).
Les entrées sont datées par session (format `YYYY-MM-DD`) plutôt que
versionnées, faute de cycle de release formalisé.

Portée : couvre le code de l'app, la config CMake, les outils (`tools/`),
les skills et scripts d'automatisation Claude (`.claude/`), les prompts
projet (`.github/prompts/`), et les artefacts de la distribution Buildroot
qui vivent hors repo (`~/sdk/buildroot/**`, `~/sdk/rpi-zero/**`) quand ils
sont modifiés dans le cadre du projet.

Hors portée : builds locaux (`build.*/`), fichiers générés, modifications
d'environnement personnel (dotfiles, PATH…).

---

## [Non publié]

<!-- Ajouter ici les changements en cours avant la prochaine "session close" -->

---

## 2026-08-11

### Ajouté
- **Prompt projet** `.github/prompts/vanassistant.prompt.md` (19 sections) —
  contexte complet : architecture, mapping PLATFORM↔BACKEND, arborescence,
  cycle de vie des pages, protocole série J7 + Victron, système graphique
  320×240, distribution Buildroot embarquée (§13), firmwares compagnons,
  outils, recettes d'ajout (page/widget/sprite/font/paquet), pièges connus.
- **Skill `vanassistant-buildroot`** (`.claude/skills/vanassistant-buildroot/SKILL.md`)
  et wrapper `.claude/scripts/va_buildroot.sh` — orchestre la génération
  de l'image SD Pi Zero W :
  - Sous-commandes : `info`, `check`, `build-app`, `build-kernel`,
    `update-logo <jpg>`, `build-image`, `full`, `quick`, `flash /dev/sdX`.
  - Centralise les chemins (`~/sdk/buildroot`, `~/sdk/rpi-zero`,
    `~/sdk/rpi-kernel`) surchargeables par env vars.
  - `flash` exige confirmation `OUI` explicite après affichage `lsblk`
    du device (opération destructive).
- **Skill `vanassistant-build-all`** (`.claude/skills/vanassistant-build-all/SKILL.md`)
  et wrapper `.claude/scripts/va_build_all.sh` — smoke test multi-plateforme :
  - Cibles gérées : `linux_x11`, `linux_fb`, `pi_x11`, `pi_fb`,
    `buildroot`, `wasm`, `win`.
  - Sous-commandes : `list`, `quick`, `build <target>`, `all`, `clean`,
    `log <target>`.
  - Build isolé dans `build.check.<target>/` (n'interfère pas avec
    les `build.*` de release) ; log complet par cible.
  - Exit code = nombre de FAIL. `SKIP` non comptabilisés en échec.
  - Auto-source de `~/sdk/emsdk/emsdk_env.sh` si `EMSDK` non exporté.
- **Toolchain Windows** `.claude/scripts/toolchains/mingw64.cmake` —
  fichier référencé (mais absent) dans `CMakeLists.txt` L160 ; désormais
  fourni, utilise `x86_64-w64-mingw32-g++-posix`.
- **CHANGELOG.md** (ce fichier) — maintenance persistante entre sessions
  via mémoire Claude (`~/.claude/projects/-home-drzoid-dev-vanassistant/memory/feedback_changelog.md`).

### Modifié
- `~/sdk/emsdk/` : installation d'emsdk 6.0.6 (git clone +
  `emsdk install latest` + `emsdk activate latest`) pour débloquer la
  compilation WASM. Le wrapper `va_build_all.sh` détecte et sources
  automatiquement `emsdk_env.sh` depuis ce chemin.

### Corrigé
- `.claude/scripts/va_build_all.sh` : bug d'expansion d'array vide sous
  `set -u` dans `cmd_all` (les tableaux `results_skip` / `results_fail`
  n'étaient pas explicitement initialisés — l'expansion `"${arr[*]:-…}"`
  échouait avec « unbound variable » quand aucun élément n'avait été
  ajouté). Correctif : `local -a results_ok=() results_fail=() results_skip=()`
  + variables intermédiaires pour le résumé.

### Validé
- `.claude/scripts/va_build_all.sh all` : **7/7 cibles OK**
  (`linux_x11`, `linux_fb`, `pi_x11`, `pi_fb`, `buildroot`, `wasm`, `win`).
