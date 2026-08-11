#!/bin/bash
# va_build_all.sh — vérifie que VanAssistant build sur toutes les cibles
#                   disponibles sur cette machine (smoke test multi-plateforme)
#
# Usage:
#   va_build_all.sh <sous-commande> [args…]
#
# Sous-commandes :
#   list                — liste les cibles + statut toolchain (disponible/manquant)
#   quick               — build LINUX_X11 seul (smoke test rapide)
#   build <target>      — build une cible (linux_x11, linux_fb, pi_x11, pi_fb, buildroot, wasm, win)
#   all [--parallel]    — build toutes les cibles disponibles, résumé à la fin
#   clean               — supprime tous les build.check.*/
#   log <target>        — affiche les 100 dernières lignes du log de build
#
# Chaque target est buildée dans build.check.<target>/ (isolée des build.*)
# Les logs sont dans build.check.<target>/build.log
# Exit code : 0 si tout OK, N = nombre de cibles en échec.

set -uo pipefail   # PAS de -e : on veut collecter tous les résultats

# ────────────────────────────────────────────────────────────────────
# Configuration
# ────────────────────────────────────────────────────────────────────
VA_REPO="${VA_REPO:-$HOME/dev/vanassistant}"
SCRIPTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOLCHAINS_DIR="$SCRIPTS_DIR/toolchains"

BR_ROOT="${BR_ROOT:-$HOME/sdk/buildroot}"
BR_TOOLCHAINFILE="$BR_ROOT/output/host/share/buildroot/toolchainfile.cmake"
PI_TOOLCHAINFILE="${PI_TOOLCHAINFILE:-$HOME/sdk/crosspigcc/toolchain.cmake}"
WIN_TOOLCHAINFILE="${WIN_TOOLCHAINFILE:-$TOOLCHAINS_DIR/mingw64.cmake}"

# Emscripten : soit EMSDK exporté, soit script d'activation à sourcer.
# On teste dans l'ordre :
#   1. $EMSDK (déjà activé dans le shell courant)
#   2. $EMSDK_ROOT/emsdk_env.sh
#   3. ~/sdk/emsdk/emsdk_env.sh (install par défaut de cette skill)
EMSDK_ENV_SCRIPT="${EMSDK_ENV_SCRIPT:-$HOME/sdk/emsdk/emsdk_env.sh}"
EMSCRIPTEN_TOOLCHAINFILE="${EMSCRIPTEN_TOOLCHAINFILE:-}"

# Cibles connues : nom lowercase (=build dir suffix) → nom PLATFORM CMake
declare -A TARGETS=(
    [linux_x11]=LINUX_X11
    [linux_fb]=LINUX_FB
    [pi_x11]=PI_X11
    [pi_fb]=PI_FB
    [buildroot]=BUILDROOT
    [wasm]=WASM
    [win]=WIN
)
# Ordre stable pour l'affichage
TARGET_ORDER=(linux_x11 linux_fb pi_x11 pi_fb buildroot wasm win)

# ────────────────────────────────────────────────────────────────────
# Utilitaires
# ────────────────────────────────────────────────────────────────────
log()  { printf '\033[1;36m▸\033[0m %s\n' "$*" >&2; }
warn() { printf '\033[1;33m⚠\033[0m %s\n' "$*" >&2; }
err()  { printf '\033[1;31m✖\033[0m %s\n' "$*" >&2; }
ok()   { printf '\033[1;32m✓\033[0m %s\n' "$*" >&2; }

nproc_safe() { command -v nproc >/dev/null 2>&1 && nproc || echo 4; }

# Source emsdk_env.sh si nécessaire, puis renseigne EMSCRIPTEN_TOOLCHAINFILE.
# Renvoie 0 si prêt, 1 sinon (avec SKIP_REASON).
detect_emscripten_toolchain() {
    # Cas 1 : toolchain déjà fourni par l'utilisateur
    if [ -n "$EMSCRIPTEN_TOOLCHAINFILE" ] && [ -f "$EMSCRIPTEN_TOOLCHAINFILE" ]; then
        return 0
    fi

    # Cas 2 : $EMSDK déjà exporté dans le shell courant
    if [ -z "${EMSDK:-}" ]; then
        # Cas 3 : sourcer le script d'activation s'il existe
        if [ -f "$EMSDK_ENV_SCRIPT" ]; then
            # shellcheck disable=SC1090
            source "$EMSDK_ENV_SCRIPT" >/dev/null 2>&1 || true
        fi
    fi

    if [ -z "${EMSDK:-}" ]; then
        SKIP_REASON="emsdk introuvable — installe-le puis 'source $EMSDK_ENV_SCRIPT' (ou exporte EMSDK)"
        return 1
    fi

    # Résolution du toolchain file selon la structure standard emsdk
    local candidate="$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
    if [ -f "$candidate" ]; then
        EMSCRIPTEN_TOOLCHAINFILE="$candidate"
        return 0
    fi
    SKIP_REASON="EMSDK défini ($EMSDK) mais Emscripten.cmake introuvable"
    return 1
}

