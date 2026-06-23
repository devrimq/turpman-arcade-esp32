#include "Map.h"

const int TOPLAM_BOLGE = 7; //[cite: 11]
Bolge anadolu_turu[TOPLAM_BOLGE] = { //[cite: 11]
    {"MARMARA",      "Iskender",      TFT_SKYBLUE, 300}, //[cite: 11]
    {"EGE",          "Enginar",       TFT_GREEN,   400}, //[cite: 11]
    {"AKDENIZ",      "Tantuni",       TFT_ORANGE,  500}, //[cite: 11]
    {"IC ANADOLU",   "Kayseri Manti", TFT_YELLOW,  600}, //[cite: 11]
    {"KARADENIZ",    "Kuymak",        TFT_CYAN,    700}, //[cite: 11]
    {"D. ANADOLU",   "Cag Kebabi",    TFT_WHITE,   800}, //[cite: 11]
    {"G.D. ANADOLU", "Ali Nazik",     TFT_RED,     900}  //[cite: 11]
};

int aktif_bolge_indeksi = 0; //[cite: 11]

static const SpawnNokta SPAWN_KOLAY[2] = {{280, 24}, {280, 136}}; //[cite: 11]
static const SpawnNokta SPAWN_ORTA[2]  = {{280, 24}, {280, 136}}; //[cite: 11]
static const SpawnNokta SPAWN_ZOR[2]   = {{280, 24}, {280, 136}}; //[cite: 11]

SpawnNokta hayalet_spawn[2] = {{280, 24}, {280, 136}}; //[cite: 11]

// PROGMEM eklenerek RAM tasarrufu sağlandı
const uint8_t LABIRENT_KOLAY[HARITA_SATIR][HARITA_SUTUN] PROGMEM = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,0,0},
    {0,0,1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

const uint8_t LABIRENT_ORTA[HARITA_SATIR][HARITA_SUTUN] PROGMEM = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {0,0,1,0,1,1,0,1,1,0,0,1,1,0,1,1,0,1,0,0},
    {0,0,0,0,1,1,0,1,0,0,0,0,1,0,1,1,0,0,0,0},
    {1,0,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,0,1},
    {1,0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

const uint8_t LABIRENT_ZOR[HARITA_SATIR][HARITA_SUTUN] PROGMEM = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,0,0,1},
    {1,0,1,0,1,0,1,0,0,0,0,0,0,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,1,1,1,0,0,1,1,1,0,0,0,1,0,1},
    {0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,1,0,1,1,1,0,0,1,1,1,0,1,0,0,0,0},
    {1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,1,0,0,0,1,1,1,0,0,1,1,1,0,0,0,1,0,1},
    {1,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

const uint8_t (*aktif_harita)[HARITA_SUTUN] = LABIRENT_KOLAY; //[cite: 11]

void haritaGuncelle() { //[cite: 11]
    const SpawnNokta* spawn; //[cite: 11]
    if (aktif_bolge_indeksi <= 1) { //[cite: 11]
        aktif_harita = LABIRENT_KOLAY; //[cite: 11]
        spawn        = SPAWN_KOLAY; //[cite: 11]
    } else if (aktif_bolge_indeksi <= 4) { //[cite: 11]
        aktif_harita = LABIRENT_ORTA; //[cite: 11]
        spawn        = SPAWN_ORTA; //[cite: 11]
    } else { //[cite: 11]
        aktif_harita = LABIRENT_ZOR; //[cite: 11]
        spawn        = SPAWN_ZOR; //[cite: 11]
    } //[cite: 11]
    hayalet_spawn[0] = spawn[0]; //[cite: 11]
    hayalet_spawn[1] = spawn[1]; //[cite: 11]
}

bool duvarVarMi(int x, int y, int /*pac_boyut*/) { //[cite: 11]
    if (x < 0 || x >= EKRAN_GENISLIK) return false; //[cite: 11]
    if (y < 0 || y >= OYUN_ALANI_Y)   return true; //[cite: 11]

    const int hitbox = 3; //[cite: 11]
    int sol = constrain((x - hitbox) / KUTU_BOYUTU, 0, HARITA_SUTUN - 1); //[cite: 11]
    int sag = constrain((x + hitbox) / KUTU_BOYUTU, 0, HARITA_SUTUN - 1); //[cite: 11]
    int ust = constrain((y - hitbox) / KUTU_BOYUTU, 0, HARITA_SATIR - 1); //[cite: 11]
    int alt = constrain((y + hitbox) / KUTU_BOYUTU, 0, HARITA_SATIR - 1); //[cite: 11]

    // PROGMEM okuması için pgm_read_byte eklendi
    return (pgm_read_byte(&(aktif_harita[ust][sol])) == 1 || 
            pgm_read_byte(&(aktif_harita[ust][sag])) == 1 ||
            pgm_read_byte(&(aktif_harita[alt][sol])) == 1 || 
            pgm_read_byte(&(aktif_harita[alt][sag])) == 1);
}

void haritayiCiz(lgfx::LGFX_Device* ekran) { //[cite: 11]
    for (int r = 0; r < HARITA_SATIR; r++) { //[cite: 11]
        for (int c = 0; c < HARITA_SUTUN; c++) { //[cite: 11]
            if (pgm_read_byte(&(aktif_harita[r][c])) == 1) { // PROGMEM Güncellemesi
                ekran->fillRect(c * KUTU_BOYUTU, r * KUTU_BOYUTU, KUTU_BOYUTU, KUTU_BOYUTU, TFT_BLACK); //[cite: 11]
                ekran->drawRect(c * KUTU_BOYUTU, r * KUTU_BOYUTU, KUTU_BOYUTU, KUTU_BOYUTU, anadolu_turu[aktif_bolge_indeksi].duvar_rengi); //[cite: 11]
            } else { //[cite: 11]
                if ((r == 1 && c == 1)  || (r == 1 && c == 18) || (r == 8 && c == 1)  || (r == 8 && c == 18)) { //[cite: 11]
                    ekran->fillCircle(c * KUTU_BOYUTU + 8, r * KUTU_BOYUTU + 8, 5, TFT_GOLD); //[cite: 11]
                } else { //[cite: 11]
                    ekran->fillRect(c * KUTU_BOYUTU + 6, r * KUTU_BOYUTU + 6, 4, 4, TFT_YELLOW); //[cite: 11]
                }
            }
        }
    }
}