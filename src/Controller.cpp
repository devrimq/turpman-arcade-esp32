#include "Controller.h"
#include <Arduino.h>

void controllerKurulum() {
    // Analog pinler için pinMode tanımına gerek yoktur.
    // Sadece joystick'in üstüne basma butonu için PULLUP yapıyoruz:
    pinMode(JOY_SW, INPUT_PULLUP);
}

void girdileriAl() {
    // ESP32'de analogRead 0 ile 4095 arası değer döndürür.
    // Joystick ortadayken değerler yaklaşık 2000-2100 civarındadır.
    int x_deger = analogRead(JOY_X);
    int y_deger = analogRead(JOY_Y);

    // X Ekseni (Sağ - Sol)
    // Eşik değerlerini (1000 ve 3000) joystick'i hafif oynattığında algılamaması için (deadzone) koyduk.
    if (x_deger < 1000) {
        pac_yon = 2; // Sol
    } 
    else if (x_deger > 3000) {
        pac_yon = 1; // Sağ
    }

    // Y Ekseni (Yukarı - Aşağı)
    if (y_deger < 1000) {
        pac_yon = 3; // Yukarı
    } 
    else if (y_deger > 3000) {
        pac_yon = 4; // Aşağı
    }
}