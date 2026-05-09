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

# ==========================================
# LOAD YOLO MODEL
# ==========================================

model = YOLO("yolov8n.pt")

# ==========================================
# CAMERA SETUP
# ==========================================

cap = cv2.VideoCapture(0)

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
cap.set(cv2.CAP_PROP_FPS, 30)

if not cap.isOpened():
    print("Failed to open camera")
    exit()

# ==========================================
# MAIN LOOP
# ==========================================

while True:

    # ======================================
    # DEFAULT COMMAND
    # ======================================

    command = "STOP"

    # ======================================
    # CAPTURE FRAME
    # ======================================

    ret, frame = cap.read()

    if not ret:
        print("Failed to capture frame")
        break

    # Mirror effect
    frame = cv2.flip(frame, 1)

    frame_height, frame_width, _ = frame.shape

    # ======================================
    # FRAME CENTER
    # ======================================

    center_x = frame_width // 2
    center_y = frame_height // 2

    # ======================================
    # CONTROL ZONES
    # ======================================

    left_boundary = center_x - 100
    right_boundary = center_x + 100

    top_boundary = center_y - 80
    bottom_boundary = center_y + 80

    # ======================================
    # DRAW GUIDE LINES
    # ======================================

    # Vertical lines
    cv2.line(
        frame,
        (left_boundary, 0),
        (left_boundary, frame_height),
        (0, 255, 0),
        2
    )

    cv2.line(
        frame,
        (right_boundary, 0),
        (right_boundary, frame_height),
        (0, 255, 0),
        2
    )

    # Horizontal lines
    cv2.line(
        frame,
        (0, top_boundary),
        (frame_width, top_boundary),
        (255, 0, 0),
        2
    )

    cv2.line(
        frame,
        (0, bottom_boundary),
        (frame_width, bottom_boundary),
        (255, 0, 0),
        2
    )

    # ======================================
    # YOLO INFERENCE
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

            # COCO class 0 = person
            if cls != 0:
                continue

            # ==================================
            # GET COORDINATES
            # ==================================

            x1, y1, x2, y2 = map(
                int,
                box.xyxy[0]
            )

            # Safety checks
            x1 = max(0, x1)
            y1 = max(0, y1)

            x2 = min(frame_width, x2)
            y2 = min(frame_height, y2)

            # ==================================
            # DRAW DETECTION BOX
            # ==================================

            cv2.rectangle(
                frame,
                (x1, y1),
                (x2, y2),
                (0, 255, 255),
                2
            )

            # ==================================
            # CENTER POINT
            # ==================================

            cx = (x1 + x2) // 2
            cy = (y1 + y2) // 2

            # Draw center point
            cv2.circle(
                frame,
                (cx, cy),
                8,
                (0, 0, 255),
                -1
            )

            # ==================================
            # COMMAND LOGIC
            # ==================================

            if cx < left_boundary:

                command = "LEFT"

            elif cx > right_boundary:

                command = "RIGHT"

            elif cy < top_boundary:

                command = "FORWARD"

            elif cy > bottom_boundary:

                command = "REVERSE"

            else:

                command = "STOP"

            # Only track first detected person
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
        (0, 255, 0),
        2
    )

    # ======================================
    # SHOW WINDOW
    # ======================================

    cv2.imshow(
        "YOLO Robot Control",
        frame
    )

    # ======================================
    # EXIT KEY
    # ======================================

    key = cv2.waitKey(1)

    if key == 27:
        break

# ==========================================
# CLEANUP
# ==========================================

cap.release()
cv2.destroyAllWindows()
