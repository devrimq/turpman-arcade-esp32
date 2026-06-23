#pragma once
#include <Arduino.h>

// --- DONANIM PIN TANIMLARI ---
#define TFT_POWER_EN  15
#define TFT_BL        38
#define JOY_X         10
#define JOY_Y         11
#define JOY_SW        12
#define BUZZER_PIN    17

// --- EKRAN ---
const int EKRAN_GENISLIK  = 320;
const int EKRAN_YUKSEKLIK = 170;
const int OYUN_ALANI_Y    = 160; // HUD'un başladığı Y koordinatı

// --- HARİTA ---
#define HARITA_SATIR  10
#define HARITA_SUTUN  20
const int KUTU_BOYUTU = 16;

// --- ZAMANLAMA ---
const int           KARE_SURESI        = 33;    // ~30 FPS
const unsigned long MAVI_MOD_SURESI    = 6000;
const unsigned long MAVI_UYARI_SURESI  = 1500;  // Son X ms'de hayalet yanıp söner
const unsigned long OLDU_BEKLEME       = 800;   // Ölüm animasyonu süresi (delay yok)
const unsigned long BOLGE_BEKLEME      = 2500;  // Bölge tamamlandı ekranı süresi

// --- OYUN DURUM MAKİNESİ ---
enum OyunDurumu {
    OYNUYOR,
    OLDU_ANIMASYON,
    BOLGE_TAMAMLANDI
};
