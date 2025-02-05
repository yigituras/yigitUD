#include <SoftwareSerial.h>
#include <DHT.h>

// Sensör ve GSM Modülü Tanımları
#define DHTPIN1 2
#define DHTPIN2 3
#define DHTPIN3 4
#define DHTTYPE DHT22
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);

#define MQ7_1 A0
#define MQ7_2 A1
#define MQ7_3 A2

SoftwareSerial sim800l(10, 11); // GSM modülü RX, TX pinleri
String phoneNumber = "+905XXXXXXXXX"; // SMS gönderilecek numara

float tempThreshold = 50.0; // Yangın riski sıcaklık eşiği
int gasThreshold = 300; // Yangın durumu için gaz eşiği (Örnek değer)

void setup() {
    Serial.begin(9600);
    sim800l.begin(9600);
    dht1.begin();
    dht2.begin();
    dht3.begin();
}

void loop() {
    float temp1 = dht1.readTemperature();
    float temp2 = dht2.readTemperature();
    float temp3 = dht3.readTemperature();
    
    int gas1 = analogRead(MQ7_1);
    int gas2 = analogRead(MQ7_2);
    int gas3 = analogRead(MQ7_3);
    
    String message = "";
    bool fireRisk = (temp1 > tempThreshold || temp2 > tempThreshold || temp3 > tempThreshold);
    bool fireDetected = (gas1 > gasThreshold || gas2 > gasThreshold || gas3 > gasThreshold);
    
    if (fireRisk) {
        message += "Yangın riski var! Sıcaklık: T1=" + String(temp1) + "C, T2=" + String(temp2) + "C, T3=" + String(temp3) + "C";
        sendSMS(message);
    }
    
    if (fireDetected) {
        message = "Yangın Çıktı! ";
        
        if (gas1 > gasThreshold && !gas2 && !gas3) message += "1. Bölgede";
        else if (gas1 > gasThreshold && gas2 > gasThreshold && !gas3) message += "2. Bölgede";
        else if (!gas1 && gas2 > gasThreshold && gas3 > gasThreshold) message += "3. Bölgede";
        else if (gas1 > gasThreshold && gas2 > gasThreshold && gas3 > gasThreshold) message += "2. ve 3. Bölgelerde";
        
        message += " yangın var! Gaz Yoğunlukları: G1=" + String(gas1) + ", G2=" + String(gas2) + ", G3=" + String(gas3);
        sendSMS(message);
    }
    
    delay(10000); // 10 saniyede bir kontrol
}

void sendSMS(String msg) {
    Serial.println("SMS gönderiliyor: " + msg);
    sim800l.println("AT+CMGF=1");
    delay(100);
    sim800l.println("AT+CMGS=\"" + phoneNumber + "\"");
    delay(100);
    sim800l.println(msg);
    delay(100);
    sim800l.write(26); // SMS gönderme komutu (CTRL+Z)
    delay(5000);
}
