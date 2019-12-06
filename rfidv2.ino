/*
 * Arduino RFID Relay Control + Mp3          
 * Created by Muhamad Iqbal        
 * Github : https://github.com/iqbalfl/arduinoproject
 * Youtube : https://youtu.be/Ng-N0XEh_ho
 */

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SPI.h>
#include <RFID.h>

#define SS_PIN 10
#define RST_PIN 9
SoftwareSerial mySerial(2, 3);

RFID rfid(SS_PIN, RST_PIN);
String myCard[3]; // Angka 3 bisa di sesuaikan dengan jumlah kartu yang akan di daftarkan
int relay_1 = 7; // Gunakan untuk kunci kontak
int relay_2 = 8; // Gunakan untuk starter mesin
int tapCount = 0;
int failCount = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.init();
  myCard[0] = "213321284588"; // Kartu terdaftar 1
  myCard[1] = "459512202180"; // Kartu terdaftar 2
  // Jika ingin menambahkan kartu baru, buat array data kartu baru disini

  // set pin 7,8 sebagai output
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);

  // set default relay mode off
  digitalWrite(relay_1, HIGH);
  digitalWrite(relay_2, HIGH);

  // ------------------ mp3 area ------------------ //
  // Note : AGAR DFPLAYER BISA MEMUTAR MP3 //
  // Buat folder dengan nama mp3 pada sd card //
  // Masukan file mp3 ke folder mp3 //
  // Ubah nama file dengan format 4 digit angka ///
  // Contoh 0001.mp3, 0002.mp3, 0003.mp3 dst //

  mySerial.begin(9600);
  mp3_set_serial(mySerial); //set softwareSerial for DFPlayer
  delay(10);
  mp3_reset(); //soft-Reset module DFPlayer
  delay(10); //wait 1ms for respon command
  mp3_set_volume(90); //set Volume module DFPlayer
  delay(1000);
}

void loop() {
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      String serial;
      int member = 0;

      // Perulangan untuk membaca blok kode kartu rfid
      for (int i = 0; i <= 4; i++) {
        serial += String(rfid.serNum[i], DEC);
      }
      // Serial.println("Card ID: " + serial);  // Silahkan Uncomment line ini untuk menampilkan serial kartu pada serial monitor

      // Perulangan untuk memeriksa kartu rfid yang terdaftar
      for (int i = 0; i < sizeof(myCard); i++) {
        if (serial == myCard[i]) {
          member = 1;
        }
      }

      // jika kartu terdaftar
      if (member == 1) {
        tapCount++;
        if (tapCount == 1) {
          Serial.println("Kontak menyala, silakan tap lagi untuk menyalakan mesin");
          mp3_play(1);
          digitalWrite(relay_1, LOW);
          digitalWrite(relay_2, HIGH);
        }
        if (tapCount == 2) {
          mp3_play(2);
          Serial.println("Menyalakan mesin");
          delay(1700);
          digitalWrite(relay_1, LOW);
          digitalWrite(relay_2, LOW);
          delay(800);
          digitalWrite(relay_2, HIGH);
        }
        if (tapCount == 3) {
          Serial.println("Mematikan mesin dan kontak");
          digitalWrite(relay_1, HIGH);
          digitalWrite(relay_2, HIGH);
          delay(200);
          mp3_play(5);
        }
        if (tapCount >= 3) {
          tapCount = 0;
        }

      } else {
        failCount++;
        Serial.println("Maaf, kartu anda tidak terdaftar!");
        mp3_play(3);
        digitalWrite(relay_1, HIGH);
        digitalWrite(relay_2, HIGH);
        tapCount = 0;
        if (failCount == 3) {
          delay(2000);
          Serial.println("Akses gagal selama 3 kali, sistem di blokir dalam waktu 5 menit");
          mp3_play(4);

          // Sistem di blokir selama 5 menit
          int longDelayInSeconds = 300;
          int p = 0;
          while (p < longDelayInSeconds) {
            delay(1000);
            p++;
          }

          failCount = 0;
        }
      }
    }

  }
  rfid.halt();
  delay(1000);
}