#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "led.h"
#include "music.h"
//using namespace std;

#ifndef STASSID
#define STASSID "FatPanda"
#define STAPSK  "TitiBoubou"
// #define STASSID "TiLiQUA"
// #define STAPSK  "CestPlusFortQueToi"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

//Const connexion a la page
const char* host = "www.manza3d.com";
const uint16_t port = 80;
WiFiClient client;
DynamicJsonDocument doc(1024);

const int buttonInput = 5;  // D1
bool bButtonPressedEvent = false;
bool bButtonPressedDown = false;
bool bButtonPressedUp = false;
bool countButtonPressed = false;

int i = 0;
int MusiquePin = 14;

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
            countButtonPressed = true; 

            // ################## Connexion a la page  ################## 

            if (client.connect(host, 80)) {  //starts client connection, checks for connection
                Serial.println("connected");
                client.println("GET /juxebox_server/ HTTP/1.1"); //download text
                client.println("Host: www.manza3d.com");
                client.println("Connection: close");  //close 1.1 persistent connection 
                client.println(); //end of get request
            }
            else{
                Serial.println("connection failed");
            }

            // Read all the lines of the reply from server and print them to Serial
            Serial.println("receiving from remote server");
            // not testing 'client.connected()' since we do not need to send data here

            String json = "";
            boolean httpBody = false;
            while (client.connected()) {
                String line = client.readStringUntil('\r');
                if (!httpBody && line.charAt(1) == '{') {
                    httpBody = true;
                }
                if (httpBody) {
                    json += line;
                }
            }
            //StaticJsonBuffer<400> jsonBuffer;
            Serial.println("Got data:");
            Serial.println(json);

            
            // Close the connexion
            Serial.println();
            Serial.println("closing connection");
            client.stop();  

            //  ################## Fin Connexion  ################## 

            // ### parse JSON ###

            DeserializationError error = deserializeJson(doc, json);        

            // Test if parsing succeeds.
            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
                return;
            }

            i = 0;
        }

        bButtonPressedEvent = false;     
    }


    if (countButtonPressed == true)
    {   
        const char* name = doc["name"]; 
        int note = doc["notes"][i]["note"];  
        int tempo = doc["notes"][i]["tempo"];      
        Serial.print("name : ");
        Serial.println(name);
        Serial.print("note : ");
        Serial.println(note);
        Serial.print("tempo : ");
        Serial.println(tempo);        

        //music = rand() % 3 + 1;

        if (note != 0)
        {
            //On joue la note
            tone(MusiquePin, note, tempo);
            //On attend X millisecondes (durée de la note) avant de passer à la suivante
            delay(tempo);
            //On arrête la lecture de la note
            noTone(MusiquePin);
            //On marque une courte pose (entre chaque note, pour les différencer)
            delay(50);
            //On passe à la note suivante
            ++i;
        }
        else
        {
            i = 0;
        }
    }

}



