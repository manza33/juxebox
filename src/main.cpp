#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Buzzer.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include "led.h"
#include "music.h"

LiquidCrystal_PCF8574 lcd(0x27);
int show = -1;

#ifndef STASSID
#define STASSID "FatPanda"
#define STAPSK  "TitiBoubou"
// #define STASSID "TiLiQUA"
// #define STAPSK  "CestPlusFortQueToi"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

//Const connexion a la page
WiFiClient client;
DynamicJsonDocument doc(10000);

const int buttonInput = 12;  // D1
bool bButtonPressedEvent = false;
bool bButtonPressedDown = false;
bool bButtonPressedUp = false;
bool countButtonPressed = false;
bool isStopMusic = true;
const char* name = "";
int pause = 10;

int i = 0;
int MusiquePin = 14;
unsigned long startTime = 0;

Buzzer buzzer(MusiquePin, 16);

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

    int error;
    // We start by connecting to a WiFi network
    // Mettre GPIO (General Purpose Input Output) en OUTPUT (Sortie) : LED
    Serial.begin(115200);    

    Serial.println("Dose: check for LCD");

    // See http://playground.arduino.cc/Main/I2cScanner how to test for a I2C device.
    Wire.begin();
    Wire.beginTransmission(0x27);
    error = Wire.endTransmission();
    Serial.print("Error: ");
    Serial.print(error);

    if (error == 0)
    {
        Serial.println(": LCD found.");
        show = 0;
        lcd.begin(16, 2); // initialize the lcd
    }
    else
    {
        Serial.println(": LCD not found.");
    } // if

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
            startTime = millis();
            Serial.println("APPUI COURT!!!");
            bButtonPressedDown = false;
            countButtonPressed = true; 

            // ################## Connexion a la page  ################## 

            HTTPClient http;    //Declare object of class HTTPClient            
            
            //GET Data
            String Link = "http://www.manza3d.com/juxebox_server/";
            
            http.begin(client, Link);     //Specify request destination
            
            int httpCode = http.GET();            //Send the request
            String payload = http.getString();    //Get the response payload
            
            Serial.println("Got data:");
            Serial.println(httpCode);   //Print HTTP return code
            Serial.println(payload);    //Print request response payload
            
            http.end();  //Close connection       
            
            //  ################## Fin Connexion  ################## 

            // ### parse JSON ###

            DeserializationError error = deserializeJson(doc, payload);        

            // Test if parsing succeeds.
            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
                return;
            }
            i = 0;
        }

        if (bButtonPressedUp)
        {
            Serial.println("BUTTON RELEASED!!!");
            bButtonPressedUp = false;
            unsigned long elapseTime = millis() - startTime;
            Serial.print("ELAPSE TIME : ");
            Serial.print(elapseTime);
            Serial.println("ms");

            if (elapseTime > 1000)
            {
                Serial.println("APPUI LONG!!!");
                isStopMusic = true;
            }
            else
            {
                Serial.println("APPUI COURT!!!");
                isStopMusic = false;            
            }
            Serial.print("isStopMusic : ");
            Serial.println(isStopMusic);

        }
        bButtonPressedEvent = false;     
    }

    if(i == 0){
        pause = doc["pause"];  
        buzzer.begin(pause);
    }

    if (countButtonPressed == true && isStopMusic == false)
    {   
        name = doc["name"]; 
        int note = doc["notes"][i]["note"];  
        int tempo = doc["notes"][i]["tempo"];           
        Serial.print("name : ");
        Serial.println(name);
        Serial.print("note : ");
        Serial.println(note);
        Serial.print("tempo : ");
        Serial.println(tempo);        

        //music = rand() % 3 + 1;

        if (note != -1)
        {
            buzzer.sound(note, tempo);
            ++i;
        }
        else if(note == -1)
        {
            Serial.println("Fin");   
            i = 0;  
            buzzer.end(500);
        }      

    }

    if (show == 0)
    {
        lcd.setBacklight(255);
        lcd.home();
        lcd.clear();
        lcd.print(name);

        lcd.setBacklight(0);
        lcd.setBacklight(255);
    }
    else if (show == 1)
    {
        lcd.clear();
        lcd.print(name);
        lcd.cursor();
    }
    else if (show == 2)
    {
        lcd.clear();
        lcd.print(name);
        lcd.blink();
    }
    else if (show == 3)
    {
        lcd.clear();
        lcd.print(name);
        lcd.noBlink();
        lcd.noCursor();
    }
    else if (show == 4)
    {
        lcd.clear();
        lcd.print(name);
        lcd.noDisplay();
    }
    else if (show == 5)
    {
        lcd.clear();
        lcd.print(name);
        lcd.display();
    }
    else if (show == 7)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(name);
        lcd.setCursor(0, 1);
        lcd.print("");
    }
    else if (show == 8)
    {
        lcd.scrollDisplayLeft();
    }
    else if (show == 9)
    {
        lcd.scrollDisplayLeft();
    }
    else if (show == 10)
    {
        lcd.scrollDisplayLeft();
    }
    else if (show == 11)
    {
        lcd.scrollDisplayRight();
    }
    else if (show == 12)
    {
        lcd.clear();
        lcd.print(name);
    }
    else if (show > 12)
    {
        lcd.print(show - 13);
    } 

    show = (show + 1) % 16;
}



