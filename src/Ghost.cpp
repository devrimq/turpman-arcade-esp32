#include "Ghost.h"
#include <Arduino.h>

// Başlangıç yönleri eklendi (0 yerine mantıklı yönler)
Hayalet hayaletler[2] = {
    {280, 24,  280, 24,  0xF800, 0, 0, 2}, // Blinky - Sola bakarak başlar[cite: 9]
    {280, 136, 280, 136, 0xFE19, 0, 1, 2}  // Pinky  - Sola bakarak başlar[cite: 9]
};

void hayaletleriSifirla() { //[cite: 9]
    hayaletler[0] = {hayalet_spawn[0].x, hayalet_spawn[0].y, hayalet_spawn[0].x, hayalet_spawn[0].y, 0xF800, 0, 0, 2}; //[cite: 9]
    hayaletler[1] = {hayalet_spawn[1].x, hayalet_spawn[1].y, hayalet_spawn[1].x, hayalet_spawn[1].y, 0xFE19, 0, 1, 2}; //[cite: 9]
}

void hayaletYapayZeka(bool mavi_mod) { //[cite: 9]
    // Yön tanımları koordinat değişim matrisleri (Endeksler: 1=Sağ, 2=Sol, 3=Yukarı, 4=Aşağı)
    const int dx[] = {0, 2, -2, 0, 0};
    const int dy[] = {0, 0, 0, -2, 2};
    // Bir yönün tam tersini veren yardımcı dizi (Titreme/U dönüşü engellemek için)
    const int ters_yon[] = {0, 2, 1, 4, 3}; 

    for (int i = 0; i < 2; i++) { //[cite: 9]
        hayaletler[i].hiz_sayac++; //[cite: 9]
        int tetikleme = mavi_mod ? 7 : 4; //[cite: 9]

        if (hayaletler[i].hiz_sayac < tetikleme) continue; //[cite: 9]
        hayaletler[i].hiz_sayac = 0; //[cite: 9]
        hayaletler[i].eski_x = hayaletler[i].x; //[cite: 9]
        hayaletler[i].eski_y = hayaletler[i].y; //[cite: 9]

        int hedef_x = pac_x; //[cite: 9]
        int hedef_y = pac_y; //[cite: 9]

        // Pinky önden kesme mantığı[cite: 9]
        if (!mavi_mod && hayaletler[i].AI_tipi == 1 && mevcut_yon != 0) { //[cite: 9]
            const int oncelik = 32; //[cite: 9]
            if      (mevcut_yon == 1) hedef_x += oncelik; //[cite: 9]
            else if (mevcut_yon == 2) hedef_x -= oncelik; //[cite: 9]
            else if (mevcut_yon == 3) hedef_y -= oncelik; //[cite: 9]
            else if (mevcut_yon == 4) hedef_y += oncelik; //[cite: 9]
        }

        int en_iyi_yon = -1;
        long en_iyi_mesafe = mavi_mod ? -1 : 999999; // Mavi modda en uzağı, normalde en yakını arıyoruz

        // 4 olası yönü tara
        for (int d = 1; d <= 4; d++) {
            // Orijinal Pacman kuralı: Hayalet geri vites yapamaz (U dönüşü yasak)
            if (d == ters_yon[hayaletler[i].mevcut_yon]) continue;

            int nx = hayaletler[i].x + dx[d];
            int ny = hayaletler[i].y + dy[d];

            if (duvarVarMi(nx, ny, PAC_BOYUT)) continue;

            // Hedefe olan Manhattan + Öklid karması mesafe hesabı
            long dist = pow(nx - hedef_x, 2) + pow(ny - hedef_y, 2);

            if (mavi_mod) {
                // Kaçış modu: En uzak güvenli yolu seç[cite: 9]
                if (dist > en_iyi_mesafe) {
                    en_iyi_mesafe = dist;
                    en_iyi_yon = d;
                }
            } else {
                // Kovalama modu: En yakın yolu seç
                if (dist < en_iyi_mesafe) {
                    en_iyi_mesafe = dist;
                    en_iyi_yon = d;
                }
            }
        }

        // Eğer çıkmaz sokağa girdiyse ve tek çare geri dönmekse kuralı esnet
        if (en_iyi_yon == -1) {
            en_iyi_yon = ters_yon[hayaletler[i].mevcut_yon];
        }

        // Hayaleti hareket ettir ve yeni yönünü kaydet
        hayaletler[i].x += dx[en_iyi_yon];
        hayaletler[i].y += dy[en_iyi_yon];
        hayaletler[i].mevcut_yon = en_iyi_yon;
    }
}