#!/bin/bash
# Setup HTTPS for WebGPU development server.
# Generates a self-signed certificate for your local IP.

set -e

# Detect local IP (cross-platform)
if [[ "$(uname)" == "Darwin" ]]; then
    LOCAL_IP=$(ipconfig getifaddr en0 2>/dev/null || echo "localhost")
else
    LOCAL_IP=$(hostname -I 2>/dev/null | awk '{print $1}' || echo "localhost")
fi

echo "Local IP detected: $LOCAL_IP"
echo "Generating SSL certificate for $LOCAL_IP..."

# Generate self-signed certificate in the Scripts directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem \
    -days 365 -nodes \
    -subj "/CN=$LOCAL_IP" \
    -addext "subjectAltName=DNS:localhost,IP:127.0.0.1,IP:$LOCAL_IP" \
    2>/dev/null

echo "Certificate generated!"
echo ""
echo "To start the HTTPS server, run from project root:"
echo "  python3 Scripts/https_server.py build-web/Examples/PreVEngineExample/"
echo ""
echo "Then access in browser:"
echo "  https://localhost:8443/PreVEngineExample.html"
echo "  https://$LOCAL_IP:8443/PreVEngineExample.html  (other devices)"
echo ""
echo "Note: You'll need to accept the certificate warning in your browser."
