#include <WiFi.h>
#include <WebSocketsServer.h>

// Wi-Fi credentials
const char* ssid = "-----";
const char* password = "-----";

// WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Pin Definitions
const int LED_PIN_R = 2; // D2 pin for LED
const int LED_PIN_G = 4; // D2 pin for LED
const int segPins[] = {26, 27, 13, 12, 14, 25, 33};  // Pins for segments a to g of the 7-segment display
//                     A   B   C   D   E   F   G

// State Variables
bool ledRedState = false;
bool ledGreenState = false;
bool isBlinkingRed = false;
bool isBlinkingGreen = false; // Tracks blinking state
unsigned long redPreviousMillis = 0; 
unsigned long greenPreviousMillis = 0; // Stores time for blink delay
int redBlinkCount = 0, greenBlinkCount = 0; // Number of blinks
int redBlinkDelay = 0, greenBlinkDelay = 0; // Delay between blinks in milliseconds
int redCurrentBlinkCount = 0, greenCurrentBlinkCount = 0; // Counter for current number of blinks

//Websoket massage Conditions
bool isLEDcommand = false;
bool isBlinkcommand = false;
bool isShutDownCommand = false;
bool isNumberCommand = false;

// Segment patterns for numbers 0-9 (a-g)
// 1 = ON, 0 = OFF
const byte segmentPatterns[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

//---------------------------------------LED-----------------------------------------//

// LED control functions
void turnRedLEDOn() {
  digitalWrite(LED_PIN_R, HIGH);
  ledRedState = true;
  isBlinkingRed = false;
  isLEDcommand = true;
  webSocket.broadcastTXT("Red LED turned ON");
}

void turnRedLEDOff() {
  digitalWrite(LED_PIN_R, LOW);
  ledRedState = false;
  isBlinkingRed = false;
  isLEDcommand = true;
  webSocket.broadcastTXT("Red LED turned OFF");
}

void turnGreenLEDOn() {
  digitalWrite(LED_PIN_G, HIGH);
  ledGreenState = true;
  isBlinkingGreen = false;
  isLEDcommand = true;
  webSocket.broadcastTXT("Green LED turned ON");
}

void turnGreenLEDOff() {
  digitalWrite(LED_PIN_G, LOW);
  ledGreenState = false;
  isBlinkingGreen = false;
  isLEDcommand = true;
  webSocket.broadcastTXT("Green LED turned OFF");
}

// Function to handle LED control command
void handleLEDCommand(String sanitizedMessage) {
  if (sanitizedMessage.indexOf("turn on red led") != -1) {
    turnRedLEDOn();
  } else if (sanitizedMessage.indexOf("turn on green led") != -1) {
    turnGreenLEDOn();
  } else if (sanitizedMessage.indexOf("turn off red led") != -1) {
    turnRedLEDOff();
  } else if (sanitizedMessage.indexOf("turn off green led") != -1) {
    turnGreenLEDOff();
  } else if (sanitizedMessage.indexOf("turn off leds") != -1) {
    turnRedLEDOff();
    turnGreenLEDOff();
    webSocket.broadcastTXT("All LEDs turned OFF");
  } else {
      isLEDcommand = false;
  }
}

//--------------------------------------Blinking-----------------------------------------//

void startRedBlinking(int count, int delayTime) {
  isBlinkingRed = true;
  redBlinkCount = count;
  redBlinkDelay = delayTime;
  redCurrentBlinkCount = 0;
  isBlinkcommand = true;
  redPreviousMillis = millis();
  webSocket.broadcastTXT("Red LED blinking started");
}

void startGreenBlinking(int count, int delayTime) {
  isBlinkingGreen = true;
  greenBlinkCount = count;
  greenBlinkDelay = delayTime;
  greenCurrentBlinkCount = 0;
  isBlinkcommand = true;
  greenPreviousMillis = millis();
  webSocket.broadcastTXT("Green LED blinking started");
}

// Function to handle blink command
void handleBlinkCommand(String sanitizedMessage) {
  if (sanitizedMessage.indexOf("blink red") != -1) {
    startRedBlinking(10, 1000); // Blink red LED 10 times with 1-second delay
  } else if (sanitizedMessage.indexOf("blink green") != -1) {
    startGreenBlinking(10, 1000); // Blink green LED 10 times with 1-second delay
  } else {
      isBlinkcommand = false;
  }
}

//------------------------------------Numbers-----------------------------------------//

// Function to display a number on the 7-segment display
void displayNumber(int number) {
  if (number < 0 || number > 9) return; // Invalid number
  
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], segmentPatterns[number][i]);
  }
}

