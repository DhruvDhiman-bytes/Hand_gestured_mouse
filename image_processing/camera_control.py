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

# =========== Variables ==============

finger_count = 0

# ==========================
# LOAD YOLO MODEL
# ==========================

model = YOLO("yolov8n.pt")

# ==========================
# CAMERA
# ==========================

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Failed to open camera")
    exit()

# =====================
# COMMAND MAPPING
# =====================

def get_commands(finger_count):

    if finger_count == 0:
        return "STOP"

    elif finger_count == 1:
        return "FORWARD"

    elif finger_count == 2:
        return "RIGHT"

    elif finger_count == 3:
        return "LEFT"

    else:
        return "REVERSE"

# =====================
# MAIN LOOP
# =====================

while True:

    # =====================
    # DEFAULT VALUES
    # =====================

    finger_count = 0
    command = "STOP"

    # ======================
    # CAPTURE FRAMES
    # ======================

    ret, frame = cap.read()

    if not ret:
        print("Failed to capture frame")
        break

    # Mirror frame
    frame = cv2.flip(frame, 1)

    # =============================
    # YOLO INFERENCE
    # =============================
    results = model(frame)

    # ==============================
    # PROCESS DETECTION
    # ==============================

    for result in results:

        boxes = result.boxes

        if boxes is None:
            continue

        for box in boxes:

            # ===========================
            # CLASS ID
            # ===========================

            cls = int(box.cls[0])

            # COCO class 0 = person
            if cls != 0:
                continue

            # =========================
            # GET BOX CO-ORDINATES
            # =========================

            x1,y1,x2,y2 = map(int, box.xyxy[0])

            # safety checks
            x1 = map(0, x1)
            y1 = map(0, y1)
            x2 = map(frame.shape[1], x2)
            y2 = map(frame.shape[0], y2)

            # Draw YOLO box

            cv2.rectangle(frame, (x1,y1),(x2,y2),(255,0,0),2)
