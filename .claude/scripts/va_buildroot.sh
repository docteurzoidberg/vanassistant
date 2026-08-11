#!/bin/bash
# va_buildroot.sh — orchestrateur de génération de la distribution Buildroot VanAssistant
#
# Usage:
#   va_buildroot.sh <sous-commande> [args…]
#
# Sous-commandes :
#   check              — vérifie chemins, toolchains, symlinks, présence des cibles
#   build-app          — cross-compile vanassistant (build.buildroot/) et copie dans overlay
#   build-kernel       — compile le kernel Pi Zero W custom + installe modules dans overlay
#   update-logo <jpg>  — remplace le bootlogo (jpg → logo_linux_clut224.ppm)
#   build-image        — invoque `make` de Buildroot → sdcard.img
#   full               — enchaîne build-kernel + build-app + build-image
#   quick              — build-app + build-image (skip kernel : cas fréquent)
#   flash <device>     — dd sdcard.img vers /dev/sdX (avec confirmation)
#   info               — affiche les chemins et versions détectés
#
# Toute erreur est fatale (set -e). Chaque sous-commande log ce qu'elle fait.

set -euo pipefail

# ────────────────────────────────────────────────────────────────────
# Chemins de référence (aligner avec le prompt vanassistant §13)
# ────────────────────────────────────────────────────────────────────
VA_REPO="${VA_REPO:-$HOME/dev/vanassistant}"
VA_BUILD_DIR="${VA_BUILD_DIR:-$VA_REPO/build.buildroot}"
VA_BIN="vanassistant.buildroot"

BR_ROOT="${BR_ROOT:-$HOME/sdk/buildroot}"                     # symlink → buildroot-2025.02.4
BR_OVERLAY="${BR_OVERLAY:-$BR_ROOT/vanassistant_overlay}"
BR_OVERLAY_APP="${BR_OVERLAY_APP:-$BR_OVERLAY/opt/vanassistant}"
BR_DEFCONFIG="${BR_DEFCONFIG:-vanassistant_rpi0w_defconfig}"
BR_TOOLCHAINFILE="${BR_TOOLCHAINFILE:-$BR_ROOT/output/host/share/buildroot/toolchainfile.cmake}"
BR_SDIMG="${BR_SDIMG:-$BR_ROOT/output/images/sdcard.img}"

KERNEL_DIR="${KERNEL_DIR:-$HOME/sdk/rpi-kernel}"
RPI_ZERO_DIR="${RPI_ZERO_DIR:-$HOME/sdk/rpi-zero}"

# ────────────────────────────────────────────────────────────────────
# Utilitaires
# ────────────────────────────────────────────────────────────────────
log()  { printf '\033[1;36m▸\033[0m %s\n' "$*" >&2; }
warn() { printf '\033[1;33m⚠\033[0m %s\n' "$*" >&2; }
err()  { printf '\033[1;31m✖\033[0m %s\n' "$*" >&2; exit 1; }
ok()   { printf '\033[1;32m✓\033[0m %s\n' "$*" >&2; }

require_file() {
    [ -e "$1" ] || err "chemin introuvable : $1"
}

require_dir() {
    [ -d "$1" ] || err "répertoire introuvable : $1"
}

nproc_safe() {
    command -v nproc >/dev/null 2>&1 && nproc || echo 4
}

# ────────────────────────────────────────────────────────────────────
# Sous-commandes
# ────────────────────────────────────────────────────────────────────

