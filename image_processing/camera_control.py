# The idea behind this file is that it will do the main
# work of image processing for the project
# And then sent the details over to the esp32 using bluetooth
# ======================= NOTE TO NON-AUTHOR ========================
# If you like to make a pull request then plz do by explaining everything about your changes
#   - The reason behind
#   - The architecture it will support
#   - Tools compatiblity

import cv2
from ultralytics import YOLO

from wifi_client import close_connection, connect_wifi, send_command

# ==========================================
# CONFIGURATION
# ==========================================

MODEL_PATH = "yolov8n.pt"

FRAME_WIDTH = 640
FRAME_HEIGHT = 480
FPS = 30

LEFT_RIGHT_OFFSET = 100
TOP_BOTTOM_OFFSET = 80

BOX_COLOR = (255, 255, 255)
CENTER_COLOR = (0, 0, 255)

LEFT_RIGHT_LINE_COLOR = (0, 255, 0)
TOP_BOTTOM_LINE_COLOR = (255, 0, 0)

TEXT_COLOR = (0, 255, 0)

# ==========================================
# LOAD YOLO MODEL
# ==========================================

model = YOLO(MODEL_PATH)

# ==========================================
# CAMERA INITIALIZATION
# ==========================================

cap = cv2.VideoCapture(0)

cap.set(cv2.CAP_PROP_FRAME_WIDTH, FRAME_WIDTH)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)
cap.set(cv2.CAP_PROP_FPS, FPS)

if not cap.isOpened():
    print("[ERROR] Failed to open camera")

    exit()

# ==========================================
# WIFI CONNECTION
# ==========================================

connect_wifi()

# ==========================================
# PREVIOUS COMMAND TRACKER
# ==========================================

previous_command = ""

# ==========================================
# COMMAND GENERATION
# ==========================================


def get_command(cx, cy, left_boundary, right_boundary, top_boundary, bottom_boundary):

    if cx < left_boundary:
        return "LEFT"

    elif cx > right_boundary:
        return "RIGHT"

    elif cy < top_boundary:
        return "FORWARD"

    elif cy > bottom_boundary:
        return "REVERSE"

    else:
        return "STOP"


# ==========================================
# DRAW CONTROL ZONES
# ==========================================


def draw_control_lines(
    frame,
    left_boundary,
    right_boundary,
    top_boundary,
    bottom_boundary,
    frame_width,
    frame_height,
):

    # LEFT LINE
    cv2.line(
        frame,
        (left_boundary, 0),
        (left_boundary, frame_height),
        LEFT_RIGHT_LINE_COLOR,
        2,
    )

    # RIGHT LINE
    cv2.line(
        frame,
        (right_boundary, 0),
        (right_boundary, frame_height),
        LEFT_RIGHT_LINE_COLOR,
        2,
    )

    # TOP LINE
    cv2.line(
        frame, (0, top_boundary), (frame_width, top_boundary), TOP_BOTTOM_LINE_COLOR, 2
    )

    # BOTTOM LINE
    cv2.line(
        frame,
        (0, bottom_boundary),
        (frame_width, bottom_boundary),
        TOP_BOTTOM_LINE_COLOR,
        2,
    )


# ==========================================
# MAIN LOOP
# ==========================================

while True:
    command = "STOP"

    person_detected = False

    # ======================================
    # READ CAMERA FRAME
    # ======================================

    ret, frame = cap.read()

    if not ret:
        print("[ERROR] Failed to capture frame")

        break

    # ======================================
    # MIRROR FRAME
    # ======================================

    frame = cv2.flip(frame, 1)

    frame_height, frame_width, _ = frame.shape

    # ======================================
    # FRAME CENTER
    # ======================================

    center_x = frame_width // 2
    center_y = frame_height // 2

    # ======================================
    # CONTROL BOUNDARIES
    # ======================================

    left_boundary = center_x - LEFT_RIGHT_OFFSET
    right_boundary = center_x + LEFT_RIGHT_OFFSET

    top_boundary = center_y - TOP_BOTTOM_OFFSET
    bottom_boundary = center_y + TOP_BOTTOM_OFFSET

    # ======================================
    # DRAW CONTROL LINES
    # ======================================

    draw_control_lines(
        frame,
        left_boundary,
        right_boundary,
        top_boundary,
        bottom_boundary,
        frame_width,
        frame_height,
    )

    # ======================================
    # YOLO DETECTION
    # ======================================

    results = model(frame)

    # ======================================
    # PROCESS DETECTIONS
    # ======================================

    for result in results:
        boxes = result.boxes

        if boxes is None:
            continue

        for box in boxes:
            cls = int(box.cls[0])

            # COCO CLASS 0 = PERSON
            if cls != 0:
                continue

            # ==================================
            # BOUNDING BOX COORDINATES
            # ==================================

            x1, y1, x2, y2 = map(int, box.xyxy[0])

            # ==================================
            # SAFETY LIMITS
            # ==================================

            x1 = max(0, x1)
            y1 = max(0, y1)

            x2 = min(frame_width, x2)
            y2 = min(frame_height, y2)

            # ==================================
            # DRAW DETECTION BOX
            # ==================================

            cv2.rectangle(frame, (x1, y1), (x2, y2), BOX_COLOR, 2)

            # ==================================
            # CENTER POINT
            # ==================================

            cx = (x1 + x2) // 2
            cy = (y1 + y2) // 2

            cv2.circle(frame, (cx, cy), 8, CENTER_COLOR, -1)

            # ==================================
            # COMMAND GENERATION
            # ==================================

            command = get_command(
                cx, cy, left_boundary, right_boundary, top_boundary, bottom_boundary
            )

            # ==================================
            # SEND ONLY IF CHANGED
            # ==================================

            if command != previous_command:
                send_command(command)

                previous_command = command

                print(f"[COMMAND] {command}")

            # ==================================
            # TRACK ONLY FIRST PERSON
            # ==================================

            person_detected = True

            break

        if person_detected:
            break

    # ======================================
    # DISPLAY COMMAND
    # ======================================

    cv2.putText(
        frame,
        f"Command: {command}",
        (20, 40),
        cv2.FONT_HERSHEY_SIMPLEX,
        1,
        TEXT_COLOR,
        2,
    )

    # ======================================
    # DISPLAY WINDOW
    # ======================================

    cv2.imshow("YOLO Robot Control", frame)

    # ======================================
    # EXIT KEY
    # ======================================

    key = cv2.waitKey(1)

    if key == 27:
        break

# ==========================================
# CLEANUP
# ==========================================

close_connection()

cap.release()

cv2.destroyAllWindows()
