import cv2
import os

# Define the directory where you want to save the images
save_directory = "/home/raspberry/Mars-Rover/firmware/camera/camera_test"
if not os.path.exists(save_directory):
    os.makedirs(save_directory)

# Initialize the camera
camera = cv2.VideoCapture(0)

if not camera.isOpened():
    print("Error: Could not access the camera.")
    exit()

while True:
    # Capture a frame
    ret, frame = camera.read()
    
    if not ret:
        print("Error: Failed to capture image.")
        break

    # Generate a unique filename
    filename = f'{save_directory}/latest_frame.jpg'
    
    # Save the image
    cv2.imwrite(filename, frame)

    # Optional: sleep for a short period to reduce the frequency of saves
    cv2.waitKey(100)  # Adjust as needed for your use case

camera.release()