cmd_info() {
    printf '%s\n' "─── Chemins VanAssistant Buildroot ────────────────────"
    printf 'VA_REPO           %s\n' "$VA_REPO"
    printf 'VA_BUILD_DIR      %s\n' "$VA_BUILD_DIR"
    printf 'BR_ROOT           %s' "$BR_ROOT"
    [ -L "$BR_ROOT" ] && printf ' → %s' "$(readlink -f "$BR_ROOT")"
    printf '\n'
    printf 'BR_OVERLAY        %s\n' "$BR_OVERLAY"
    printf 'BR_OVERLAY_APP    %s\n' "$BR_OVERLAY_APP"
    printf 'BR_DEFCONFIG      %s\n' "$BR_DEFCONFIG"
    printf 'BR_TOOLCHAINFILE  %s\n' "$BR_TOOLCHAINFILE"
    printf 'BR_SDIMG          %s\n' "$BR_SDIMG"
    printf 'KERNEL_DIR        %s\n' "$KERNEL_DIR"
    printf 'RPI_ZERO_DIR      %s\n' "$RPI_ZERO_DIR"
    printf '%s\n' "───────────────────────────────────────────────────────"
    if [ -f "$BR_ROOT/.config" ]; then
        grep -E '^BR2_(DEFCONFIG|GCC_VERSION|LINUX_KERNEL_DEFCONFIG|TARGET_GENERIC_HOSTNAME)=' "$BR_ROOT/.config" \
            | sed 's/^/  /'
    fi
}

cmd_check() {
    log "Vérification des pré-requis…"
    require_dir "$VA_REPO"
    require_dir "$BR_ROOT"
    require_dir "$BR_OVERLAY"
    require_file "$BR_ROOT/configs/$BR_DEFCONFIG"
    require_dir "$BR_ROOT/board/vanassistant-rpi0w"
    require_dir "$KERNEL_DIR"
    require_dir "$RPI_ZERO_DIR"
    [ -f "$BR_TOOLCHAINFILE" ] || warn "toolchainfile absent — lance d'abord 'make $BR_DEFCONFIG && make toolchain' dans $BR_ROOT"
    command -v arm-linux-gnueabihf-gcc >/dev/null 2>&1 \
        || warn "arm-linux-gnueabihf-gcc absent du PATH (nécessaire pour build-kernel)"
    command -v genimage >/dev/null 2>&1 || true   # fourni par Buildroot host
    ok "Pré-requis OK"
}

cmd_build_app() {
    cmd_check
    log "Configure build.buildroot si nécessaire…"
    if [ ! -d "$VA_BUILD_DIR" ] || [ ! -f "$VA_BUILD_DIR/CMakeCache.txt" ]; then
        require_file "$BR_TOOLCHAINFILE"
        mkdir -p "$VA_BUILD_DIR"
        ( cd "$VA_BUILD_DIR" && cmake .. \
            -DPLATFORM=BUILDROOT \
            -DCMAKE_TOOLCHAIN_FILE="$BR_TOOLCHAINFILE" )
    fi
    log "Compilation vanassistant (ARM cross-compile)…"
    ( cd "$VA_BUILD_DIR" && make -j"$(nproc_safe)" )
    require_file "$VA_BUILD_DIR/$VA_BIN"
    log "Copie du binaire dans l'overlay ($BR_OVERLAY_APP/vanassistant)…"
    mkdir -p "$BR_OVERLAY_APP"
    install -m 755 "$VA_BUILD_DIR/$VA_BIN" "$BR_OVERLAY_APP/vanassistant"
    install -m 755 "$VA_BUILD_DIR/$VA_BIN" "$BR_OVERLAY_APP/$VA_BIN"
    ok "App déployée dans l'overlay"
}

cmd_build_kernel() {
    cmd_check
    log "Compilation du kernel Pi Zero W custom (via $RPI_ZERO_DIR/Makefile.kernel)…"
    ( cd "$RPI_ZERO_DIR" && make -f Makefile.kernel kernel -j"$(nproc_safe)" )
    ok "Kernel + DTB + modules installés"
}

