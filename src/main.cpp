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

const char* host = "djxmmx.net";
const uint16_t port = 17;



const int buttonInput = 5;  // D1
//const int ledOutput = 16;   // D0
bool bButtonPressedEvent = false;
bool bButtonPressedDown = false;
bool bButtonPressedUp = false;
const int APPUI_MAX = 3; // Triple clic c'est déjà pas mal, non ?

// Buzzer buzzer(14, 16);

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
            Serial.println("BUTTON PRESSED!!!");
            clignoter_led();
            bButtonPressedDown = false;
            startTime = millis();

        }

        if (bButtonPressedUp) {
            Serial.println("BUTTON RELEASED!!!");
            bButtonPressedUp = false;
            unsigned long elapseTime = millis() - startTime;
            Serial.print("ELAPSE TIME : ");
            Serial.print(elapseTime);
            Serial.println("ms");

            if (elapseTime > 2000) {
                Serial.println("APPUI LONG!!!");
                appuiSuccessif = 0;
            } else {
                Serial.println("APPUI COURT!!!");

                WiFiClient client;
                String url = "http://www.manza3d.com/juxebox_server/";
                client.print(String("GET /") + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Connection: close\r\n" +
                    "\r\n"
                );

                //if (client.available()) {
                    char c = client.read();
                    Serial.print("num_music : ");
                    Serial.println(c);
                    // Do something with data ...
                //}   

                if (elapseTime < 100) {

                    // Clic ultra court
                    if (appuiSuccessif != APPUI_MAX) {
                        appuiSuccessif++;
                        elapseTimeAfterManyClick = millis();
                    }
                }
            }
        }
    }

    if (appuiSuccessif > 0 && millis() - elapseTimeAfterManyClick > 300) {
        Serial.print("YOU DID A ");
        Serial.print(appuiSuccessif);
        Serial.println("-CLICK");

        switch (appuiSuccessif) {
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

        appuiSuccessif = 0;

    }
}



