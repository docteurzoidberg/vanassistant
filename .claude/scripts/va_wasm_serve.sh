#!/usr/bin/env bash
# va_wasm_serve.sh — sert le build WASM de VanAssistant pour test navigateur,
#                    y compris depuis un autre appareil du réseau local.
#
# Usage:
#   va_wasm_serve.sh <sous-commande> [args…]
#
# Sous-commandes :
#   start [port]   — démarre le serveur en arrière-plan (port par défaut : 8000)
#   stop           — arrête le serveur démarré par ce script
#   status         — serveur actif ? sur quel port, quel répertoire, quels artefacts
#   url [port]     — réaffiche les URLs d'accès (local + LAN)
#   lan [port]     — instructions de redirection Windows pour l'accès LAN (WSL2)
#
# Le serveur est wasm/wasmserve.py : il envoie les en-têtes COOP/COEP
# (Cross-Origin-Opener-Policy / Cross-Origin-Embedder-Policy) sans lesquels
# SharedArrayBuffer est bloqué — et le build est lié en -pthread, donc la page
# ne démarre pas avec un simple « python3 -m http.server ».

set -uo pipefail

VA_REPO="${VA_REPO:-$HOME/dev/vanassistant}"
SERVE_PY="$VA_REPO/wasm/wasmserve.py"
PIDFILE="${XDG_RUNTIME_DIR:-/tmp}/va_wasmserve.pid"
PORTFILE="${XDG_RUNTIME_DIR:-/tmp}/va_wasmserve.port"
DEFAULT_PORT=8000

log()  { printf '\033[1;36m▸\033[0m %s\n' "$*" >&2; }
warn() { printf '\033[1;33m⚠\033[0m %s\n' "$*" >&2; }
err()  { printf '\033[1;31m✖\033[0m %s\n' "$*" >&2; }
ok()   { printf '\033[1;32m✓\033[0m %s\n' "$*" >&2; }

# ────────────────────────────────────────────────────────────────────
# Utilitaires
# ────────────────────────────────────────────────────────────────────

# Répertoire de build à servir : on prend le premier qui contient
# vanassistant.html. build.wasm/ (build manuel) prime sur build.check.wasm/
# (smoke test de va_build_all.sh), qui est jetable.
find_build_dir() {
    local d
    for d in "$VA_REPO/build.wasm" "$VA_REPO/build.check.wasm"; do
        [ -f "$d/vanassistant.html" ] && { echo "$d"; return 0; }
    done
    return 1
}

wsl_ip() { ip -4 -o addr show eth0 2>/dev/null | awk '{print $4}' | cut -d/ -f1; }

is_wsl() { grep -qi microsoft /proc/sys/kernel/osrelease 2>/dev/null; }

