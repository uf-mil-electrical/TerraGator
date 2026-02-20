/*
 * Esp32 Recevier Code
 * Listens for "HELLO" and prints it to the (PuTTY) terminal.
 * base on NRF.ino file 
 */

#include <SPI.h> // communicate with the nrf board through spi
#include <nRF24L01.h>
#include <RF24.h>

// Initialize radio on ESP32 specific pins (CE=4, CSN=5)
RF24 radio(4, 5); 

// The pine address -  match the Sender exactly
const byte address[6] = "FETRI"; 

void setup() {
  // Start the link to the (PuTTY)
  // Baud rate 115200 is standard for ESP32
  Serial.begin(115200); 
  
  radio.begin();              // Boot up the nRF24L01 chip
  
  // low to reduce power 
  radio.setPALevel(RF24_PA_LOW); 
  
  // Open the channel to listen on (Pipe 0)
  radio.openReadingPipe(0, address); 
  
  // Start listening to hear the sender (Receiver Mode)
  radio.startListening(); 
  
  Serial.println("Receiver Ready. Waiting for 'HELLO'...");
}

void loop() {
  // Check if a radio signal has arrived
  if (radio.available()) {
    
    // Create a temporary bucket with size of 32 to hold the incoming text
    
    char text[32] = ""; 
    
    // Read the data from the radio chip into our bucket
    radio.read(&text, sizeof(text)); 
    
    // Print the message to the PC (This shows up in PuTTY!)
    Serial.println(text); 
  }
}