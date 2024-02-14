#include <WiFi.h>

int enin1Pin = 18; // TODO: Change
int in1Pin = 19; // TODO: Change
int in2Pin = 21; // TODO: Change

int openDirection = 1;
int openSpeed = 255;
int openRunTime = 1100;
int closeDirection = 0;
int closeSpeed = 255;
int closeRunTime = 1100;

const char* ssid = "Monson";
const char* password = "saint raphael";

WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

const char* textBody = "HTTP/1.1 200 OK\nContent-type:text/plain\nConnection: close\n\nok\n\n";
const char* htmlBody = "HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n\n<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>Mischief Tech Cookie Jar</title><style>body{font-family:Arial,Helvetica,sans-serif;background:#fcefd4}main{width:300px;margin:0 auto}h1{text-align:center;color:#2b354f}button{appearance:none;border:0;display:block;width:100%;margin:10px 0;background:#da2837;border-radius:6px;padding:5px 8px;font-weight:medium;font-size:20px;color:#fef8ec;cursor:pointer}</style></head><body><main><h1>Dr. JJ Mauvais's Cookie Jar</h1><button id=\"open\">Open</button><button id=\"close\">Close</button></main><script>function send(msg){fetch('/'+msg)}document.getElementById('open').addEventListener('click',()=>send('open'));document.getElementById('close').addEventListener('click',()=>send('close'));</script></body></html>\n\n";


void setup() {
  Serial.begin(115200);

  pinMode(enin1Pin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);

	stopMotor();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  int action = 0;

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /open") >= 0) {
              Serial.println("GET /open");
              action = 1;
              client.print(textBody);

            } else if (header.indexOf("GET /close") >= 0) {
              Serial.println("GET /close");
              action = 2;
              client.print(textBody);

            } else {
              Serial.println("GET default");
            // Display the HTML web page
              client.print(htmlBody);
            }

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");

    if (action == 1)
    {
      openLid();
    } else if (action == 2)
    {
      closeLid();
    }
  }

  // // put your main code here, to run repeatedly:
  // delay(2000);
  // openLid();

  // delay(2000);
  // closeLid();
}

void stopMotor() {
	digitalWrite(in1Pin, LOW);
	digitalWrite(in2Pin, LOW);
}

void runMotor(int speed, int direction) {
  analogWrite(enin1Pin, speed);
  if (direction) {
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
  } else {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
  }
}

void openLid() {
  Serial.println("Opening Lid");
  runMotor(openSpeed, openDirection);
  delay(openRunTime);
  stopMotor();
}

void closeLid() {
  Serial.println("Opening Lid");
  runMotor(closeSpeed, closeDirection);
  delay(closeRunTime);
  stopMotor();
}