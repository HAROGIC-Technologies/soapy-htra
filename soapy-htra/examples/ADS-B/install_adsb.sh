#!/usr/bin/env bash
set -Eeuo pipefail

REPO_URL="https://github.com/mhostetter/gr-adsb.git"
GR_ADSB_REF="${GR_ADSB_REF:-maint-3.10}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GR_ADSB_DIR="$SCRIPT_DIR/gr-adsb"
BUILD_DIR="$GR_ADSB_DIR/build"
WEB_DIR="$GR_ADSB_DIR/web"
VENV_DIR="$WEB_DIR/venv"
BUILD_JOBS="${BUILD_JOBS:-$(nproc)}"

log(){ printf '\n[gr-adsb] %s\n' "$*"; }
die(){ printf '\n[gr-adsb] ERROR: %s\n' "$*" >&2; exit 1; }
trap 'die "Installation stopped at line $LINENO."' ERR

[[ "$EUID" -ne 0 ]] || die "Run this script as a normal user, not with sudo."

log "Installing system dependencies"
sudo apt-get update
sudo apt-get install -y build-essential cmake git gnuradio gnuradio-dev \
    libzmq3-dev pkg-config python3-dev python3-pip python3-venv

log "GNU Radio version: $(gnuradio-config-info --version)"

if [[ -d "$GR_ADSB_DIR/.git" ]]; then
    log "Using existing repository: $GR_ADSB_DIR"
    git -C "$GR_ADSB_DIR" fetch --all --tags || true
else
    [[ ! -e "$GR_ADSB_DIR" ]] || die "$GR_ADSB_DIR already exists but is not a Git repository."
    log "Cloning gr-adsb"
    git clone --branch "$GR_ADSB_REF" --single-branch "$REPO_URL" "$GR_ADSB_DIR"
fi

git -C "$GR_ADSB_DIR" checkout "$GR_ADSB_REF"
git -C "$GR_ADSB_DIR" pull --ff-only origin "$GR_ADSB_REF" || true

log "Building gr-adsb"
rm -rf "$BUILD_DIR"
cmake -S "$GR_ADSB_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --parallel "$BUILD_JOBS"

log "Installing gr-adsb"
sudo cmake --install "$BUILD_DIR"
sudo ldconfig

log "Verifying GNU Radio ADS-B module"
python3 - <<'PY'
from gnuradio import adsb
print("Imported gnuradio.adsb from:", adsb.__file__)
PY

[[ -f "$WEB_DIR/webserver.py" ]] || die "webserver.py was not found."

log "Creating clean virtual environment"
rm -rf "$VENV_DIR"
python3 -m venv --system-site-packages "$VENV_DIR"
PYTHON="$VENV_DIR/bin/python"

log "Installing compatible Web map dependencies"
"$PYTHON" -m pip install --upgrade pip setuptools wheel
"$PYTHON" -m pip install \
    "Flask==1.1.4" \
    "Werkzeug==1.0.1" \
    "Flask-SocketIO==4.3.2" \
    "python-socketio==4.6.1" \
    "python-engineio==3.14.2" \
    "Jinja2==2.11.3" \
    "MarkupSafe==2.0.1" \
    "itsdangerous==1.1.0" \
    "click==7.1.2" \
    "gevent" "pyzmq" "six"

log "Verifying Web map environment"
"$PYTHON" - <<'PY'
import flask, flask_socketio, gevent, pmt, socketio, zmq
print("Flask:", flask.__version__)
print("Flask-SocketIO:", flask_socketio.__version__)
print("python-socketio:", socketio.__version__)
print("PMT:", pmt.__file__)
print("pyzmq:", zmq.__version__)
PY

log "Configuring Web server Socket.IO settings"

python3 - <<PY
from pathlib import Path

webserver = Path("$WEB_DIR/webserver.py")

if not webserver.exists():
    raise FileNotFoundError(webserver)

text = webserver.read_text()

old = "socketio = SocketIO(app)"
new = "socketio = SocketIO(app, cors_allowed_origins='*')"

if old in text:
    text = text.replace(old, new)
    webserver.write_text(text)
    print("Updated Socket.IO CORS configuration.")
elif new in text:
    print("Socket.IO CORS configuration already exists.")
else:
    print("Warning: SocketIO configuration was not found.")
PY

cat <<EOF

Installation completed successfully.


Then open:
  http://127.0.0.1:5000
EOF