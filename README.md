# turpman-arcade-esp32# 🕹️ Turpman Arcade - ESP32-S3 & LovyanGFX v1.0

Yerli ve milli tarım harikası, Osmaniye kıtırı **Turpman** artık ESP32-S3 ve LovyanGFX motoruyla el atarinizde! Klavyeden aldığınız komutlarla sarı turpu koridorlarda koşturun, hayaletten kaçın ve "YOU'RE FIRED!" cezasına çarptırılmadan rekorları kırın!

---

## 🛠️ Donanım ve Sürücü Özellikleri
* **Mikrodenetleyici:** ESP32-S3 (Klon Geliştirme Kartı)
* **Grafik Motoru:** LovyanGFX v1 (80MHz SPI Hızı ile Ultra Akıcı 30 FPS Kilitli)
* **Ekran Sürücüsü:** ST7789 IPS Panel ($170 \times 320$ Gerçek Çözünürlük, +35px Klon Kayma Payı)
* **Bağlantı:** Tamamen donanım dostu, kilitlenmeyen seri port veri yolu köprüsü.

---

## 🎮 Oyun Mekanikleri & Kontroller
Oyun, sinsi `GPIO 0` ve `GPIO 2` bootloop kilitlenmelerini aşmak adına **PlatformIO Seri Monitör (Serial Monitor)** üzerinden klavye ile kontrol edilmektedir:
* `w` veya `8` : Yukarı
* `s` veya `2` : Aşağı
* `a` || `4` : Sol
* `d` || `6` : Sağ

### 🚨 Önemli Özellikler
* **Akıllı Labirent:** Matris tabanlı duvar çarpışma motoru sayesinde ne Turpman ne de Hayalet duvarların içinden geçebilir.
* **Dengelenmiş AI:** Hayalet, Turpman'den 4 kat daha yavaş hareket ederek oyuncuya taktiksel kaçış alanı tanır.
* **Mizah Soslu Game Over:** Yakalandığınız an ekranda beliren o meşhur kırmızı şerit: **YOU'RE FIRED!**

---

## 🚀 Kurulum ve Çalıştırma
1. Projeyi bilgisayarınıza klonlayın:
   ```bash
   git clone [https://github.com/devrimq/turpman-arcade-esp32.git](https://github.com/devrimq/turpman-arcade-esp32.git)