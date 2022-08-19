#include "MAX30100_PulseOximeter.h"
#include <Wire.h>
#include <SoftwareSerial.h>

#define REPORTING_PERIOD_MS     500

int overrideButton = 3;
bool butPressed = false;
SoftwareSerial hc08(10, 11);
PulseOximeter pox;

uint32_t tsLastReport = 0;

void onBeatDetected()
{
    Serial.println("beat");
}

void setup() {
    Wire.begin();
    pinMode(overrideButton, INPUT);

    Serial.begin(9600);
    hc08.begin(9600);
    Serial.println("START");

    delay(250);
    hc08.write("AT+ROLE=S");
    delay(250);
    hc08.write("AT+CONT=1"); // broadcast mode
    delay(250);
    hc08.write("AT+AVDA=OFF");
    delay(250);
    hc08.write("AT+AINT=800");
    delay(250);

    while (true) {
        if (pox.begin()) break;
        delay(1000);
    }
    Serial.println("Initialized pulse oximeter");

    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void delayWithPoxUpdate(int target_wait) {
    for (long i = 0; i < target_wait/5; i++) {
        pox.update();
        delay(1);
    }
}
int cnt = 0;
void loop() {
    pox.update();

    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        int hr = pox.getHeartRate();
        int spo2 = pox.getSpO2();
        Serial.println(spo2);

        if (spo2 == 0) {
          cnt = 0;
        }

        if (butPressed == HIGH || (spo2 < 94 && spo2 != 0)) {
          cnt++;
          Serial.println(cnt);
        }

        if (cnt > 11) {
          cnt = 0;
            Serial.println("ON");
            hc08.write("AT+AVDA=ON\n");
            delayWithPoxUpdate(10 * 1000);
            hc08.write("AT+AVDA=OFF\n");
            delayWithPoxUpdate(250);
        }

        tsLastReport = millis();
    }

    butPressed = digitalRead(overrideButton) == HIGH;
}