server_pid() {
    [ -f "$PIDFILE" ] || return 1
    local pid; pid=$(cat "$PIDFILE" 2>/dev/null)
    [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null && { echo "$pid"; return 0; }
    return 1
}

current_port() { cat "$PORTFILE" 2>/dev/null || echo "$DEFAULT_PORT"; }

print_urls() {
    local port="$1" ip
    ip=$(wsl_ip)
    printf '\n'
    printf '  Local (ce PC) : \033[1mhttp://localhost:%s/vanassistant.html\033[0m\n' "$port"
    if [ -n "$ip" ]; then
        printf '  Depuis WSL    : http://%s:%s/vanassistant.html\n' "$ip" "$port"
    fi
    if is_wsl; then
        printf '\n'
        printf '  \033[1;33mLAN\033[0m : WSL2 tourne en reseau NAT, cette IP est interne et non\n'
        printf '        joignable depuis le reseau local. Voir : %s lan %s\n' "$0" "$port"
    fi
    printf '\n'
}

# ────────────────────────────────────────────────────────────────────
# Sous-commandes
# ────────────────────────────────────────────────────────────────────

cmd_start() {
    local port="${1:-$DEFAULT_PORT}"

    local pid
    if pid=$(server_pid); then
        warn "serveur déjà actif (pid $pid, port $(current_port)) — '$0 stop' d'abord"
        return 1
    fi

    [ -f "$SERVE_PY" ] || { err "serveur introuvable : $SERVE_PY"; return 1; }

    local build_dir
    if ! build_dir=$(find_build_dir); then
        err "aucun vanassistant.html trouvé dans build.wasm/ ni build.check.wasm/"
        err "builder d'abord : .claude/scripts/va_build_all.sh build wasm"
        return 1
    fi

    if ss -tln 2>/dev/null | grep -qE "[:.]$port\b"; then
        err "port $port déjà occupé — choisir un autre port : $0 start 8001"
        return 1
    fi

    # nohup : le serveur survit à la fin du shell appelant (SIGHUP ignoré).
    # Deux pièges à ne pas réintroduire, tous deux menant à un $! qui n'est pas
    # le pid de python — donc à un « stop » qui laisse le serveur sur le port :
    #   - setsid forke : $! serait le pid de setsid ;
    #   - « cd … && nohup python3 … & » backgrounde la liste ENTIÈRE, pas le
    #     seul python : $! serait le pid du sous-shell. D'où le cd séparé.
    (
        cd "$build_dir" || exit 1
        nohup python3 "$SERVE_PY" "$port" >"$build_dir/wasmserve.log" 2>&1 &
        echo $! >"$PIDFILE"
    )
    echo "$port" >"$PORTFILE"

    sleep 1
    if ! pid=$(server_pid); then
        err "le serveur n'a pas démarré — voir $build_dir/wasmserve.log"
        rm -f "$PIDFILE" "$PORTFILE"
        return 1
    fi

    ok "serveur démarré (pid $pid) — racine : $build_dir"
    log "artefacts : $(date -r "$build_dir/vanassistant.html" '+%Y-%m-%d %H:%M')"
    print_urls "$port"
    log "arrêt : $0 stop"
}

cmd_stop() {
    local pid
    if ! pid=$(server_pid); then
        warn "aucun serveur actif (démarré par ce script)"
        rm -f "$PIDFILE" "$PORTFILE"
        return 0
    fi
    kill "$pid" 2>/dev/null
    sleep 0.3
    kill -0 "$pid" 2>/dev/null && kill -9 "$pid" 2>/dev/null
    rm -f "$PIDFILE" "$PORTFILE"
    ok "serveur arrêté (pid $pid)"
}

cmd_status() {
    local build_dir pid port
    port=$(current_port)
    if pid=$(server_pid); then
        ok "serveur ACTIF — pid $pid, port $port"
        ss -tlnp 2>/dev/null | grep -E "[:.]$port\b" | sed 's/^/    /'
        print_urls "$port"
    else
        warn "serveur INACTIF"
    fi

    if build_dir=$(find_build_dir); then
        log "build servi : $build_dir"
        ls -la "$build_dir"/vanassistant.{html,js,wasm} 2>/dev/null | sed 's/^/    /'
    else
        warn "aucun build WASM disponible (build.wasm/ ni build.check.wasm/)"
    fi
}

cmd_url() { print_urls "${1:-$(current_port)}"; }

cmd_lan() {
    local port="${1:-$(current_port)}"
    local ip; ip=$(wsl_ip)

    if ! is_wsl; then
        log "pas sous WSL — le serveur écoute déjà sur 0.0.0.0:$port,"
        log "accessible directement depuis le LAN (vérifier le pare-feu)."
        return 0
    fi

    cat <<EOF

WSL2 est en réseau NAT : l'IP $ip appartient à un sous-réseau interne, non
routé depuis le LAN. Deux options pour tester depuis un téléphone / autre PC.

── Option A — redirection de port (ponctuel) ────────────────────────
Dans un PowerShell **administrateur** côté Windows :

  netsh interface portproxy add v4tov4 listenport=$port listenaddress=0.0.0.0 connectport=$port connectaddress=$ip
  netsh advfirewall firewall add rule name="WSL vanassistant wasm $port" dir=in action=allow protocol=TCP localport=$port

Puis, depuis l'autre appareil : http://<IP-LAN-de-ton-PC-Windows>:$port/vanassistant.html
(IP Windows : \`ipconfig\` côté Windows, carte Wi-Fi/Ethernet.)

Pour retirer :
  netsh interface portproxy delete v4tov4 listenport=$port listenaddress=0.0.0.0
  netsh advfirewall firewall delete rule name="WSL vanassistant wasm $port"

/!\\ En mode NAT, l'IP WSL ($ip) change à chaque redémarrage de WSL —
    la redirection est à refaire.

── Option B — réseau miroir (durable, recommandé) ───────────────────
Dans %USERPROFILE%\\.wslconfig côté Windows :

  [wsl2]
  networkingMode=mirrored

Puis \`wsl --shutdown\` et relancer. WSL partage alors l'IP de l'hôte
Windows : le serveur est directement joignable sur
http://<IP-LAN-Windows>:$port/vanassistant.html, sans portproxy et sans
rien à refaire aux reboots. (Il reste à autoriser le port dans le pare-feu.)

EOF
}

show_help() { awk 'NR==1 { next } /^#/ { sub(/^# ?/, ""); print; next } { exit }' "$0"; }

sub="${1:-}"; shift 2>/dev/null || true
[ -n "$sub" ] || { show_help; exit 1; }

case "$sub" in
    start)  cmd_start "$@" ;;
    stop)   cmd_stop "$@" ;;
    status) cmd_status "$@" ;;
    url)    cmd_url "$@" ;;
    lan)    cmd_lan "$@" ;;
    -h|--help|help) show_help ;;
    *)      err "sous-commande inconnue : $sub"; show_help; exit 1 ;;
esac
