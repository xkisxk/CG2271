// Load Wi-Fi library
#include <WiFi.h>

#define RXD2 16
#define TXD2 17

const char* ssid = "ESP32-Wifi";
const char* password = "1234567890";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String response, ip_address;

// Auxiliar variables to store the current output state
String output26State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
int wait30 = 30000; // time to reconnect when connection is lost.

IPAddress local_IP(192, 168, 4, 100);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);

  //Configure Static IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("Static IP failed to configure");
  }

  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  ip_address = myIP.toString();
  Serial.println(myIP);
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP().toString());
  server.begin();

  Serial.println("Server started");
}

void loop() {
  // Check if a client has connected..
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.print("New client: ");
  Serial.println(client.remoteIP());

  // Espera hasta que el cliente envíe datos.
  // while(!client.available()){ delay(1); }

  /////////////////////////////////////////////////////
  // Read the information sent by the client.
  String req = client.readStringUntil('\r');

  // Make the client's request.
  //http://{ip_address}/{command}
  if (req.indexOf("status") != -1)
  {
    response = "WiFi Connected: " + ip_address;
    Serial2.write(0x36);
  }
  if (req.indexOf("endChallenge") != -1)
  {
    response = "Challenge Ended!";
    Serial2.write(0x37);
  }
  if (req.indexOf("onRed") != -1)
  {
    digitalWrite(output26, HIGH);
    response = "RED LED ON";
    Serial2.write(0x31);
  }
  if (req.indexOf("offRed") != -1)
  {
    digitalWrite(output26, LOW);
    response = "RED LED OFF";
    Serial2.write(0x30);
  }
  if (req.indexOf("onGreen") != -1)
  {
    digitalWrite(output26, HIGH);
    response = "GREEN LED ON";
    Serial2.write(0x33);
  }
  if (req.indexOf("offGreen") != -1)
  {
    digitalWrite(output26, LOW);
    response = "GREEN LED OFF";
    Serial2.write(0x32);
  }
  if (req.indexOf("onBlue") != -1)
  {
    digitalWrite(output26, HIGH);
    response = "BLUE LED ON";
    Serial2.write(0x35);
  }
  if (req.indexOf("offBlue") != -1)
  {
    digitalWrite(output26, LOW);
    response = "BLUE LED OFF";
    Serial2.write(0x34);
  }
  if (req.indexOf("moveForward") != -1)
  {
    response = "MOVING FORWARD";
    Serial2.write(0x40); //0b0100 0000
  }
  if (req.indexOf("turnLeft") != -1)
  {
    response = "TURNING LEFT";
    Serial2.write(0x41); //0b0100 0001
  }
  if (req.indexOf("turnRight") != -1)
  {
    response = "TURNING RIGHT";
    Serial2.write(0x42); //0b0100 0010
  }
  if (req.indexOf("moveBackward") != -1)
  {
    response = "MOVING BACKWARD";
    Serial2.write(0x43); //0b0100 0011
  }
  if (req.indexOf("forwardLeft") != -1)
  {
    response = "MOVING FORWARD AND LEFT";
    Serial2.write(0x44); //0b0100 0011
  }
  if (req.indexOf("forwardRight") != -1)
  {
    response = "MOVING FORWARD AND RIGHT";
    Serial2.write(0x45); //0b0100 0011
  }
  if (req.indexOf("reverseLeft") != -1)
  {
    response = "MOVING BACKWARD AND LEFT";
    Serial2.write(0x46); //0b0100 0011
  }
  if (req.indexOf("reverseRight") != -1)
  {
    response = "MOVING BACKWARD AND RIGHT";
    Serial2.write(0x47); //0b0100 0011
  }
  if (req.indexOf("stop") != -1)
  {
    response = "STOP";
    Serial2.write(0x48);
  }
  if (req.indexOf("selfDriveMode") != -1)
  {
    response = "Self Drive Mode";
    Serial2.write(0x49);
  }
  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println(response); //  Return status.

  client.flush();
  client.stop();
  Serial.println("Client disconnected.");
}
