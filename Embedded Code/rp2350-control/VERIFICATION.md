# RP2350 RemoteXY receive path

The single-joystick ESP32 firmware still sends the same nine-byte motor packet.
The RP2350 is the I2C master at 100 kHz; it addresses the ESP32 slave at 0x20.
It writes request 0x00 and reads one status byte. Status 1 triggers request 0x01
and a nine-byte read. Status 0 means no new command. Other status values are
ignored. Failed or short transfers do not refresh the motor-command watchdog.

Each request write is followed by a 2 ms preparation interval before reading.
On original ESP32 hardware, Arduino core 3.3.12 invokes onRequest after the
read completes. The ESP32 sketch therefore preloads the reply with slaveWrite
from onReceive. It must not queue the old reply again from onRequest. Both
firmwares must be updated together. A recurring `invalid sync bits (0x01, 0xFF)`
matches a one-byte status response consumed as a nine-byte motor response.
The delay allows task dispatch but is not a hard real-time scheduling guarantee;
hardware logs remain necessary to confirm reliable transfers.

## Packet contract

| Index | Value or field |
| --- | --- |
| 0, 1 | 0x37, 0x01 |
| 2, 3 | 0x30, 0x00 |
| 4, 5 | 0x03, 0x00 (little-endian payload length) |
| 6 | Device ID 5 in bits 7:4; relay bit 3; brake bit 2; left direction bit 1; right direction bit 0 |
| 7 | Left speed, 0–100 |
| 8 | Right speed, 0–100 |

Direction 1 is forward; direction 0 is reverse. There is no checksum in this
format. Invalid packets stop the motors in main.c. Loss of valid packets for
more than 500 ms stops PWM and disables the relay. The ESP32 refreshes commands
every 100 ms, including unchanged joystick values.

Example: `37 01 30 00 03 00 5B 32 32` commands both sides forward at 50% PWM.
`37 01 30 00 03 00 57 00 00` is a stop with relay off and brake requested.

## Output mapping

Left motor PWM: GP9, GP8, GP7. Right motor PWM: GP6, GP5, GP4.
Left forward direction: GP3=1, GP2=0. Right forward direction: GP1=0, GP0=1.
Reverse swaps each pair. Firmware brake mode sets both pairs low and PWM zero.
All three motors on each side receive that side's speed.

Relay control is GP39, active high. This is the custom RP2350B control-board
mapping, not a complete standard Pico 2 wiring configuration. Do not choose a
replacement relay GPIO without knowing the actual wiring. RELAY_FB is defined
but not read: get_relay_state reports the software command, not physical contact
feedback. Electrical braking and power enable depend on the external driver
and relay hardware.

## Checks performed

Native tests compile the production parser, I2C polling, PWM/motor driver,
relay/LED driver, and main loop against SDK mocks. They check:

- Exact I2C address/request sequence and nine-byte transfer.
- Forward, reverse and opposing directions; separate left/right PWM channels.
- Relay-off, brake, invalid identifiers/lengths/speeds and null input.
- No-data/invalid status, write errors and short reads.
- One initialization per PWM slice, zero PWM at initialization and all five LEDs.
- Real main-loop watchdog stopping motors after simulated packet loss.
- Actual ESP32 sketch with a post-read callback model: replies are ready before
  reads, one/nine-byte response switching works, and onRequest leaves no stale
  reply behind. Forward, turn and disconnected packets match the RP2350 format.

Run in PowerShell with a native GCC installation:

```powershell
./tests/run_tests.ps1 -Compiler C:/msys64/mingw64/bin/gcc.exe
```

Mocks verify software behavior, not electrical timing, ESP32 slave response
buffering, Bluetooth delivery, or physical motor movement. The CLI debug menu
is compiled but not called by main; auto_control.c contains no active control
logic. The active path is main -> rover_i2c_poll_esp32 ->
rover_i2c_parse_data -> rover_i2c_extract_motor_commands -> motor/relay outputs.

## Hardware diagnosis after flashing

1. Keep the wheels raised: RUN_FORWARD_STARTUP_TEST remains enabled and runs
   for five seconds on every boot, before command polling starts.
2. Confirm ESP32 GP21/SDA connects to RP2350 GP10/SDA, ESP32 GP22/SCL connects
   to RP2350 GP11/SCL, and both have a shared ground. Confirm bus pull-ups to
   3.3 V and both boards are powered.
3. Connect RemoteXY to TerraGator over BLE and move the joystick. ESP32 serial
   at 115200 should show connected=1, nonzero derived speeds, relay=1, brake=0.
4. On the RP2350 USB serial port, valid packet totals should increase and
   Motor RX should show the same speeds/directions. I2C write FAILED indicates
   a bus/slave problem. Rejected packet messages indicate malformed or stale
   response bytes. Valid packets requesting relay=0 or brake=1 mean stop.
5. If ESP32 controls change but RP2350 valid totals remain zero, capture logs
   from both boards before changing the protocol. A successful startup motor
   test does not exercise the ESP32/I2C receive path.

Compile Project before Run Project (USB): the current Run task does not itself
declare a build dependency. The UF2 is build/Rover-Control-Board.uf2.
