#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>


#define SCREEN_WIDTH 128
#define SCREEN_HIGHT 64

#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HIGHT, &Wire, OLED_RESET);

SoftwareSerial espSerial(2, 3);

void setup() {
    Serial.begin(9600);

    Wire.begin();

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while(1);
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("READY");
    diplay.display();
}

void loop() {
    if(Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        Serial.print("Gesture: ");
        Serial.println(command);

        display.clearDisplay();

        display.setCursor(0, 20);

        display.setTextSize(2);

        display.println(command);

        display.display();
    }
}
