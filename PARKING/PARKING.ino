
// Include Libraries
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "LiquidCrystal_PCF8574.h"

//IP address: 192.168.15.100

// Replace with your network credentials
const char* ssid = "hanturaya";
const char* password = "azreen01";
String apikey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im5hYmlsYV8xIiwicmVhZF93cml0ZSI6dHJ1ZSwiaWF0IjoxNzAwMjAyMTQ0fQ.hW9nJey3RkIS5z2Jg2H4wCrdTkjT8-VApuONaRcMN7U"; 
char server[] = "apiv2.favoriot.com";
const String device_developer_id = "assistant1_device@nabila_1";
//datatype yourData = "";

WiFiClient client;

// Pin Definitions
#define IROBJAVOID_PIN_A1 D6
#define IROBJAVOID_PIN_A2 D7
#define IROBJAVOID_PIN_B1 D5
#define IROBJAVOID_PIN_B2 D2 //but pin at D0. 
//#define IROBJAVOID_PIN_2 D7

#define LCD_ADDRESS 0x27
// Define LCD characteristics
#define LCD_ROWS 4
#define LCD_COLUMNS 20
#define SCROLL_DELAY 150
#define BACKLIGHT 255

// object initialization
LiquidCrystal_PCF8574 lcd20x4;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() 
{
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial) ; // wait for serial port to connect. Needed for native USB
    // Serial.println("start");

    // Connect to WiFi network
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Connection successful
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP address: ");
//---------------------------------------------------------------------------------
    // Initialize IR sensor pin as input
     pinMode(IROBJAVOID_PIN_A1, INPUT);
     pinMode(IROBJAVOID_PIN_A2, INPUT);
     pinMode(IROBJAVOID_PIN_B1, INPUT);
     pinMode(IROBJAVOID_PIN_B1, INPUT);
    

    // initialize the lcd
    // lcd20x4.begin(LCD_COLUMNS, LCD_ROWS, LCD_ADDRESS, BACKLIGHT); 
    lcd20x4.begin(LCD_COLUMNS, LCD_ROWS); // Initialize the LCD
    lcd20x4.setBacklight(BACKLIGHT); // Set the backlight intensity
    // menuOption = menu();
    
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() 
{
  // Read IR sensor values for Parking A
  bool obstacleDetectedA1 = !digitalRead(IROBJAVOID_PIN_A1);
  bool obstacleDetectedA2 = !digitalRead(IROBJAVOID_PIN_A2);

  // Read IR sensor values for Parking B
  bool obstacleDetectedB1 = !digitalRead(IROBJAVOID_PIN_B1);
  bool obstacleDetectedB2 = !digitalRead(IROBJAVOID_PIN_B2);

  // Count unoccupied parking spaces for Parking A
  int unoccupiedParkingA = 0;
  if (!obstacleDetectedA1) {
    unoccupiedParkingA++;
  }
  if (!obstacleDetectedA2) {
    unoccupiedParkingA++;
  }

  // Count unoccupied parking spaces for Parking B
  int unoccupiedParkingB = 0;
  if (!obstacleDetectedB1) {
    unoccupiedParkingB++;
  }
  if (!obstacleDetectedB2) {
    unoccupiedParkingB++;
  }

  // Update the LCD display with the total unoccupied parking spaces
  lcd20x4.clear();

  lcd20x4.setCursor(0, 2);
  lcd20x4.print("Parking A: ");
  lcd20x4.print(unoccupiedParkingA);

  lcd20x4.setCursor(0, 3);
  lcd20x4.print("Parking B: ");
  lcd20x4.print(unoccupiedParkingB);

  // Calculate the total unoccupied parking spaces
  int totalUnoccupiedParking = unoccupiedParkingA + unoccupiedParkingB;

  lcd20x4.setCursor(0, 0);
  lcd20x4.print("Parking available: ");
  lcd20x4.print(totalUnoccupiedParking);


  delay(SCROLL_DELAY);

  //------------JSON DATA TO SENT TO FAVORIOT---------------
   String json = "{\"device_developer_id\":\"" + device_developer_id + "\",\"data\":{";
   json += "\"Parking_A_1\":\"" + String(!obstacleDetectedA1) + "\",";
   json += "\"Parking_A_2\":\"" + String(!obstacleDetectedA2) + "\",";
   json += "\"Parking_B_1\":\"" + String(!obstacleDetectedB1) + "\",";
   json += "\"Parking_B_2\":\"" + String(!obstacleDetectedB2) + "\"}}";


    // Send the data to Favoriot
    if (client.connect(server, 80))
    {
      client.println("POST /v2/streams HTTP/1.1");
      client.println("Host: apiv2.favoriot.com");
      client.println("apikey:eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Im5hYmlsYV8xIiwicmVhZF93cml0ZSI6dHJ1ZSwiaWF0IjoxNzAwMjAyMTQ0fQ.hW9nJey3RkIS5z2Jg2H4wCrdTkjT8-VApuONaRcMN7U" );
      client.println("Content-Type: application/json");
      client.println("cache-control: no-cache");
      client.print("Content-Length: ");
      int thisLength = json.length();
      client.println(thisLength);
      client.println("Connection: close");
      client.println();
      client.println(json);
    }

    while (client.available())
    {
      char c = client.read();
      Serial.write(c);
    }

    if (!client.connected())
    {
      client.stop();
    }
  }



