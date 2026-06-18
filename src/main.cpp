#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#define TFT_POWER_EN 15
#define TFT_BL       38

class LGFX_SPI_Config : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789  _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
public:
    LGFX_SPI_Config() {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST; 
            cfg.spi_mode = 0;
            cfg.freq_write = 80000000; 
            cfg.pin_sclk = 8;
            cfg.pin_mosi = 9;
            cfg.pin_miso = 13;
            cfg.pin_dc   = 7;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = 6;
            cfg.pin_rst          = 5;
            cfg.panel_width      = 170; 
            cfg.panel_height     = 320; 
            cfg.offset_x         = 35;  
            cfg.offset_y         = 0;
            cfg.invert           = true; 
            _panel_instance.config(cfg);
        }
        setPanel(&_panel_instance);
    }
};

LGFX_SPI_Config lcd;

// --- HARİTA MATRİSİ (1: DUVAR, 0: BOŞ YOL) ---
// 10 satır x 20 sütunluk basitleştirilmiş bir labirent
#define HARITA_SATIR 10
#define HARITA_SUTUN 20
const int KUTU_BOYUTU = 16; // Her bir kare 16x16 piksel (160x320 tam kaplar)

const uint8_t LABIRENT[HARITA_SATIR][HARITA_SUTUN] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,0,1,1,0,0,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,1,0,1,0,0,0,0,1,0,1,1,0,0,0,1},
    {1,0,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,0,1},
    {1,0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// --- OYUN BİLEŞENLERİ ---
int pac_x = 24; // Başlangıç koordinatları boş yollara göre ayarlandı
int pac_y = 24;
int pac_boyut = 6;
int pac_hiz = 2; 
int pac_yon = 0; 

int yem_x = 112;
int yem_y = 48;
int skor = 0;

int hayalet_x = 280;
int hayalet_y = 136;

unsigned long son_kare_zamani = 0;
const int KARE_SURESI = 35; 
int hayalet_sayac = 0;      

bool oyun_bitti = false;

// Verilen koordinatın duvara çarpıp çarpmadığını kontrol eden fonksiyon
bool duvarVarMi(int x, int y) {
    // Nesnenin sınırlarını kutu boyutuna bölerek matristeki indeksini buluyoruz
    int sol_sutun = (x - pac_boyut) / KUTU_BOYUTU;
    int sag_sutun = (x + pac_boyut) / KUTU_BOYUTU;
    int ust_satir = (y - pac_boyut) / KUTU_BOYUTU;
    int alt_satir = (y + pac_boyut) / KUTU_BOYUTU;

    // Harita sınırları dışı veya matriste 1 olan yer duvardır
    if (sol_sutun < 0 || sag_sutun >= HARITA_SUTUN || ust_satir < 0 || alt_satir >= HARITA_SATIR) return true;
    
    if (LABIRENT[ust_satir][sol_sutun] == 1 || LABIRENT[ust_satir][sag_sutun] == 1 ||
        LABIRENT[alt_satir][sol_sutun] == 1 || LABIRENT[alt_satir][sag_sutun] == 1) {
        return true;
    }
    return false;
}

void yeniYemUret() {
    while (true) {
        int r_satir = random(1, HARITA_SATIR - 1);
        int r_sutun = random(1, HARITA_SUTUN - 1);
        if (LABIRENT[r_satir][r_sutun] == 0) { // Sadece boş yollarda yem üret
            yem_x = (r_sutun * KUTU_BOYUTU) + (KUTU_BOYUTU / 2);
            yem_y = (r_satir * KUTU_BOYUTU) + (KUTU_BOYUTU / 2);
            break;
        }
    }
}

// Tüm harita labirentini ekrana çizen fonksiyon
void labirentiCiz() {
    for (int r = 0; r < HARITA_SATIR; r++) {
        for (int c = 0; c < HARITA_SUTUN; c++) {
            if (LABIRENT[r][c] == 1) {
                // Duvarları mavi kutu olarak çiz
                lcd.fillRect(c * KUTU_BOYUTU, r * KUTU_BOYUTU, KUTU_BOYUTU, KUTU_BOYUTU, TFT_BLUE);
                // İçine tatlı bir detay çerçeve ekle
                lcd.drawRect(c * KUTU_BOYUTU + 1, r * KUTU_BOYUTU + 1, KUTU_BOYUTU - 2, KUTU_BOYUTU - 2, 0x0010); 
            }
        }
    }
}

void oyunSifirla() {
    lcd.fillScreen(TFT_BLACK);
    labirentiCiz(); // Labirenti arkaya gömüyoruz
    
    pac_x = 24;   
    pac_y = 24;
    pac_yon = 0;  
    hayalet_x = 280; 
    hayalet_y = 136;
    skor = 0;
    hayalet_sayac = 0;
    oyun_bitti = false;
    yeniYemUret();

    // İlk çizimler
    lcd.fillCircle(yem_x, yem_y, 3, TFT_RED);
    lcd.fillCircle(pac_x, pac_y, pac_boyut, TFT_YELLOW);
    lcd.fillRect(hayalet_x - 5, hayalet_y - 5, 10, 10, TFT_CYAN);
}

void setup() {
    Serial.begin(115200);
    delay(500);

    pinMode(TFT_POWER_EN, OUTPUT);
    digitalWrite(TFT_POWER_EN, HIGH);
    delay(200);

    lcd.init();
    lcd.setRotation(1); 
    
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    oyunSifirla();
}

void loop() {
    unsigned long su_an = millis();
    if (su_an - son_kare_zamani < KARE_SURESI) return;
    son_kare_zamani = su_an;

    if (oyun_bitti) {
        if (Serial.available() > 0) {
            while(Serial.available() > 0) Serial.read(); 
            oyunSifirla();
        }
        return;
    }

    // --- 1. GİRDİ SİSTEMİ ---
    if (Serial.available() > 0) {
        char tus = Serial.read();
        if (tus == 'd' || tus == '6') pac_yon = 1; // Sağ
        if (tus == 'a' || tus == '4') pac_yon = 2; // Sol
        if (tus == 'w' || tus == '8') pac_yon = 3; // Yukarı
        if (tus == 's' || tus == '2') pac_yon = 4; // Aşağı
    }

    int eski_pac_x = pac_x;
    int eski_pac_y = pac_y;

    // --- 2. DUVAR KONTROLLÜ HAREKET MATEMATİĞİ ---
    int dene_x = pac_x;
    int dene_y = pac_y;

    if (pac_yon == 1) dene_x += pac_hiz; 
    else if (pac_yon == 2) dene_x -= pac_hiz; 
    else if (pac_yon == 3) dene_y -= pac_hiz; 
    else if (pac_yon == 4) dene_y += pac_hiz; 

    // Eğer gideceğimiz yerde duvar yoksa ilerle, varsa dur!
    if (!duvarVarMi(dene_x, dene_y)) {
        pac_x = dene_x;
        pac_y = dene_y;
    }

    // --- 3. HARİTAYA GÖRE HAYALET AI (YAVAŞLATILMIŞ) ---
    int eski_hayalet_x = hayalet_x;
    int eski_hayalet_y = hayalet_y;

    hayalet_sayac++;
    if (hayalet_sayac >= 4) { // Hayaleti labirentte iyice yavaşlattık (4 karede bir)
        hayalet_sayac = 0;
        if (pac_yon != 0) {
            int h_dene_x = hayalet_x;
            int h_dene_y = hayalet_y;

            // X ekseninde yaklaşmayı dene
            if (hayalet_x < pac_x) h_dene_x += 2;
            else if (hayalet_x > pac_x) h_dene_x -= 2;

            // Duvar yoksa X ekseninde yürü
            if (!duvarVarMi(h_dene_x, hayalet_y)) {
                hayalet_x = h_dene_x;
            }

            // Y ekseninde yaklaşmayı dene
            if (hayalet_y < pac_y) h_dene_y += 2;
            else if (hayalet_y > pac_y) h_dene_y -= 2;

            // Duvar yoksa Y ekseninde yürü
            if (!duvarVarMi(hayalet_x, h_dene_y)) {
                hayalet_y = h_dene_y;
            }
        }
    }

    // --- 4. ÇARPIŞMA TESTLERİ ---
    if (abs(pac_x - yem_x) < 8 && abs(pac_y - yem_y) < 8) {
        skor++;
        lcd.fillCircle(yem_x, yem_y, 4, TFT_BLACK); // Yemin yerini temizle
        yeniYemUret();
    }

    if (abs(pac_x - hayalet_x) < 8 && abs(pac_y - hayalet_y) < 8) {
        oyun_bitti = true;
        // Kıpkırmızı Trump Ceza Kürsüsü
        lcd.fillRect(40, 45, 240, 80, TFT_RED);
        lcd.drawRect(40, 45, 240, 80, TFT_WHITE);
        
        lcd.setTextColor(TFT_WHITE);
        lcd.setTextSize(3); // Yazıyı kocaman yaptık!
        lcd.setCursor(55, 55);  lcd.print("YOU'RE FIRED!");
        
        lcd.setTextSize(1);
        lcd.setTextColor(TFT_YELLOW); // Alt yazı sarı kafa rengi
        lcd.setCursor(50, 100); lcd.print("Yeni bir secim icin harf gonder...");
        return;
    }

    // --- 5. ULTRA AKILLI RENDER ---
    // Sadece eski konumları siyahla temizle (Mavi duvarlar asla silinmez!)
    if (eski_pac_x != pac_x || eski_pac_y != pac_y) {
        lcd.fillCircle(eski_pac_x, eski_pac_y, pac_boyut, TFT_BLACK);
    }
    if (eski_hayalet_x != hayalet_x || eski_hayalet_y != hayalet_y) {
        lcd.fillRect(eski_hayalet_x - 5, eski_hayalet_y - 5, 10, 10, TFT_BLACK);
    }

    // Yenileri bas
    lcd.fillCircle(yem_x, yem_y, 3, TFT_RED); 
    lcd.fillCircle(pac_x, pac_y, pac_boyut, TFT_YELLOW); 
    lcd.fillRect(hayalet_x - 5, hayalet_y - 5, 10, 10, TFT_CYAN); 

    // Skor Tablosu
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    lcd.setTextSize(1);
    lcd.setCursor(10, 5);
    lcd.printf("SKOR: %d", skor);
}