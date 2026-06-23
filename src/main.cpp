#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "Config.h"
#include "Map.h"
#include "Player.h"
#include "Ghost.h"
#include "Controller.h"
#include "Sound.h"
#include "Visuals.h"

// ============================================================
// LCD KURULUMU (ESP32-S3 + ST7789 320x170)
// ============================================================
class LGFX_SPI_Config : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI      _bus_instance;
public:
    LGFX_SPI_Config() {
        {
            auto cfg      = _bus_instance.config();
            cfg.spi_host  = SPI2_HOST;
            cfg.spi_mode  = 0;
            cfg.freq_write= 80000000;
            cfg.pin_sclk  = 8;
            cfg.pin_mosi  = 9;
            cfg.pin_miso  = 13;
            cfg.pin_dc    = 7;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg        = _panel_instance.config();
            cfg.pin_cs      = 6;
            cfg.pin_rst     = 5;
            cfg.panel_width = 170;
            cfg.panel_height= 320;
            cfg.offset_x    = 35;
            cfg.offset_y    = 0;
            cfg.invert      = true;
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};
LGFX_SPI_Config lcd;

// ============================================================
// OYUN DEĞİŞKENLERİ
// ============================================================
OyunDurumu oyun_durumu      = OYNUYOR;
unsigned long durum_zamani  = 0;        // Durum geçiş zaman damgası
unsigned long son_kare_zamani = 0;

int skor       = 0;   // Bu bölgenin skoru
int toplam_skor = 0;  // Tüm bölgeler toplamı

int eski_pac_x, eski_pac_y;
bool mavi_mod = false;
unsigned long mavi_mod_baslangic = 0;

int  aktif_hayalet_sayisi = 1;
bool yem_yendi[HARITA_SATIR][HARITA_SUTUN];

// ============================================================
// YARDIMCI FONKSİYONLAR
// ============================================================
int haritadakiToplamYemiSay() {
    int toplam = 0;
    for (int r = 0; r < HARITA_SATIR; r++)
        for (int c = 0; c < HARITA_SUTUN; c++)
           if (pgm_read_byte(&(aktif_harita[r][c])) == 0) toplam++; // PROGMEM Güncellemesi[cite: 10]
    return toplam;
}

int yenenYemSayisi() {
    int sayac = 0;
    for (int r = 0; r < HARITA_SATIR; r++)
        for (int c = 0; c < HARITA_SUTUN; c++)
            if (yem_yendi[r][c]) sayac++;
    return sayac;
}

// ============================================================
// OYUN SIFIRLAMA
// haritaGuncelle() açıkça çağrılarak harita + spawn güncellenir,
// ardından haritayiCiz() bağımsız olarak çalışır.
// ============================================================
void oyunSifirla() {
    memset(yem_yendi, 0, sizeof(yem_yendi));

    // Önce haritayı ve spawn noktalarını güncelle
    haritaGuncelle();

    lcd.fillScreen(TFT_BLACK);
    haritayiCiz(&lcd);

    aktif_hayalet_sayisi = 1;
    mavi_mod = false;
    pac_x = 24; pac_y = 24;
    pac_yon = 0; mevcut_yon = 0;
    eski_pac_x = pac_x; eski_pac_y = pac_y;

    vektorPacmanCiz(&lcd, pac_x, pac_y, pac_yon, millis());

    // Hayaletleri güncellenen spawn noktasından başlat
    hayaletleriSifirla();
    for (int i = 0; i < aktif_hayalet_sayisi; i++)
        vektorHayaletCiz(&lcd, hayaletler[i].x, hayaletler[i].y,
                         hayaletler[i].orijinal_renk, 0, millis());

    oyun_durumu = OYNUYOR;
    hudCiz(&lcd, skor, toplam_skor,
           anadolu_turu[aktif_bolge_indeksi].bolge_adi,
           anadolu_turu[aktif_bolge_indeksi].efsane_lezzet);
}

// ============================================================
// SETUP
// ============================================================
void setup() {
    Serial.begin(115200);
    pinMode(TFT_POWER_EN, OUTPUT); digitalWrite(TFT_POWER_EN, HIGH);
    pinMode(TFT_BL, OUTPUT);       digitalWrite(TFT_BL, HIGH);
    delay(100);
    lcd.init();
    lcd.setRotation(1);
    controllerKurulum();
    soundKurulum();
    oyunSifirla();
}

// ============================================================
// ANA DÖNGÜ
// ============================================================
void loop() {
    unsigned long su_an = millis();
    if (su_an - son_kare_zamani < (unsigned long)KARE_SURESI) return;
    son_kare_zamani = su_an;

    // --- DURUM MAKİNESİ ---

    // ÖLÜM: OLDU_BEKLEME ms geçtikten sonra sahneyi sıfırla
    if (oyun_durumu == OLDU_ANIMASYON) {
        if (su_an - durum_zamani >= OLDU_BEKLEME) {
            oyunSifirla();
        }
        return;
    }

    // BÖLGE TAMAMLANDI: Ekran gösterildi, süre dolunca sonraki bölgeye geç
    if (oyun_durumu == BOLGE_TAMAMLANDI) {
        if (su_an - durum_zamani >= BOLGE_BEKLEME) {
            toplam_skor += skor;
            skor = 0;
            aktif_bolge_indeksi = (aktif_bolge_indeksi + 1) % TOPLAM_BOLGE;
            oyunSifirla();
        }
        return;
    }

    // ============================================================
    // OYNUYOR DURUMU
    // ============================================================
    girdileriAl();
    playerHareketSistemi();
    hayaletYapayZeka(mavi_mod);

    // Mavi mod zaman aşımı
    if (mavi_mod) {
        unsigned long gecen = su_an - mavi_mod_baslangic;
        if (gecen > MAVI_MOD_SURESI) {
            mavi_mod = false;
            // Hayaletlerin anında orijinal renge dönmesi için render zorla
            for (int i = 0; i < aktif_hayalet_sayisi; i++)
                hayaletler[i].eski_x = hayaletler[i].x - 1;
        }
    }

    // İkinci hayalet aktivasyonu
    if (!mavi_mod &&
        skor >= (anadolu_turu[aktif_bolge_indeksi].patron_skoru / 2) &&
        aktif_hayalet_sayisi == 1) {
        aktif_hayalet_sayisi = 2;
        tone(BUZZER_PIN, 600, 200);
    }

    // --- ÇARPIŞMA KONTROLÜ ---
    for (int i = 0; i < aktif_hayalet_sayisi; i++) {
        if (abs(pac_x - hayaletler[i].x) < 8 && abs(pac_y - hayaletler[i].y) < 8) {
            if (mavi_mod) {
                // Hayaleti ye
                tone(BUZZER_PIN, 950, 150);
                hayaletler[i].x = hayalet_spawn[i].x;
                hayaletler[i].y = hayalet_spawn[i].y;
                skor += 100;
                hudCiz(&lcd, skor, toplam_skor,
                       anadolu_turu[aktif_bolge_indeksi].bolge_adi,
                       anadolu_turu[aktif_bolge_indeksi].efsane_lezzet);
            } else {
                // Öldü: non-blocking, state machine devralır
                olduSesiCal();
                oyun_durumu  = OLDU_ANIMASYON;
                durum_zamani = su_an;
                return;
            }
        }
    }

    // --- YEM KONTROLÜ ---
    int pac_kutu_c = pac_x / KUTU_BOYUTU;
    int pac_kutu_r = pac_y / KUTU_BOYUTU;

    if (pac_kutu_r >= 0 && pac_kutu_r < HARITA_SATIR &&
        pac_kutu_c >= 0 && pac_kutu_c < HARITA_SUTUN)
    {
        if (aktif_harita[pac_kutu_r][pac_kutu_c] == 0 &&
            !yem_yendi[pac_kutu_r][pac_kutu_c])
        {
            yem_yendi[pac_kutu_r][pac_kutu_c] = true;

            bool guclendiriciyem = (pac_kutu_r == 1 && pac_kutu_c == 1)  ||
                                   (pac_kutu_r == 1 && pac_kutu_c == 18) ||
                                   (pac_kutu_r == 8 && pac_kutu_c == 1)  ||
                                   (pac_kutu_r == 8 && pac_kutu_c == 18);
            if (guclendiriciyem) {
                skor += 50;
                mavi_mod = true;
                mavi_mod_baslangic = su_an;
                tone(BUZZER_PIN, 880, 300);
            } else {
                skor += 10;
                wakaSesiCikar();
            }

            lcd.fillRect(pac_kutu_c * KUTU_BOYUTU + 6,
                         pac_kutu_r * KUTU_BOYUTU + 6, 4, 4, TFT_BLACK);
            hudCiz(&lcd, skor, toplam_skor,
                   anadolu_turu[aktif_bolge_indeksi].bolge_adi,
                   anadolu_turu[aktif_bolge_indeksi].efsane_lezzet);

            // --- SEVİYE BİTİŞ ŞARTI ---
            if (yenenYemSayisi() >= haritadakiToplamYemiSay()) {
                kazandiSesiCal();

                lcd.fillRect(0, 0, EKRAN_GENISLIK, OYUN_ALANI_Y, TFT_BLACK);
                lcd.setTextSize(2);

                if (skor >= anadolu_turu[aktif_bolge_indeksi].patron_skoru) {
                    lcd.setTextColor(TFT_GOLD);
                    lcd.setCursor(30, 45);
                    lcd.printf("GURME USTASI!");
                } else {
                    lcd.setTextColor(TFT_GREEN);
                    lcd.setCursor(40, 45);
                    lcd.printf("BOLGE TEMIZ!");
                }

                // Düzeltme: setTextSize integer alır; 1.5 → 1
                lcd.setTextSize(1);
                lcd.setTextColor(TFT_WHITE, TFT_BLACK);
                lcd.setCursor(40, 85);
                lcd.printf("%s TAMAMLANDI!", anadolu_turu[aktif_bolge_indeksi].bolge_adi);
                lcd.setCursor(40, 100);
                lcd.printf("TOPLAM SKOR: %d", toplam_skor + skor);

                oyun_durumu  = BOLGE_TAMAMLANDI;
                durum_zamani = su_an;
                return;
            }
        }
    }

    // ============================================================
    // RENDER SİSTEMİ
    // ============================================================

    // Mavi mod uyarısı: son MAVI_UYARI_SURESI ms'de yanıp sönme
    bool mavi_uyari = mavi_mod &&
        (su_an - mavi_mod_baslangic > MAVI_MOD_SURESI - MAVI_UYARI_SURESI);

    // Pacman render
    if (eski_pac_x != pac_x || eski_pac_y != pac_y) {
        lcd.fillCircle(eski_pac_x, eski_pac_y, PAC_BOYUT + 1, TFT_BLACK);

        // Silinen alandaki duvarları yeniden çiz
        int p_c = eski_pac_x / KUTU_BOYUTU;
        int p_r = eski_pac_y / KUTU_BOYUTU;
        for (int rd = -1; rd <= 1; rd++) {
            for (int cd = -1; cd <= 1; cd++) {
                int r = p_r + rd, c = p_c + cd;
                if (r < 0 || r >= HARITA_SATIR || c < 0 || c >= HARITA_SUTUN) continue;
                if (pgm_read_byte(&(aktif_harita[r][c])) == 1) { // PROGMEM Güncellemesi[cite: 10]
                    lcd.fillRect(c * KUTU_BOYUTU, r * KUTU_BOYUTU, KUTU_BOYUTU, KUTU_BOYUTU, TFT_BLACK); //[cite: 10]
                    lcd.drawRect(c * KUTU_BOYUTU, r * KUTU_BOYUTU, KUTU_BOYUTU, KUTU_BOYUTU, anadolu_turu[aktif_bolge_indeksi].duvar_rengi); //[cite: 10]
                }
            }
        }
        vektorPacmanCiz(&lcd, pac_x, pac_y, mevcut_yon, su_an);
        eski_pac_x = pac_x; eski_pac_y = pac_y;
    }

    // Hayalet render
    for (int i = 0; i < aktif_hayalet_sayisi; i++) {
        if (hayaletler[i].eski_x == hayaletler[i].x &&
            hayaletler[i].eski_y == hayaletler[i].y) continue;

        // Ghost sprite: x+-6 genislik, y-8'den y+7'ye yukseklik - guvenli margin ile temizle
        lcd.fillRect(hayaletler[i].eski_x - 9, hayaletler[i].eski_y - 10, 19, 20, TFT_BLACK);

        // Eski alandaki duvar/yem restore
        int eski_c = hayaletler[i].eski_x / KUTU_BOYUTU;
        int eski_r = hayaletler[i].eski_y / KUTU_BOYUTU;
        for (int rd = -1; rd <= 1; rd++) {
            for (int cd = -1; cd <= 1; cd++) {
                int r = eski_r + rd, c = eski_c + cd;
                if (r < 0 || r >= HARITA_SATIR || c < 0 || c >= HARITA_SUTUN) continue;
                if (pgm_read_byte(&(aktif_harita[r][c])) == 1) { // PROGMEM Güncellemesi[cite: 10]
                    lcd.fillRect(c * KUTU_BOYUTU, r * KUTU_BOYUTU, KUTU_BOYUTU, KUTU_BOYUTU, TFT_BLACK); //[cite: 10]
                    lcd.drawRect(c * KUTU_BOYUTU, r * KUTU_BOYUTU, KUTU_BOYUTU, KUTU_BOYUTU, anadolu_turu[aktif_bolge_indeksi].duvar_rengi); //[cite: 10]
                } else {
                    if (yem_yendi[r][c]) {
                        lcd.fillRect(c * KUTU_BOYUTU, r * KUTU_BOYUTU,
                                     KUTU_BOYUTU, KUTU_BOYUTU, TFT_BLACK);
                    } else {
                        bool guclu = (r == 1 && c == 1)  || (r == 1 && c == 18) ||
                                     (r == 8 && c == 1)  || (r == 8 && c == 18);
                        if (guclu)
                            lcd.fillCircle(c * KUTU_BOYUTU + 8, r * KUTU_BOYUTU + 8, 5, TFT_GOLD);
                        else
                            lcd.fillRect(c * KUTU_BOYUTU + 6, r * KUTU_BOYUTU + 6, 4, 4, TFT_YELLOW);
                    }
                }
            }
        }

        // Mavi mod uyarısında hayalet beyaz-mavi arasında yanıp söner
        uint32_t renk;
        if (mavi_mod) {
            if (mavi_uyari)
                renk = (su_an / 250) % 2 ? TFT_WHITE : TFT_BLUE;
            else
                renk = TFT_BLUE;
        } else {
            renk = hayaletler[i].orijinal_renk;
        }

        vektorHayaletCiz(&lcd, hayaletler[i].x, hayaletler[i].y, renk, hayaletler[i].mevcut_yon, su_an);
        hayaletler[i].eski_x = hayaletler[i].x;
        hayaletler[i].eski_y = hayaletler[i].y;
    }
}