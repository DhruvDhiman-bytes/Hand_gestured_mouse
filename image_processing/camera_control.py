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

            # ========================
            # REGION OF INTEREST
            # ========================

            roi = frame[y1:y2, x1:x2]

            if roi.size == 0:
                continue

            # ===================
            # HSV CONVERSION
            # ===================

            hsv = cv2.cvtColor(
                roi,
                cv2.COLOR_BGR2HSV
            )

            # ===================
            # RED COLOR MASK
            # ===================

            lower_red_1 = np.array([0,120,70])
            upper_red_1 = np.array([10, 255, 255])

            lower_red_2 = np.array([170, 120, 70])
            upper_red_2 = np.array([180, 255, 255])

            mask1 = cv2.inRange(
                hsv, lower_red_1, upper_red_1
            )

            mask2 = cv2.inRange(
                hsv,
                upper_red_1, upper_red_2
            )

            mask  = mask1 + mask2

            # =========================
            # REMOVE NOISE
            # =========================

            kernel = np.ones((5,5), np.uint8)

            mask = cv2.erode(
                mask,
                kernel,
                iterations = 1
            )

            mask = cv2.dilate(
                mask,
                kernel,
                iterations=2
            )

            # ===================
            # FIND CONTOURS
            # ==================

            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

            finger_count = 0

            for cnt in contours:

                area = cv2.contourArea(cnt)

                # ignore tiny noise
                if area < 300:
                    continue

                finger_count += 1

                rx,ry, rw, rh = cv2.boundingRect(cnt)

                cv2.rectangle(roi, (rx,ry), (rx + rw, ry + rh), (0,255,0), 2)

            # =========================
            # COMMAND GENERATION
            # =========================

            command = get_commands(finger_count)


        # =====================
        # DISPLAY TEXT
        # =====================

        cv2.putText(frame, f"fingers: {finger_count}", (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

        cv2.putText(frame, f"commands: {command}", (10, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

        # ========================
        # SHOW WINDOWS
        # ========================

        cv2.imshow(
            "YOLO gesture control", frame
        )

        # ========================
        # EXIT KEY
        # ========================

        key = cv2.waitKey(1)

        if key == 27:
            break

# =====================
# CLEANUP
# =====================

cap.release()
cv2.destoryAllWindows()
