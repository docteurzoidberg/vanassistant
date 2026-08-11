---
name: vanassistant-wasm-serve
description: Sert le build WASM de VanAssistant pour le tester dans un navigateur, en local ou depuis un autre appareil du réseau local (téléphone, tablette, autre PC). À utiliser quand l'utilisateur demande "lance le wasm", "sers le wasm", "lance le serveur web", "je veux tester dans le navigateur", "teste depuis mon réseau local", "teste depuis mon téléphone", "démarre le serveur de test", "arrête le serveur", ou veut voir VanAssistant tourner dans une page web.
---

# Skill — Serveur de test WASM VanAssistant

Cette skill démarre le serveur HTTP local qui sert le build emscripten de
VanAssistant, pour un test navigateur — sur la machine de dev ou depuis un
autre appareil du réseau.

**Toujours** passer par le wrapper `.claude/scripts/va_wasm_serve.sh`, jamais
par un `python3 -m http.server` ad-hoc : voir §« Pourquoi ce serveur » plus bas.

## Sous-commandes

```bash
.claude/scripts/va_wasm_serve.sh <sous-commande> [port]
```

| Sous-commande   | Rôle                                                                  |
| --------------- | --------------------------------------------------------------------- |
| `start [port]`  | Démarre le serveur en arrière-plan (défaut 8000) + affiche les URLs    |
| `stop`          | Arrête le serveur démarré par ce script                               |
| `status`        | Serveur actif ? port, répertoire servi, date des artefacts            |
| `url [port]`    | Réaffiche les URLs d'accès                                            |
| `lan [port]`    | Instructions de redirection Windows pour l'accès depuis le LAN (WSL2) |

Le serveur tourne détaché : il survit à la fin de la commande et reste actif
entre deux tours de conversation. Log : `<build_dir>/wasmserve.log`.

## Répertoire servi

Détection automatique, dans cet ordre — le premier qui contient
`vanassistant.html` gagne :

1. `build.wasm/` — build manuel de l'utilisateur (prioritaire)
2. `build.check.wasm/` — smoke test de la skill `vanassistant-build-all`

La racine HTTP est le répertoire de build lui-même, pas la racine du repo :
l'URL est donc `/vanassistant.html` et non `/build.wasm/vanassistant.html`.
Ça évite aussi d'exposer tout le dépôt sur le réseau local.

## Workflows

### W1 — Test navigateur local

```bash
.claude/scripts/va_wasm_serve.sh start
```

→ http://localhost:8000/vanassistant.html

Si aucun build n'existe, le script le dit et renvoie vers
`.claude/scripts/va_build_all.sh build wasm`. Enchaîner build puis start :

```bash
.claude/scripts/va_build_all.sh build wasm && .claude/scripts/va_wasm_serve.sh start
```

### W2 — Test depuis le réseau local (téléphone, autre PC)

Le serveur écoute déjà sur `0.0.0.0`, donc sur une machine Linux normale il
suffit de donner l'IP LAN. **Mais cette machine de dev est sous WSL2 en mode
NAT** : l'IP de `eth0` (172.25.x.x) est interne et n'est pas routée depuis le
réseau local. Il faut une étape côté Windows.

```bash
.claude/scripts/va_wasm_serve.sh start
.claude/scripts/va_wasm_serve.sh lan     # imprime les commandes Windows à lancer
```

`lan` propose deux options :

- **A — portproxy `netsh`** : ponctuel, à lancer dans un PowerShell
  administrateur. L'IP WSL change à chaque redémarrage de WSL, donc à refaire.
- **B — `networkingMode=mirrored`** dans `%USERPROFILE%\.wslconfig` : durable,
  recommandé. WSL partage l'IP de l'hôte Windows, plus de portproxy.

Ces commandes sont à exécuter **par l'utilisateur** côté Windows — l'interop
`powershell.exe` n'est pas dans le PATH de ce WSL, donc ne pas essayer de les
lancer depuis ici. Les présenter et laisser l'utilisateur faire.

### W3 — Après un rebuild

Pas besoin de redémarrer le serveur : il sert les fichiers du disque à chaque
requête. Rebuilder puis rafraîchir la page (avec vidage de cache) suffit.

`status` affiche la date des artefacts, utile pour confirmer que la page
servie correspond bien au dernier build.

## Pourquoi ce serveur et pas `python3 -m http.server`

`wasm/wasmserve.py` fait deux choses indispensables :

1. **En-têtes COOP/COEP** — `Cross-Origin-Opener-Policy: same-origin` et
   `Cross-Origin-Embedder-Policy: require-corp`. Sans eux, le navigateur
   désactive `SharedArrayBuffer` ; or le build est lié en `-pthread`
   (`-sPTHREAD_POOL_SIZE=5`), donc la page ne démarre pas du tout.
2. **Types MIME** — `application/wasm` pour le `.wasm`, sans quoi
   l'instanciation en streaming échoue.

Ouvrir `vanassistant.html` directement via `file://` ne marche pas non plus,
pour les mêmes raisons.

## Règles non-négociables

1. **Toujours** utiliser le wrapper, jamais un serveur HTTP ad-hoc.
2. **Ne pas modifier `wasm/wasmserve.py`** pour « faire marcher » l'accès LAN :
   il bind déjà sur `0.0.0.0`, le blocage est côté réseau WSL2/Windows.
3. **Ne pas lancer les commandes `netsh` / modifier `.wslconfig`** — ce sont des
   changements de configuration réseau de la machine de l'utilisateur, à sa
   main. Les afficher via `lan` et s'arrêter là.
4. **Un seul serveur à la fois** : `start` refuse de démarrer si un serveur du
   script tourne déjà. Utiliser un autre port (`start 8001`) si besoin de deux
   builds en parallèle.
5. **Ne pas conclure que « ça marche » depuis un code HTTP 200** — le 200 dit
   que le fichier est servi, pas que l'appli démarre. Le test réel (rendu,
   entrées, absence d'erreur console) est visuel et revient à l'utilisateur.

## Dépannage

| Symptôme                                | Cause probable / fix                                              |
| --------------------------------------- | ----------------------------------------------------------------- |
| `aucun vanassistant.html trouvé`        | Pas de build WASM → `va_build_all.sh build wasm`                   |
| `port 8000 déjà occupé`                 | Serveur résiduel → `stop`, ou `start 8001`                         |
| Page blanche, console : SharedArrayBuffer | Page ouverte en `file://` ou via un autre serveur sans COOP/COEP  |
| Page inaccessible depuis le téléphone   | WSL2 NAT → `lan`, appliquer l'option A ou B                        |
| `status` montre des artefacts trop vieux | Rebuild non fait → `va_build_all.sh build wasm`                    |

## Références

- Serveur : `wasm/wasmserve.py` — shell HTML : `wasm/vanassistant_shell.html`
- Flags de link WASM : `CMakeLists.txt` (bloc `if(${PLATFORM} STREQUAL "WASM")`)
- Skill sœur : `.claude/skills/vanassistant-build-all/SKILL.md` (produit le build)