# Renvoie 0 si la toolchain pour <target> est dispo, sinon renseigne SKIP_REASON
check_toolchain() {
    local target="$1"
    SKIP_REASON=""
    case "$target" in
        linux_x11)
            command -v cmake >/dev/null || { SKIP_REASON="cmake absent"; return 1; }
            dpkg -l libx11-dev libpng-dev libglu1-mesa-dev >/dev/null 2>&1 \
                || { SKIP_REASON="libs X11/PNG/GLU manquantes"; return 1; }
            ;;
        linux_fb)
            command -v cmake >/dev/null || { SKIP_REASON="cmake absent"; return 1; }
            ;;
        pi_x11|pi_fb)
            [ -f "$PI_TOOLCHAINFILE" ] || { SKIP_REASON="crosspigcc toolchain absent ($PI_TOOLCHAINFILE)"; return 1; }
            command -v arm-linux-gnueabihf-g++ >/dev/null 2>&1 \
                || { SKIP_REASON="arm-linux-gnueabihf-g++ pas dans le PATH"; return 1; }
            ;;
        buildroot)
            [ -f "$BR_TOOLCHAINFILE" ] || { SKIP_REASON="toolchainfile Buildroot absent ($BR_TOOLCHAINFILE) — lance 'make toolchain' dans $BR_ROOT"; return 1; }
            ;;
        wasm)
            # detect_emscripten_toolchain source emsdk_env.sh au besoin
            # et positionne EMSDK + PATH + EMSCRIPTEN_TOOLCHAINFILE.
            detect_emscripten_toolchain || return 1
            command -v emcmake >/dev/null 2>&1 \
                || { SKIP_REASON="emcmake toujours pas dans le PATH après source de $EMSDK_ENV_SCRIPT"; return 1; }
            ;;
        win)
            [ -f "$WIN_TOOLCHAINFILE" ] || { SKIP_REASON="mingw toolchain absent ($WIN_TOOLCHAINFILE)"; return 1; }
            command -v x86_64-w64-mingw32-g++-posix >/dev/null 2>&1 \
                || { SKIP_REASON="x86_64-w64-mingw32-g++-posix absent (apt install g++-mingw-w64-x86-64)"; return 1; }
            ;;
        *) SKIP_REASON="cible inconnue : $target"; return 1 ;;
    esac
    return 0
}

# Renvoie la commande cmake pour <target> (via echo, à eval par le caller)
cmake_cmd_for() {
    local target="$1" build_dir="$2"
    local platform="${TARGETS[$target]}"
    local common="-S $VA_REPO -B $build_dir -DPLATFORM=$platform"
    case "$target" in
        linux_x11|linux_fb) echo "cmake $common" ;;
        pi_x11|pi_fb)       echo "cmake $common -DCMAKE_TOOLCHAIN_FILE=$PI_TOOLCHAINFILE" ;;
        buildroot)          echo "cmake $common -DCMAKE_TOOLCHAIN_FILE=$BR_TOOLCHAINFILE" ;;
        win)                echo "cmake $common -DCMAKE_TOOLCHAIN_FILE=$WIN_TOOLCHAINFILE" ;;
        wasm)               echo "emcmake cmake $common" ;;
    esac
}

# ────────────────────────────────────────────────────────────────────
# Sous-commandes
# ────────────────────────────────────────────────────────────────────

cmd_list() {
    printf '%-12s  %-10s  %s\n' "TARGET" "STATUT" "COMMENTAIRE"
    printf '%-12s  %-10s  %s\n' "──────" "──────" "───────────"
    for t in "${TARGET_ORDER[@]}"; do
        if check_toolchain "$t"; then
            printf '\033[32m%-12s  %-10s\033[0m  %s\n' "$t" "OK" "prêt à builder"
        else
            printf '\033[33m%-12s  %-10s\033[0m  %s\n' "$t" "SKIP" "$SKIP_REASON"
        fi
    done
}

