import evdev
from evdev import InputDevice, categorize, ecodes

print_log = False

# Confirm path for controller with: sudo evtest
device_path = '/dev/input/event4'  
controller = InputDevice(device_path)

# Open a text file to write the output
with open('button_dump.txt', 'w') as logfile:
    print(f"Listening to {controller.name}...")
    logfile.write(f"Listening to {controller.name}...\n")

    for event in controller.read_loop():
        # Log the raw event
        logfile.write(f"Raw event: {event}\n")
        
        if event.type == ecodes.EV_KEY:
            key_event = categorize(event)
            if key_event.keystate == key_event.key_up:
                logfile.write(f"Button released: {key_event.keycode}\n")
                if(print_log):
                    print(f"Button released: {key_event.keycode}")
            elif key_event.keystate == key_event.key_down:
                logfile.write(f"Button pressed: {key_event.keycode}\n")
                if(print_log):
                    print(f"Button pressed: {key_event.keycode}")
        
        elif event.type == ecodes.EV_ABS:
            abs_event = categorize(event)
            logfile.write(f"Analog input: {abs_event.event.code} - {abs_event.event.value}\n")
            if(print_log):
                print(f"Analog input: {abs_event.event.code} - {abs_event.event.value}")
