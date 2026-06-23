#pragma once
#include "Config.h" //[cite: 3]
#include "Map.h" //[cite: 3]
#include "Player.h" //[cite: 3]

struct Hayalet { //[cite: 3]
    int      x, y, eski_x, eski_y; //[cite: 3]
    uint32_t orijinal_renk; //[cite: 3]
    int      hiz_sayac; //[cite: 3]
    int      AI_tipi; // 0: Blinky, 1: Pinky[cite: 3]
    int      mevcut_yon; // YENİ: 1=Sağ, 2=Sol, 3=Yukarı, 4=Aşağı
};

extern Hayalet hayaletler[2]; //[cite: 3]

void hayaletYapayZeka(bool mavi_mod); //[cite: 3]
void hayaletleriSifirla(); //[cite: 3]