cmd_update_logo() {
    local src="${1:-}"
    [ -n "$src" ]        || err "usage: update-logo <chemin.jpg>"
    [ -f "$src" ]        || err "fichier introuvable : $src"
    command -v jpegtopnm >/dev/null 2>&1 || err "jpegtopnm absent (installe netpbm)"
    command -v ppmquant  >/dev/null 2>&1 || err "ppmquant absent (installe netpbm)"
    command -v pnmnoraw  >/dev/null 2>&1 || err "pnmnoraw absent (installe netpbm)"

    local dest_ppm="$KERNEL_DIR/drivers/video/logo/logo_linux_clut224.ppm"
    local tmp_dir; tmp_dir="$(mktemp -d)"
    trap 'rm -rf "$tmp_dir"' EXIT

    log "Conversion $src → logo_linux_clut224.ppm (224 couleurs)…"
    jpegtopnm "$src" > "$tmp_dir/logo.ppm"
    ppmquant 224     "$tmp_dir/logo.ppm" > "$tmp_dir/logo224.tmp"
    pnmnoraw         "$tmp_dir/logo224.tmp" > "$tmp_dir/logo_linux_clut224.ppm"

    log "Installation dans $dest_ppm"
    cp -v "$tmp_dir/logo_linux_clut224.ppm" "$dest_ppm"
    warn "Rebuild kernel obligatoire pour propager le logo → 'va_buildroot.sh build-kernel'"
    ok "Logo mis à jour"
}

cmd_build_image() {
    cmd_check
    require_file "$BR_ROOT/.config" || {
        log "Génération .config depuis $BR_DEFCONFIG"
        ( cd "$BR_ROOT" && make "$BR_DEFCONFIG" )
    }
    log "Build Buildroot (make -j$(nproc_safe)) — peut prendre plusieurs minutes…"
    ( cd "$BR_ROOT" && make -j"$(nproc_safe)" )
    require_file "$BR_SDIMG"
    ok "Image générée : $BR_SDIMG ($(du -h "$BR_SDIMG" | cut -f1))"
}

cmd_full() {
    log "Pipeline complet : kernel → app → image"
    cmd_build_kernel
    cmd_build_app
    cmd_build_image
    ok "Distribution complète prête : $BR_SDIMG"
}

cmd_quick() {
    log "Pipeline rapide : app → image (kernel inchangé)"
    cmd_build_app
    cmd_build_image
    ok "Image mise à jour : $BR_SDIMG"
}

cmd_flash() {
    local device="${1:-}"
    [ -n "$device" ] || err "usage: flash /dev/sdX"
    [ -b "$device" ] || err "$device n'est pas un périphérique bloc"
    require_file "$BR_SDIMG"

    warn "OPÉRATION DESTRUCTIVE : $device sera intégralement écrasé"
    printf '  Image  : %s (%s)\n' "$BR_SDIMG" "$(du -h "$BR_SDIMG" | cut -f1)" >&2
    printf '  Device : %s\n' "$device" >&2
    lsblk -o NAME,SIZE,MODEL,MOUNTPOINT "$device" 2>/dev/null | sed 's/^/    /' >&2
    printf 'Confirmer avec "OUI" en majuscules : ' >&2
    local ans; read -r ans
    [ "$ans" = "OUI" ] || err "annulé"

    log "sudo dd if=$BR_SDIMG of=$device bs=4M status=progress conv=fsync"
    sudo dd if="$BR_SDIMG" of="$device" bs=4M status=progress conv=fsync
    sync
    ok "SD card flashée. Retirer proprement puis insérer dans le Pi."
}

# ────────────────────────────────────────────────────────────────────
# Dispatcher
# ────────────────────────────────────────────────────────────────────
show_help() {
    # Affiche l'en-tête (commentaires contigus depuis la ligne 2 jusqu'à la
    # première ligne non-commentaire — évite de sortir les séparateurs de
    # sections plus bas dans le fichier).
    awk 'NR==1 { next } /^#/ { sub(/^# ?/, ""); print; next } { exit }' "$0"
}

sub="${1:-}"
[ -n "$sub" ] || { show_help; exit 1; }
shift
case "$sub" in
    check)        cmd_check "$@" ;;
    info)         cmd_info "$@" ;;
    build-app)    cmd_build_app "$@" ;;
    build-kernel) cmd_build_kernel "$@" ;;
    update-logo)  cmd_update_logo "$@" ;;
    build-image)  cmd_build_image "$@" ;;
    full)         cmd_full "$@" ;;
    quick)        cmd_quick "$@" ;;
    flash)        cmd_flash "$@" ;;
    *)            err "sous-commande inconnue : $sub (voir l'aide en tête de fichier)" ;;
esac