// Function to handle 'number' commands
void handleNumberCommand(String sanitizedMessage) {
  String numberKeywords[] = {"display", "number", "show", "#"};
  bool containsKeyword = false;

  // Check if the message contains any number-related keyword
  for (String keyword : numberKeywords) {
    if (sanitizedMessage.indexOf(keyword) != -1) {
      containsKeyword = true;
      break; // Exit loop if a keyword is found
    }
  }

  if (!containsKeyword) {
    isNumberCommand = false;
    return; // Exit if no relevant keyword is found
  }

  isNumberCommand = true;
  int number = -1;

  // Spelled numbers
  if (sanitizedMessage.indexOf("one") != -1) number = 1;
  else if (sanitizedMessage.indexOf("two") != -1) number = 2;
  else if (sanitizedMessage.indexOf("three") != -1 || sanitizedMessage.indexOf("tree") != -1) number = 3;
  else if (sanitizedMessage.indexOf("four") != -1 || sanitizedMessage.indexOf("for") != -1) number = 4;
  else if (sanitizedMessage.indexOf("five") != -1) number = 5;
  else if (sanitizedMessage.indexOf("six") != -1) number = 6;
  else if (sanitizedMessage.indexOf("seven") != -1) number = 7;
  else if (sanitizedMessage.indexOf("eight") != -1) number = 8;
  else if (sanitizedMessage.indexOf("nine") != -1 || sanitizedMessage.indexOf("name") != -1) number = 9;
  else if (sanitizedMessage.indexOf("zero") != -1) number = 0;
  // Digital numbers
  else if (sanitizedMessage.indexOf("1") != -1) number = 1;
  else if (sanitizedMessage.indexOf("2") != -1) number = 2;
  else if (sanitizedMessage.indexOf("3") != -1) number = 3;
  else if (sanitizedMessage.indexOf("4") != -1) number = 4;
  else if (sanitizedMessage.indexOf("5") != -1) number = 5;
  else if (sanitizedMessage.indexOf("6") != -1) number = 6;
  else if (sanitizedMessage.indexOf("7") != -1) number = 7;
  else if (sanitizedMessage.indexOf("8") != -1) number = 8;
  else if (sanitizedMessage.indexOf("9") != -1) number = 9;
  else if (sanitizedMessage.indexOf("0") != -1) number = 0;

  // If a number was found
  if (number != -1) {
    displayNumber(number); // Display the number on the 7-segment display
    webSocket.broadcastTXT("Displaying number: " + String(number));
  } else {
    webSocket.broadcastTXT("Invalid number command");
  }
}


void turnOff7Segment() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], LOW); // Set all segment pins to LOW (OFF)
  }
}

//-------------------------------------ShutDown-------------------------------------//

// Function to handle shutdown command
void handleShutdownCommand(String sanitizedMessage) {
  if (sanitizedMessage.indexOf("shut down") != -1 || sanitizedMessage.indexOf("shutdown") != -1) {
    turnRedLEDOff();
    turnGreenLEDOff();
    turnOff7Segment(); // Turns off the 7-segment display
    webSocket.broadcastTXT("System shut down");
    isShutDownCommand = true;
  } else {
      isShutDownCommand = false;
  }
}

//---------------------------------Debug Functions----------------------------------//

// Command sanitization: remove extra spaces and convert to lowercase
String sanitizeCommand(String command) {
  command.trim();       // Remove leading/trailing spaces
  command.toLowerCase(); // Convert to lowercase
  return command;
}

// Handle WebSocket messages
void handleWebSocketMessage(String message) {
  String sanitizedMessage = sanitizeCommand(message);
  Serial.println("Received Command: " + sanitizedMessage);

  // Command Processing
  handleLEDCommand(sanitizedMessage);
  handleBlinkCommand(sanitizedMessage);
  handleShutdownCommand(sanitizedMessage);
  handleNumberCommand(sanitizedMessage);
  // Process commands or fallback
  if (!isNumberCommand && !isLEDcommand && !isShutDownCommand && !isBlinkcommand) {
    webSocket.broadcastTXT("Unknown command");
  }
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char*)payload).substring(0, length);
    handleWebSocketMessage(message);
  }
}

void setup() {
  pinMode(LED_PIN_R, OUTPUT); // Set red LED pin as output
  pinMode(LED_PIN_G, OUTPUT); // Set green LED pin as output

  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT); // Set all segment pins as output
  }

  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Wi-Fi connected!");
  Serial.print("IP Address: " );
  Serial.println(WiFi.localIP());
  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");
}

// BLINKING Action function

void loop() {
  webSocket.loop(); // Process WebSocket events

  // Handle blinking LED
  if (isBlinkingRed) {
    unsigned long currentMillis = millis();
    if (currentMillis - redPreviousMillis >= redBlinkDelay) {
      redPreviousMillis = currentMillis;
      if (ledRedState) {
        digitalWrite(LED_PIN_R, LOW);
        ledRedState = false;
        redCurrentBlinkCount++;
      } else {
        digitalWrite(LED_PIN_R, HIGH);
        ledRedState = true;
      }
      if (redCurrentBlinkCount >= redBlinkCount) {
        isBlinkingRed = false;
        turnRedLEDOff();
      }
    }
  }

  if (isBlinkingGreen) {
    unsigned long currentMillis = millis();
    if (currentMillis - greenPreviousMillis >= greenBlinkDelay) {
      greenPreviousMillis = currentMillis;
      if (ledGreenState) {
        digitalWrite(LED_PIN_G, LOW);
        ledGreenState = false;
        greenCurrentBlinkCount++;
      } else {
        digitalWrite(LED_PIN_G, HIGH);
        ledGreenState = true;
      }
      if (greenCurrentBlinkCount >= greenBlinkCount) {
        isBlinkingGreen = false;
        turnGreenLEDOff();
      }
    }
  }

}
