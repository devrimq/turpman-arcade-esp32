#include "Sound.h"
#include <Arduino.h>

void soundKurulum() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

void wakaSesiCikar() {
    tone(BUZZER_PIN, 440, 50);
}

// Ölüm sesi: non-blocking (150ms tek ton)
void olduSesiCal() {
    tone(BUZZER_PIN, 150, 150);
}

// Kazanma melodisi: BOLGE_TAMAMLANDI state'e girildiğinde bir kez çağrılır.
// Bu noktada oyun loop'u askıya alınmış durumda olduğundan delay() kabul edilebilir.
void kazandiSesiCal() {
    tone(BUZZER_PIN, 523, 100); delay(110);
    tone(BUZZER_PIN, 659, 100); delay(110);
    tone(BUZZER_PIN, 784, 150); delay(160);
    tone(BUZZER_PIN, 1046, 300);
}
