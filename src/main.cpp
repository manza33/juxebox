#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "led.h"
#include "music.h"

#ifndef STASSID
#define STASSID "FatPanda"
#define STAPSK  "TitiBoubou"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

int music = 0;

const char* host = "http://www.manza3d.com/juxebox_server";
const uint16_t port = 80;
WiFiClient client;

const int buttonInput = 5;  // D1
bool bButtonPressedEvent = false;
bool bButtonPressedDown = false;
bool bButtonPressedUp = false;

// Checks if motion was detected, sets LED HIGH and starts a timer
ICACHE_RAM_ATTR void buttonPressed() {
    bButtonPressedEvent = true;
    if (digitalRead(buttonInput)) {
        bButtonPressedDown = true;
    } else {
        bButtonPressedUp = true;
    }
}

void setup() {


// We start by connecting to a WiFi network

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Mettre GPIO (General Purpose Input Output) en INPUT (Entrée) : Bouton
    pinMode(buttonInput, INPUT);
    attachInterrupt(digitalPinToInterrupt(buttonInput), buttonPressed, CHANGE);

    // Mettre GPIO (General Purpose Input Output) en OUTPUT (Sortie) : LED
    Serial.begin(115200);

    
}

unsigned long lastMsg = 0;
unsigned long startTime = 0;
uint8_t appuiSuccessif = 0;
unsigned long elapseTimeAfterManyClick = 0;

void loop() {
  
    
    unsigned long now = millis();
    if (now - lastMsg > 1000) {
      lastMsg = now;
      Serial.println("Loop...");
    }

    if (bButtonPressedEvent) {
        
        if (bButtonPressedDown) {
            
            Serial.println("APPUI COURT!!!");
            bButtonPressedDown = false;

            // if (!client.connect(host, port)) {
            //     Serial.println("connection failed");
            //     delay(5000);
            //     return;
            // }

            if (client.connect(host, 80)) {  //starts client connection, checks for connection
                Serial.println("connected");
                client.println("GET /juxebox_server/ HTTP/1.1"); //download text
                client.println("Host: www.manza3d.com");
                client.println("Connection: close");  //close 1.1 persistent connection 
                client.println(); //end of get request
            }

            //GET /juxebox_server/ HTTP/1.1
            //Host: www.manza3d.com

            // This will send a string to the server
            // Serial.println("sending data to server");
            // if (client.connected()) {
            //     client.println("hello from ESP8266");
            // }

              // wait for data to be available
            // unsigned long timeout = millis();
            // while (client.available() == 0) {
            //     //Serial.println("Timeout loop");

            //     if (millis() - timeout > 5000) {
            //     Serial.println(">>> Client Timeout !");
            //     client.stop();
            //     delay(60000);
            //     return;
            //     }
            // }

            // Read all the lines of the reply from server and print them to Serial
            Serial.println("receiving from remote server");
            // not testing 'client.connected()' since we do not need to send data here
            while (client.available()) {
                char music = static_cast<char>(client.read());
                Serial.print(music);
            }

            // Close the connection
            Serial.println();
            Serial.println("closing connection");
            client.stop();

            // if (client.available()) {
            //     Serial.println("Client reader");

            //     char c = client.read();
            //     Serial.print("num_music : ");
            //     Serial.println(c);
            //     // Do something with data ...
            // }   

            //int music = rand() % 3 + 1;

            Serial.print("Random music : ");
            Serial.println(music);

            switch (music) {
                case 1:
                    Serial.println("Fonction 1");
                    PlayMario();
                    break;
                case 2:
                    Serial.println("Fonction 2");
                    PlayStarwars();
                    break;
                case 3:
                    Serial.println("Fonction 3");
                    break;
                default:
                    break;
                
            }
        }
     
    }
}



