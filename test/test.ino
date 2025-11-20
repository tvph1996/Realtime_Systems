// Define a variable to hold the incoming data
char incomingByte = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize Serial communication at 9600 bits per second:
  // Note: For Arduino Due, you may often use 115200 for faster data transfer, 
  // but 9600 is standard and safe for testing.
  Serial.begin(9600);
  Serial.println("--- LED Control Test ---");
  Serial.println("Type '1' to turn the LED ON.");
  Serial.println("Type '0' to turn the LED OFF.");
}

// the loop function runs over and over again forever
void loop() {
  // Check if there is any data available to read from the serial port
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer
    incomingByte = Serial.read();

    // Check what the incoming byte is
    if (incomingByte == '1') {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("LED is now ON");
    } 
    else if (incomingByte == '0') {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("LED is now OFF");
    }
    // Optional: Echo back the character if it's not '1' or '0'
    else {
      Serial.print("Received unknown command: ");
      Serial.println(incomingByte);
    }
  }
}