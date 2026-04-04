import serial
import time
import cv2
import serial.tools.list_ports


ESP_BAUD = 115200

SAVE_PHOTO_PATH = "intruder_photo.jpg"

ESP_PORT = "COM3"   

PHOTO_DELAY = 15    

last_photo_time = 0



def validate_port(port_name):

    available = [p.device for p in serial.tools.list_ports.comports()]

    if port_name not in available:

        print("\n ERROR: Port", port_name, "not found!")

        print("Available ports:", available)

        exit()

    return True


validate_port(ESP_PORT)


print("Connecting to ESP32 on", ESP_PORT, "...")

try:

    ser = serial.Serial(ESP_PORT, ESP_BAUD, timeout=1)

except Exception as e:

    print("\n Failed to open port:", e)

    print("➡ Close Arduino Serial Monitor and try again.")

    exit()


time.sleep(2)

print("Connected!\nWaiting for ESP messages...\n")


def take_photo():

    global last_photo_time

    if time.time() - last_photo_time < PHOTO_DELAY:

        print(" Skipping photo (delay active)")

        return



    print("Taking intruder photo...")



    cam = cv2.VideoCapture(0)

    time.sleep(0.8)

    ret, frame = cam.read()

    cam.release()



    if not ret:

        print(" Camera failed to capture image!")

        return



    cv2.imwrite(SAVE_PHOTO_PATH, frame)

    last_photo_time = time.time()

    print("Photo saved as:", SAVE_PHOTO_PATH)


while True:

    try:

        line = ser.readline().decode(errors="ignore").strip()

        if not line:

            continue


        print("ESP:", line)


        access_denied_patterns = [

            "ACCESS_DENIED",

            "Access Denied",

            "Access denied",

            "Invalid Finger",

            "InvalidFingerprint"

        ]


        no_finger_patterns = [

            "No Finger Found",

            "No Finger",

            "no finger"

        ]


        if any(keyword in line for keyword in no_finger_patterns):

            print(" No finger detected → NOT taking photo")

            continue


        if any(keyword in line for keyword in access_denied_patterns):

            take_photo()


    except KeyboardInterrupt:

        print("\nExiting...")

        break


    except Exception as e:

        print("Error:", e)

