'''
This file is used to control the motors and allow the rover to change directions. 
Run this file by typing: 'python mwd.py []', with the direction in brackets.
Send w for forwards, s for backwards, a for left, d for right, and e to stop all movement.
'''

import time
from adafruit_motorkit import MotorKit
import sys



'''
kit11:
MOTOR 1: Front right, proper forwards
MOTOR 2: Back Right, proper
MOTOR 3: Back left, inverted
MOTOR 4: Front left, inverted

kit12:
MOTOR 1: left, proper
MOTOR 4: right, inverted

'''

def main():
    kit1 = MotorKit()
    kit2 = MotorKit(address=0x61)
    
    if len(sys.argv) != 2:
        print("Usage: python script.py <direction>")
        return

    direction = sys.argv[1].lower()

    if direction == "w": #straight
        print("Heading forwards")
        kit1.motor1.throttle = 0.9
        kit1.motor2.throttle = 0.9
        kit1.motor3.throttle = -0.9
        kit1.motor4.throttle = -0.9
        kit2.motor1.throttle = 0.9
        kit2.motor4.throttle = -0.9
        
        
    elif direction == "a": #left
        print("Heading left") 
        kit1.motor1.throttle = 0.5
        kit1.motor2.throttle = 0.5
        kit1.motor3.throttle = 0.5
        kit1.motor4.throttle = 0.5
        kit2.motor1.throttle = -0.5
        kit2.motor4.throttle = -0.5
        
    elif direction == "s":#backwards
        print("Heading backwards")
        kit1.motor1.throttle = -0.9
        kit1.motor2.throttle = -0.9
        kit1.motor3.throttle = 0.9
        kit1.motor4.throttle = 0.9
        kit2.motor1.throttle = -0.9
        kit2.motor4.throttle = 0.9
        
        
    elif direction == "d":#right
        print("Heading right")
        kit1.motor1.throttle = -0.5
        kit1.motor2.throttle = -0.5
        kit1.motor3.throttle = -0.5
        kit1.motor4.throttle = -0.5
        kit2.motor1.throttle = 0.4
        kit2.motor4.throttle = 0.5
        
    elif direction == "e":#stop
        kit1.motor1.throttle = 0
        kit1.motor2.throttle = 0
        kit1.motor3.throttle = 0
        kit1.motor4.throttle = 0
        kit2.motor1.throttle = 0
        kit2.motor4.throttle = 0
        

if __name__ == "__main__":
    main()
