# This is a code file for wifi client on the end of the laptop
# This is the primary communication channel but if not working then falling
# back to bluetooth client.
# ==================== NOTE TO NON-AUTHOR ===========================
# Plz explian the changes and also the reason behind it both arch way and software

import socket
from multiprocessing.connection import Client

# =============================
# ESP32 CONFIG
# =============================

ESP32_IP = "192.168.1.100"
PORT = 3333

# =============================
# CONNECT FUNCTION
# =============================


def connect_wifi():
    global Client
    global wifi_connected

    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.settimeout(3)

        client.connect((ESP32_IP, PORT))

        wifi_connected = True

        print("[INFO] Connected to ESP32")
    except Exception as e:
        wifi_connected = False

        print(f"[WARNING] WiFi not connected: {e}")


# ===================================
# SEND COMMAND
# ===================================


def send_command(command):

    try:
        client.send(command.encode())

        print(f"[SENT] {command}")

    except Exception as e:
        print(f"[ERROR] send failed: {e}")


# ==================================
# CLOSE CONNECTION
# ==================================


def close_connection():

    client.close()

    print("[INFO] connection closed")
