#pragma once
#include "Config.h"
#include <Arduino.h>
#include <LovyanGFX.hpp>

struct Bolge {
    const char* bolge_adi;
    const char* efsane_lezzet;
    uint32_t    duvar_rengi;
    int         patron_skoru;
};

// Her zorluk seviyesi için hayalet başlangıç noktası
struct SpawnNokta { int x, y; };

extern const int  TOPLAM_BOLGE;
extern Bolge      anadolu_turu[];
extern int        aktif_bolge_indeksi;
extern const uint8_t (*aktif_harita)[HARITA_SUTUN];
extern SpawnNokta hayalet_spawn[2]; // Aktif haritaya göre güncellenir

// haritaGuncelle: aktif_harita ve hayalet_spawn'u senkronize günceller
void haritaGuncelle();
bool duvarVarMi(int x, int y, int pac_boyut);
void haritayiCiz(lgfx::LGFX_Device* ekran);
