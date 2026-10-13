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
// Single auto-centering joystick, portrait and landscape layouts (35 bytes).
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] = {
  255, 2, 0, 0, 0, 28, 0, 19, 0, 0, 0, 0, 31, 2, 106, 200, 200, 84, 1, 1,
  1, 0, 5, 23, 70, 60, 60, 70, 12, 60, 60, 32, 2, 26, 31
};
struct {
  int8_t joystick_x;
  int8_t joystick_y;
  uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)

// Derived controls retain the existing nine-byte RP2350 packet format.
struct {
  int8_t left_motor_y;
  int8_t right_motor_y;
  uint8_t relay_sw;
  uint8_t brake_sw;
} drive_controls = {0, 0, 0, 1};
portMUX_TYPE control_mux = portMUX_INITIALIZER_UNLOCKED;

void update_drive_controls() {
  int x = constrain((int)RemoteXY.joystick_x, -100, 100);
  int y = constrain((int)RemoteXY.joystick_y, -100, 100);
  if (abs(x) < 5) x = 0;
  if (abs(y) < 5) y = 0;
  if (!RemoteXY.connect_flag) x = y = 0;
  int left = y + x;
  int right = y - x;
  const int peak = max(abs(left), abs(right));
  if (peak > 100) {
    left = left * 100 / peak;
    right = right * 100 / peak;
  }
  portENTER_CRITICAL(&control_mux);
  drive_controls.left_motor_y = left;
  drive_controls.right_motor_y = right;
  const bool moving = left != 0 || right != 0;
  drive_controls.relay_sw = moving ? 1 : 0;
  drive_controls.brake_sw = moving ? 0 : 1;
  portEXIT_CRITICAL(&control_mux);
}

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
#define I2C_FREQ            100000

// I2C request types
#define I2C_STATUS_REQUEST      0x00
#define I2C_MOTOR_DATA_REQUEST  0x01

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

// previous RemoteXY control values
int8_t prev_left_motor_y = 0;
int8_t prev_right_motor_y = 0;
uint8_t prev_relay_sw = 0;
uint8_t prev_brake_sw = 0;

// true when RemoteXY control data has changed
volatile bool new_data = false;

// stores what type of data the RP2350 is requesting
volatile uint8_t i2c_request = I2C_STATUS_REQUEST;
volatile uint32_t i2c_receive_count = 0;
volatile uint32_t i2c_reply_count = 0;
volatile uint32_t i2c_read_count = 0;
volatile uint32_t i2c_reply_errors = 0;
bool i2c_ready = false;

unsigned long last_toggle_time = 0;
unsigned long last_command_refresh = 0;
#define COMMAND_REFRESH_MS 100
bool led_state = false;
/**********</Global Variables>**********/



/**********<ESP32 Setup Function>**********/
void setup() {

  // Serial monitor init
  Serial.begin(115200);
  delay(10);
  Serial.println("ESP32 Initializing...");

  // RemoteXY init
  RemoteXY_Init ();
  Serial.println("RemoteXY BLE device: " REMOTEXY_BLUETOOTH_NAME);
  Serial.println("Phone: RemoteXY app -> + -> Bluetooth BLE -> TerraGator");
  Serial.println("Control values print every 200 ms; joystick range is -100 to 100.");

  // Initialize LEDs
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);

  digitalWrite(LED_1_PIN, LOW);
  digitalWrite(LED_2_PIN, LOW);
  digitalWrite(LED_3_PIN, LOW);

  // I2C init and handler setup
  // if an address is provided, it becomes a slave; if no address is provided, it becomes a master
  // called when RP2350 writes a request type to the ESP32
  Wire.onReceive(receive_i2c_request);

  // called when RP2350 reads data from the ESP32
  Wire.onRequest(send_i2c_data);
  i2c_ready = Wire.begin((uint8_t)esp32_i2c_address, I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ);
  Serial.printf("I2C slave %s: address=0x20 SDA=21 SCL=22\n", i2c_ready ? "READY" : "FAILED");
}
/**********</ESP32 Setup Function>**********/



/**********<Public Functions>**********/

// Log from the main loop, never from the I2C callbacks.
void print_remotexy_controls() {
  static unsigned long last_print_time = 0;
  static int previous_connection = -1;
  const unsigned long now = millis();
  const bool connection_changed = previous_connection != RemoteXY.connect_flag;

  if (connection_changed) {
    previous_connection = RemoteXY.connect_flag;
    Serial.println(RemoteXY.connect_flag ? "RemoteXY phone CONNECTED"
                                        : "RemoteXY phone DISCONNECTED: motors stopped, relay off, brake on");
  }

  if (!connection_changed && now - last_print_time < 200) {
    return;
  }
  last_print_time = now;
  Serial.printf("RemoteXY connected=%u | joystick(x=%d,y=%d) | left=%d right=%d | relay=%u | brake=%u\n",
                (unsigned int)RemoteXY.connect_flag,
                (int)RemoteXY.joystick_x, (int)RemoteXY.joystick_y,
                (int)drive_controls.left_motor_y, (int)drive_controls.right_motor_y,
                (unsigned int)drive_controls.relay_sw, (unsigned int)drive_controls.brake_sw);
}

