# The idea behind this file is that it will do the main
# work of image processing for the project
# And then sent the details over to the esp32 using bluetooth
# ======================= NOTE TO NON-AUTHOR ========================
# If you like to make a pull request then plz do by explaining everything about your changes
#   - The reason behind
#   - The architecture it will support
#   - Tools compatiblity

import cv2
import numpy as np
from ultralytics import YOLO

# ==========================
# LOAD YOLO MODEL
# ==========================

model = YOLO("yolov8n.pt")

# ==========================
# CAMERA
# ==========================

cap = cv2.VideoCapture(0)

# =========================
# COMMAND FUNCTION
# =========================

def get_commands(fingers):

    if fingers == 0:
        return "STOP"

    elif fingers == 1:
        return "FORWARD"

    elif fingers == 2:
        return "RIGHT"

    elif fingers == 0:
        return "LEFT"

    else:
        return "REVERSE"
