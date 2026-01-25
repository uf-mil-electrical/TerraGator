

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

uint8_t RemoteXY_CONF[] =   // 141 bytes
  { 255,4,0,0,0,134,0,19,0,0,0,0,31,2,106,200,200,84,1,1,
  5,0,5,26,125,52,52,132,15,60,60,32,2,26,31,2,20,10,67,23,
  17,14,98,11,1,2,26,31,31,69,78,65,66,76,69,0,68,73,83,65,
  66,76,69,0,2,30,61,44,22,17,49,98,9,0,2,26,31,31,79,78,
  0,79,70,70,0,129,21,35,65,7,40,27,56,6,64,17,77,111,116,111,
  114,32,69,110,97,98,108,101,32,40,82,101,108,97,121,41,0,129,30,85,
  43,7,48,59,37,6,64,17,69,110,103,97,103,101,32,66,114,97,107,101,
  0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  uint8_t relay_sw; // =1 if switch ON and =0 if OFF, from 0 to 1
  uint8_t brake_sw; // =1 if switch ON and =0 if OFF, from 0 to 1

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;  

#pragma pack(pop)
/**********</RemoteXY GUI Configuration>**********/



/**********<Defines>**********/
// I2C Defines
#define esp32_i2c_address 0x20
#define i2c_clock_freq    100000
/**********</Defines>**********/



/**********<Global Variables>**********/
int8_t steering = 0;
int8_t velocity = 0;
uint8_t relay_state = 0;
uint8_t brake_state = 0;
/**********</Global Variables>**********/


/**********<I2C Request/Receive ISRs>**********/
// I2C data REQUEST ISR
void i2c_request() {
  
  uint8_t data_to_send[4];
  data_to_send[0] = (uint8_t)(steering);
  data_to_send[1] = (uint8_t)(velocity);
  data_to_send[2] = relay_state;
  data_to_send[3] = brake_state;

  Wire.write(data_to_send, 4);  // send four bytes of data

  Serial.println("I2C: data REQUEST received");
}

// I2C data RECEIVE ISR
void i2c_receive(int num_bytes) {
  uint8_t data_received;
  while(Wire.available()){
    data_received = Wire.read();
  }
  Serial.print("I2C: data RECIEVED: ");
  Serial.print(data_received);
  Serial.print("\n");
}
/**********</I2C Request/Receive ISRs>**********/


/**********<ESP32 Setup Function>**********/
void setup() {

  // RemoteXY init
  RemoteXY_Init ();

  // Serial monitor init
  Serial.begin(115200);
  delay(10);
  Serial.println("Waiting for I2C request from master device");

  // I2C init and handler setup
  Wire.setClock(i2c_clock_freq);  // set I2C clock frequency
  Wire.begin(esp32_i2c_address);  // join I2C bus in slave mode with this address
  Wire.onRequest(i2c_request);    // what happens when data is requested?
  Wire.onReceive(i2c_receive);    // what happens when data is received?
}
/**********</ESP32 Setup Function>**********/


/**********<ESP32 Loop Function>**********/
void loop() {
  // First, run RemoteXY handler (required)
    RemoteXY_Handler();

  // Second, get updated values
    steering = RemoteXY.joystick_01_x;
    velocity = RemoteXY.joystick_01_y;
    relay_state = RemoteXY.relay_sw;
    brake_state = RemoteXY.brake_sw;
}
/**********<ESP32 Loop Function>**********/