/*******send_motor_commands*******
 builds an I2C packet, transmits to RP2350 over I2C
*/
void send_motor_commands(){
  // declare variables for data to transmit
      uint8_t payload[3];
      uint8_t packet[9];
      uint16_t payload_length = 3;
      // Snapshot all four fields together; the BLE loop runs on another task.
      portENTER_CRITICAL(&control_mux);
      const auto controls = drive_controls;
      portEXIT_CRITICAL(&control_mux);

  // determine values for motor speed and direction
      if (controls.right_motor_y < 0) {
        right_motor_dir = 0;
        right_motor_speed = -1 * controls.right_motor_y;
      }
      else {
        right_motor_dir = 1;
        right_motor_speed = controls.right_motor_y;
      }


      if (controls.left_motor_y < 0) {
        left_motor_dir = 0;
        left_motor_speed = -1 * controls.left_motor_y;
      }
      else {
        left_motor_dir = 1;
        left_motor_speed = controls.left_motor_y;
      }


   // build payload bytes
      payload[0] = (ESP32_DEVICE_ID   << 4) |
                   (controls.relay_sw << 3) |
                   (controls.brake_sw << 2) |
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
   // on request will trigger this write
      write_i2c_reply(packet, sizeof(packet));


      return;
}


/*******receive_i2c_request*******
  receives a request type from the RP2350 over I2C
Arguments:
    int num_bytes: number of bytes received from RP2350

*/
void receive_i2c_request(int num_bytes){

  // make sure there is data available
  if (Wire.available()) {

    // save what type of data the RP2350 wants
    i2c_request = Wire.read();
    i2c_receive_count++;
  }

  // discard any extra bytes
  while (Wire.available()) {
    Wire.read();
  }

#if CONFIG_IDF_TARGET_ESP32
  // Original ESP32 cannot prepare a reply at read-address time. Its onRequest
  // runs AFTER the read. Preload the FIFO now, before the master's next read.
  prepare_i2c_reply();
#endif

  // lastly, return
  return;
}


/*******send_i2c_data*******
* Description
    > sends requested data to RP2350 when RP2350 performs an I2C read
* Arguments
    > N/A
* Returns
   > N/A
*/
void send_i2c_data(){
  i2c_read_count++;
#if !CONFIG_IDF_TARGET_ESP32
  // Newer ESP32 variants support preparing the reply during the read request.
  prepare_i2c_reply();
#endif
}

void write_i2c_reply(const uint8_t *data, size_t length) {
#if CONFIG_IDF_TARGET_ESP32
  const size_t written = Wire.slaveWrite(data, length);
#else
  const size_t written = Wire.write(data, length);
#endif
  if (written == length) i2c_reply_count++;
  else i2c_reply_errors++;
}

void prepare_i2c_reply(){

  // RP2350 is polling to see if new RemoteXY data exists
  if (i2c_request == I2C_STATUS_REQUEST) {

    uint8_t status = new_data ? 1 : 0;

    write_i2c_reply(&status, 1);
  }

  // RP2350 is requesting the newest motor command packet
  else if (i2c_request == I2C_MOTOR_DATA_REQUEST) {

    send_motor_commands();

    // newest data has now been sent to RP2350
    new_data = false;
  }
  else {
    const uint8_t invalid_request = 0;
    write_i2c_reply(&invalid_request, 1);
  }

  // lastly, return
  return;
}


// check for update of the motor data
void check_for_new_commands() {

    bool data_changed = false;

    if (drive_controls.left_motor_y != prev_left_motor_y) {
        data_changed = true;
    }

    if (drive_controls.right_motor_y != prev_right_motor_y) {
        data_changed = true;
    }

    if (drive_controls.relay_sw != prev_relay_sw) {
        data_changed = true;
    }

    if (drive_controls.brake_sw != prev_brake_sw) {
        data_changed = true;
    }

    if (!data_changed) {
        return;
    }

    // Save current values for next comparison
    prev_left_motor_y = drive_controls.left_motor_y;
    prev_right_motor_y = drive_controls.right_motor_y;
    prev_relay_sw = drive_controls.relay_sw;
    prev_brake_sw = drive_controls.brake_sw;

    // Tell RP2350 that new control data exists
    new_data = true;
}

/**********</Public Functions>**********/


/**********<ESP32 Loop Function>**********/
void loop() {
  // run RemoteXY handler (required)
  RemoteXY_Handler();
  update_drive_controls();

  /* if user change the joy stick on the phone
    RemoteXY library updates
    drive_controls.left_motor_y = ...
  */
  // check whether phone controls changed
  check_for_new_commands();
  print_remotexy_controls();
  static unsigned long last_i2c_log = 0;
  if (millis() - last_i2c_log >= 1000) {
    last_i2c_log = millis();
    Serial.printf("I2C ready=%u requests=%lu replies=%lu reads=%lu errors=%lu\n",
                  i2c_ready, (unsigned long)i2c_receive_count,
                  (unsigned long)i2c_reply_count, (unsigned long)i2c_read_count,
                  (unsigned long)i2c_reply_errors);
  }
  // Refresh unchanged commands for the RP communication watchdog.
  if (millis() - last_command_refresh >= COMMAND_REFRESH_MS) {
    last_command_refresh = millis();
    new_data = true;
  }

  // toggle LED once per 500ms
  unsigned long current_time = millis();

  if (current_time - last_toggle_time >= BLINK_INTERVAL) {
    last_toggle_time = current_time;
    led_state = !led_state;
    digitalWrite(LED_1_PIN, led_state ? HIGH : LOW);

  }
}
/**********</ESP32 Loop Function>**********/
