

/**********<Dependencies>**********/
// I2C
#include <Wire.h>

// RemoteXY
#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>
#define REMOTEXY_BLUETOOTH_NAME "TerraGator"  // RemoteXY connection settings 
#include <RemoteXY.h>
/**********</Dependencies>**********/



/**********<RemoteXY GUI Configuration>**********/
#pragma pack(push, 1)
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 184 bytes V19
{ 255, 6, 0, 0, 0, 177, 0, 19, 0, 0, 0, 0, 31, 2, 106, 200, 200, 84, 1, 1,
  8, 0, 5, 6, 121, 42, 42, 41, 27, 40, 40, 32, 2, 26, 31, 2, 18, 11, 67, 23,
  2, 10, 83, 9, 1, 2, 26, 31, 31, 69, 78, 65, 66, 76, 69, 0, 75, 73, 76, 76,
  0, 2, 30, 61, 44, 22, 103, 9, 83, 9, 0, 2, 26, 31, 31, 79, 78, 0, 79, 70,
  70, 0, 129, 25, 36, 56, 6, 23, 4, 47, 5, 64, 17, 77, 111, 116, 111, 114, 32, 69,
  110, 97, 98, 108, 101, 32, 40, 82, 101, 108, 97, 121, 41, 0, 129, 30, 85, 43, 7, 127,
  2, 37, 6, 64, 17, 69, 110, 103, 97, 103, 101, 32, 66, 114, 97, 107, 101, 0, 5, 56,
  121, 42, 42, 102, 27, 40, 40, 32, 2, 26, 31, 129, 16, 166, 21, 12, 55, 70, 11, 6,
  64, 17, 76, 101, 102, 116, 0, 129, 63, 166, 28, 12, 115, 70, 14, 6, 64, 17, 82, 105,
  103, 104, 116, 0
};

// this structure defines all the variables and events of your control interface
struct {

  // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t left_motor_y; // from -100 to 100
  uint8_t relay_sw; // =1 if switch ON and =0 if OFF, from 0 to 1
  uint8_t brake_sw; // =1 if switch ON and =0 if OFF, from 0 to 1
  int8_t joystick_02_x; // from -100 to 100
  int8_t right_motor_y; // from -100 to 100

  // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;
#pragma pack(pop)

/**********</RemoteXY GUI Configuration>**********/



/**********<Pin Definitions>**********/
// LEDs
#define LED_1_PIN     5
#define LED_2_PIN     17
#define LED_3_PIN     16

// I2C
#define I2C_SDA_PIN   21
#define I2C_SCL_PIN   22
/**********</Pin Definitions>**********/


/**********<Macros>**********/
// LED blink rate
#define BLINK_INTERVAL      500       // toggle LED once every BLINK_INTERVAL ms

// I2C Defines
#define esp32_i2c_address   0x20
#define rp2350_i2c_address  0x40
#define I2C_FREQ            100000

// Data formatting
#define SYNC_BIT_1          0x37
#define SYNC_BIT_2          0x01
#define ROVER_CLASS_ID      0x30
#define MOTOR_SUBCLASS_ID   0x00
#define ESP32_DEVICE_ID     0x5
/**********</Macros>**********/



/**********<Global Variables>**********/
uint8_t right_motor_dir = 0;
uint8_t left_motor_dir = 0;

uint8_t left_motor_speed = 0;
uint8_t right_motor_speed = 0;

unsigned long last_toggle_time = 0;
bool led_state = false;
/**********</Global Variables>**********/



/**********<ESP32 Setup Function>**********/
void setup() {

  // RemoteXY init
  RemoteXY_Init ();

  // Serial monitor init
  Serial.begin(115200);
  delay(10);
  Serial.println("ESP32 Initializing...");

  // Initialize LEDs
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);

  digitalWrite(LED_1_PIN, LOW);
  digitalWrite(LED_2_PIN, LOW);
  digitalWrite(LED_3_PIN, LOW);

  // I2C init and handler setup
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ);
}
/**********</ESP32 Setup Function>**********/



/**********<Public Functions>**********/

/*******send_motor_commands*******
* Description
    > builds an I2C packet, transmits to RP2350 over I2C
* Arguments
    > N/A
* Returns
   > N/A
*/
void send_motor_commands(){
  // declare variables for data to transmit
      uint8_t payload[3];
      uint8_t packet[9];
      uint16_t payload_length = 3;
      
  // determine values for motor speed and direction
      if (RemoteXY.right_motor_y < 0) {
        right_motor_dir = 0;
        right_motor_speed = -1 * RemoteXY.right_motor_y;
      }
      else {
        right_motor_dir = 1;
        right_motor_speed = RemoteXY.right_motor_y;
      }
    
      
      if (RemoteXY.left_motor_y < 0) {
        left_motor_dir = 0;
        left_motor_speed = -1 * RemoteXY.left_motor_y;
      }
      else {
        left_motor_dir = 1;
        left_motor_speed = RemoteXY.left_motor_y;
      }
 

   // build payload bytes
      payload[0] = (ESP32_DEVICE_ID   << 4) |
                   (RemoteXY.relay_sw << 3) |
                   (RemoteXY.brake_sw << 2) |
                   (left_motor_dir    << 1) |
                   (right_motor_dir   << 0);

      payload[1] = left_motor_speed;
      payload[2] = right_motor_speed;

  // build header bytes
      packet[0] = SYNC_BIT_1;
      packet[1] = SYNC_BIT_2;
      packet[2] = ROVER_CLASS_ID;
      packet[3] = MOTOR_SUBCLASS_ID;
      packet[4] = (uint8_t)(payload_length & 0xFF);           // payload len LSB
      packet[5] = (uint8_t)((payload_length >> 8) & 0xFF);    // payload len MSB

  // copy payload into packet
      packet[6] = payload[0];
      packet[7] = payload[1];
      packet[8] = payload[2];

  // send packet over I2C
      Wire.beginTransmission(rp2350_i2c_address);
      Wire.write(packet, sizeof(packet));
      Wire.endTransmission();

  // lastly, return
      return;
}

/**********</Public Functions>**********/


/**********<ESP32 Loop Function>**********/
void loop() {
  // run RemoteXY handler (required)
  RemoteXY_Handler();


  // toggle LED once per 500ms
  unsigned long current_time = millis();

  if (current_time - last_toggle_time >= BLINK_INTERVAL) {
    last_toggle_time = current_time;
    led_state = !led_state;
    digitalWrite(LED_1_PIN, led_state ? HIGH : LOW);
    send_motor_commands();
    Serial.print("500ms elapsed\n\r");
  }
}
/**********<ESP32 Loop Function>**********/
