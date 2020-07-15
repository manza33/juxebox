#include "led.h"

uint8_t val_led = 0;

void clignoter_led(void) {
    val_led = !val_led;
    digitalWrite(16, val_led);

    Serial.print("LED :");
    Serial.println(val_led);
}