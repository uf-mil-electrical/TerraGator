/* 
Sender Esp32 code 
initalize the radio, sets it to transmitter mode,
and broadcast "Hello" every second. 

Logic adapted from: Newer_remote_Mega_Code/NRF.ino 
*/

// hardware libraries 
#include <SPI.h>        // Handles the SPI (SCK, MOSI, MISO)
#include <nRF24L01.h>   // Definitions for the nRF24L01 radio chip
#include <RF24.h>       // The main library to control the radio

// hardware config 
//CE PIn = GPIO 4, CS PIN = 5
RF24 radio(4,5);   

//Address Config 
// The address is taken from Newer_Remote 
const byte address[6] = "FETRI";

void setup() {
  // initalize serial monitor for debuging
  Serial.begin(115200); 

  radio.begin();

// changed into low to reduce power so ESP32 can handle it 
  radio.setPALevel(RF24_PA_LOW);

// Open the communication channel to the specific address "FETRI"
  radio.openWritingPipe(address);
  
  // Stop listening so turn on Transmitter Mode
  radio.stopListening();


}

void loop() {
    // Define the message to send
  const char text[] = "HELLO";

  // Send the message wirelessly
  // radio.write() returns true if the receiver acknowledged the packet
  //            pass in the address of text and the length of it 
  bool success = radio.write(&text, sizeof(text));
 
  // Print debug info to Serial Monitor 
  if (success) {
    Serial.println("Sent: HELLO");
  } else {
    Serial.println("Sent (recevier not on yet )"); 
  }

  // Wait 1 second before sending again
  delay(1000);

}
