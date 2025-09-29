import time
import serial
import adafruit_gps

# Set up the serial connection (Update with your port)
# use next line when on linux
uart = serial.Serial("/dev/ttyUSB1", baudrate=9600, timeout=10)
# use next line when on windows replacing COM with the correct port
# COM = "COM5"
# uart = serial.Serial(COM, baudrate=9600, timeout=10)

# Initialize GPS module
gps = adafruit_gps.GPS(uart, debug=False)

# Configure GPS module`        
gps.send_command(b"PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")
gps.send_command(b"PMTK220,1000")
# gps.send_command(b"PMTK220,1000")  # Set update rate to 1 Hz
# gps.send_command(b"PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")  # Enable only RMC & GGA

# Read GPS data
def check_gps():
    gps.update()
    if gps.has_fix:
        '''
        print(f"Latitude: {gps.latitude:.6f}, Longitude: {gps.longitude:.6f}")
        print(f"Altitude: {gps.altitude_m} meters")
        print(f"Speed: {gps.speed_knots} knots")
        print(f"Satellites: {gps.satellites}")
        '''
        return [gps.latitude, gps.longitude, float(gps.altitude_m), float(gps.speed_knots), float(gps.satellites)]

    else:
        return []

if __name__ == "__main__":
    print(check_gps())
