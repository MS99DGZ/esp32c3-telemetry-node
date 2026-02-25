import os
import re

from SCons.Script import Import

Import("env")

# Path to wifi_config.h (relative to project root)
config_path = os.path.join("include", "wifi_config.h")

ota_password = None
ota_hostname = None

if os.path.exists(config_path):
    with open(config_path, "r", encoding="utf-8") as f:
        text = f.read()

        # Look for: #define OTA_PASSWORD "something"
        m_pw = re.search(r'#define\s+OTA_PASSWORD\s+"([^"]+)"', text)
        if m_pw:
            ota_password = m_pw.group(1)

        # Look for: #define OTA_HOSTNAME "something"
        m_host = re.search(r'#define\s+OTA_HOSTNAME\s+"([^"]+)"', text)
        if m_host:
            ota_hostname = m_host.group(1)

# === Configure OTA password ===
if ota_password:
    print("[OTA] Using password from wifi_config.h")
    flags = env.GetProjectOption("upload_flags")
    if not flags:
        flags = []
    elif isinstance(flags, str):
        flags = [flags]

    flags.append("--auth=" + ota_password)
    env.Replace(UPLOAD_FLAGS=flags)
else:
    print("[OTA] No OTA_PASSWORD found in wifi_config.h (using default upload flags)")

# === Configure upload port from OTA_HOSTNAME ===
if ota_hostname:
    upload_port = ota_hostname + ".local"
    print(f"[OTA] Using upload port from OTA_HOSTNAME: {upload_port}")
    env.Replace(UPLOAD_PORT=upload_port)
else:
    print("[OTA] No OTA_HOSTNAME found in wifi_config.h (UPLOAD_PORT unchanged)")