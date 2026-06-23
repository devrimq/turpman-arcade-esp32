#pragma once
#include <LovyanGFX.hpp>
#include "Config.h"

// Yön tanımları: 1=Sağ, 2=Sol, 3=Yukarı, 4=Aşağı

void vektorPacmanCiz(lgfx::LGFX_Device* ekran, int x, int y, int yon, unsigned long milis);
void vektorHayaletCiz(lgfx::LGFX_Device* ekran, int x, int y, uint32_t renk, int yon, unsigned long milis);

// skor   : Bu bölge skoru
// toplam : Tüm bölgeler toplamı (0 ise gösterilmez)
void hudCiz(lgfx::LGFX_Device* ekran, int skor, int toplam,
            const char* bolge_adi, const char* lezzet);
