#include "Player.h"

int pac_x = 24, pac_y = 24, pac_yon = 0, mevcut_yon = 0;
const int PAC_BOYUT = 6, PAC_HIZ = 2;

void playerHareketSistemi() {
    int dene_x = pac_x, dene_y = pac_y;

    // İstenen yön denemesi
    if      (pac_yon == 1) dene_x += PAC_HIZ;
    else if (pac_yon == 2) dene_x -= PAC_HIZ;
    else if (pac_yon == 3) dene_y -= PAC_HIZ;
    else if (pac_yon == 4) dene_y += PAC_HIZ;

    // Yatay tünel sarması
    if      (dene_x < -PAC_BOYUT)              dene_x = EKRAN_GENISLIK + PAC_BOYUT;
    else if (dene_x > EKRAN_GENISLIK + PAC_BOYUT) dene_x = -PAC_BOYUT;

    if (!duvarVarMi(dene_x, dene_y, PAC_BOYUT)) {
        pac_x = dene_x; pac_y = dene_y; mevcut_yon = pac_yon;
    } else {
        // İstenen yön bloklandı: mevcut yönle devam etmeyi dene
        dene_x = pac_x; dene_y = pac_y;
        if      (mevcut_yon == 1) dene_x += PAC_HIZ;
        else if (mevcut_yon == 2) dene_x -= PAC_HIZ;
        else if (mevcut_yon == 3) dene_y -= PAC_HIZ;
        else if (mevcut_yon == 4) dene_y += PAC_HIZ;

        if (dene_x < -PAC_BOYUT)              dene_x = EKRAN_GENISLIK + PAC_BOYUT;
        else if (dene_x > EKRAN_GENISLIK + PAC_BOYUT) dene_x = -PAC_BOYUT;

        if (!duvarVarMi(dene_x, dene_y, PAC_BOYUT)) {
            pac_x = dene_x; pac_y = dene_y;
        }
    }
}