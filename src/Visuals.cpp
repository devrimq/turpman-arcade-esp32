#include "Visuals.h"
#include <cmath>

void vektorPacmanCiz(lgfx::LGFX_Device* ekran, int x, int y, int yon, unsigned long milis) {
    const int r = 7;
    bool agiz_acik = (milis / 150) % 2;

    ekran->fillCircle(x, y, r, TFT_YELLOW);

    if (agiz_acik) {
        if      (yon == 1 || yon == 0) ekran->fillTriangle(x, y, x + r, y - 4, x + r, y + 4, TFT_BLACK);
        else if (yon == 2)             ekran->fillTriangle(x, y, x - r, y - 4, x - r, y + 4, TFT_BLACK);
        else if (yon == 3)             ekran->fillTriangle(x, y, x - 4, y - r, x + 4, y - r, TFT_BLACK);
        else if (yon == 4)             ekran->fillTriangle(x, y, x - 4, y + r, x + 4, y + r, TFT_BLACK);
    }
}

void vektorHayaletCiz(lgfx::LGFX_Device* ekran, int x, int y, uint32_t renk, int yon, unsigned long milis) {
    const int r = 6;

    ekran->fillCircle(x, y - 2, r, renk);
    ekran->fillRect(x - r, y - 2, (r * 2) + 1, 6, renk);

    // Etek animasyonu
    int kayma = (milis / 200) % 2;
    if (kayma == 0) {
        ekran->fillTriangle(x - 6, y + 4, x - 3, y + 4, x - 4, y + 7, renk);
        ekran->fillTriangle(x - 2, y + 4, x + 1, y + 4, x,     y + 7, renk);
        ekran->fillTriangle(x + 2, y + 4, x + 5, y + 4, x + 3, y + 7, renk);
    } else {
        ekran->fillTriangle(x - 5, y + 4, x - 2, y + 4, x - 3, y + 7, renk);
        ekran->fillTriangle(x - 1, y + 4, x + 2, y + 4, x,     y + 7, renk);
        ekran->fillRect(x + 3, y + 4, 3, 3, renk);
    }

    // Göz yönü kayması
    int gx = 0, gy = 0;
    if      (yon == 1) gx =  1;
    else if (yon == 2) gx = -1;
    else if (yon == 3) gy = -1;
    else if (yon == 4) gy =  1;

    ekran->fillCircle(x - 3 + gx,      y - 2 + gy,      2, TFT_WHITE);
    ekran->fillCircle(x - 3 + gx * 2,  y - 2 + gy * 2,  1, TFT_BLUE);
    ekran->fillCircle(x + 3 + gx,      y - 2 + gy,      2, TFT_WHITE);
    ekran->fillCircle(x + 3 + gx * 2,  y - 2 + gy * 2,  1, TFT_BLUE);
}

// HUD: "SKOR: 120  TOPLAM: 450  |  MARMARA - ISKENDER"
// toplam == 0 ise toplam kısmı gizlenir (ilk bölge)
void hudCiz(lgfx::LGFX_Device* ekran, int skor, int toplam,
            const char* bolge_adi, const char* lezzet) {
    ekran->fillRect(0, 160, EKRAN_GENISLIK, 10, TFT_BLACK);
    ekran->drawFastHLine(0, 160, EKRAN_GENISLIK, 0x5AEB);
    ekran->setTextColor(TFT_WHITE, TFT_BLACK);
    ekran->setTextSize(1);
    ekran->setCursor(4, 162);

    if (toplam > 0) {
        ekran->printf("SKOR:%d TOT:%d | %s-%s", skor, toplam, bolge_adi, lezzet);
    } else {
        ekran->printf("SKOR:%d | %s - %s", skor, bolge_adi, lezzet);
    }
}