# Build une cible, log complet dans build.check.<target>/build.log
# Renvoie 0 si succès, 1 sinon.
_build_one() {
    local target="$1"
    [ -n "${TARGETS[$target]:-}" ] || { err "cible inconnue : $target"; return 1; }

    if ! check_toolchain "$target"; then
        warn "$target : SKIP — $SKIP_REASON"
        return 2   # code spécial "skipped"
    fi

    local build_dir="$VA_REPO/build.check.$target"
    local log_file="$build_dir/build.log"
    mkdir -p "$build_dir"

    log "$target : configure + build → $build_dir (log : $log_file)"
    # Sous-shell (et non { }) pour deux raisons :
    #   - le statut retourné est celui de la dernière commande réelle (cmake --build),
    #     alors qu'un « set +x » final masquait tout échec derrière son propre rc=0 ;
    #   - « set -ex » reste confiné au sous-shell, donc pas besoin de le désactiver
    #     et le configure qui échoue court-circuite le build.
    (
        echo "=== $(date -Iseconds) — $target ==="
        echo "PLATFORM=${TARGETS[$target]}"
        set -ex
        eval "$(cmake_cmd_for "$target" "$build_dir")"
        cmake --build "$build_dir" -j"$(nproc_safe)"
    ) >"$log_file" 2>&1

    local rc=$?
    if [ $rc -eq 0 ]; then
        ok "$target : BUILD OK"
    else
        err "$target : BUILD FAILED (rc=$rc) — voir $log_file (ou: $0 log $target)"
    fi
    return $rc
}

cmd_build() {
    local target="${1:-}"
    [ -n "$target" ] || { err "usage: build <target>"; return 1; }
    _build_one "$target"
}

cmd_quick() {
    log "Smoke test : LINUX_X11 uniquement"
    _build_one linux_x11
}

cmd_all() {
    # Init explicite : sous `set -u`, l'expansion "${arr[*]}" échoue si l'array
    # n'a jamais reçu d'élément — même avec :-fallback.
    local -a results_ok=() results_fail=() results_skip=()
    local target rc
    log "Build de toutes les cibles disponibles…"
    for target in "${TARGET_ORDER[@]}"; do
        _build_one "$target"
        rc=$?
        case $rc in
            0) results_ok+=("$target") ;;
            2) results_skip+=("$target") ;;
            *) results_fail+=("$target") ;;
        esac
    done

    # Résumé — passer par des variables intermédiaires évite l'expansion
    # d'array vide sous set -u.
    local ok_str="(aucun)" skip_str="(aucun)" fail_str="(aucun)"
    [ ${#results_ok[@]}   -gt 0 ] && ok_str="${results_ok[*]}"
    [ ${#results_skip[@]} -gt 0 ] && skip_str="${results_skip[*]}"
    [ ${#results_fail[@]} -gt 0 ] && fail_str="${results_fail[*]}"

    printf '\n'
    printf '═══ RÉSUMÉ ═════════════════════════════════════════════\n'
    printf '  \033[32mOK\033[0m    (%d) : %s\n'   "${#results_ok[@]}"   "$ok_str"
    printf '  \033[33mSKIP\033[0m  (%d) : %s\n'   "${#results_skip[@]}" "$skip_str"
    printf '  \033[31mFAIL\033[0m  (%d) : %s\n'   "${#results_fail[@]}" "$fail_str"
    printf '═════════════════════════════════════════════════════════\n'

    return "${#results_fail[@]}"
}

cmd_clean() {
    log "Nettoyage des build.check.*/"
    local count=0
    for d in "$VA_REPO"/build.check.*/; do
        [ -d "$d" ] || continue
        rm -rf "$d"
        log "  supprimé : $d"
        count=$((count + 1))
    done
    ok "$count répertoire(s) supprimé(s)"
}

cmd_log() {
    local target="${1:-}"
    [ -n "$target" ] || { err "usage: log <target>"; return 1; }
    local log_file="$VA_REPO/build.check.$target/build.log"
    [ -f "$log_file" ] || { err "aucun log pour $target ($log_file)"; return 1; }
    tail -100 "$log_file"
}

show_help() {
    awk 'NR==1 { next } /^#/ { sub(/^# ?/, ""); print; next } { exit }' "$0"
}

# ────────────────────────────────────────────────────────────────────
# Dispatcher
# ────────────────────────────────────────────────────────────────────
sub="${1:-}"
[ -n "$sub" ] || { show_help; exit 1; }
shift
case "$sub" in
    list)   cmd_list "$@" ;;
    quick)  cmd_quick "$@" ;;
    build)  cmd_build "$@" ;;
    all)    cmd_all "$@" ;;
    clean)  cmd_clean "$@" ;;
    log)    cmd_log "$@" ;;
    *)      err "sous-commande inconnue : $sub"; show_help; exit 1 ;;
esac
