#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SPI.h>
#include <RFID.h>

#define SS_PIN 10
#define RST_PIN 9
SoftwareSerial mySerial(2, 3);

RFID rfid(SS_PIN,RST_PIN);
String myCard[3];
int relay_1 = 7; //ceritanya kunci kontak
int relay_2 = 8; //ceritanya starter mesin
int count=0;
int denied_c=0;

void setup()
{
 Serial.begin(9600);
 SPI.begin();
 rfid.init();
 myCard[0] = "213321284588"; //kartu terdaftar 1
 myCard[1] = "459512202180"; //kartu terdaftar 2

 // set pin 7,8 sebagai output
 pinMode(relay_1,OUTPUT);
 pinMode(relay_2,OUTPUT);

 // set default relay mode off
 digitalWrite(relay_1,HIGH);
 digitalWrite(relay_2,HIGH);

 // ----------- mp3 area ------------- //
 mySerial.begin (9600);
 mp3_set_serial (mySerial); //set softwareSerial for DFPlayer
 delay(10);
 mp3_reset();  //soft-Reset module DFPlayer
 delay(10);   //wait 1ms for respon command
 mp3_set_volume (90); //set Volume module DFPlayer
 delay(1000);
}

void loop()
{ 
 if(rfid.isCard())
 {
  if(rfid.readCardSerial()) 
  { 
   String serial;
   int member = 0;
   // perulangan untuk membaca kartu rfid
   for (int i=0; i<= 4; i++) 
   { 
    serial += String(rfid.serNum[i],DEC);
   }
  //Serial.println("Card ID: " + serial);  untuk menampilkan serial kartu
              
   //perulangan untuk memeriksa kartu rfid yang terdaftar
   for (int i=0; i<2; i++) 
   {
     if (serial == myCard[i])
     {
       member = 1;
     } 
   }

   // jika kartu terdaftar
   if (member == 1)
   {
     count++;
     if(count == 1){
       Serial.println("Hallo sir, silakan tap lagi untuk menyalakan mesin");
       mp3_play (1);
       digitalWrite(relay_1,LOW);
       digitalWrite(relay_2,HIGH);
     }
     if(count == 2){
       mp3_play (2);
       Serial.println("Menyalakan mesin");
       delay(1700);
       digitalWrite(relay_1,LOW);
       digitalWrite(relay_2,LOW);
       delay(800);
       digitalWrite(relay_2,HIGH);
     }if(count == 3){
       Serial.println("Mematikan mesin dan kontak");
       digitalWrite(relay_1,HIGH);
       digitalWrite(relay_2,HIGH);
       delay(200);
       mp3_play (5);
     }
     if (count >=3){count=0;}
    
   } else 
   {
     denied_c++;
     Serial.println("Maaf, kartu anda tidak terdaftar!");
     mp3_play (3);
     digitalWrite(relay_1,HIGH);
     digitalWrite(relay_2,HIGH);
     count=0;
     if(denied_c == 3){
        delay(2000);
        Serial.println("Akses gagal selama 3 kali, sistem di blokir dalam waktu 5 menit");
        mp3_play (4);
        
        //sistem delay selama 5 menit
        int longDelayInSeconds = 300;
        int p = 0; 
        while(p < longDelayInSeconds){
          delay(1000);
          p++;
        }
        
        //kembalikan denied count ke 0
        denied_c=0;
     }
   }
  }
  
 }
 rfid.halt();
 delay(1000);
}
