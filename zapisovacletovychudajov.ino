#include <MPU6050.h>            // MPU6050 library
#include <Wire.h>               // I2C communication library
#include <Adafruit_Sensor.h>    // Sensor library
#include <Adafruit_BMP280.h>    // BMP280 sensor library
#include <TinyGPS++.h>          // GPS library
#include <SoftwareSerial.h>     // SoftwareSerial library for GPS
#include <SD.h>          // SD card library

static const int RXPin = 4, TXPin = 3;  // Definujem piny RX a TX pre komunikáciu s GPS
static const uint32_t GPSBaud = 9600;   // Baudová rýchlosť GPS modulu
MPU6050 mpu;                    // Inicializácia MPU6050
Adafruit_BMP280 bmp280;         // Inicializácia senzora BMP280
TinyGPSPlus gps;                // Inicializácia GPS modulu
SoftwareSerial ss(RXPin, TXPin);// Inicializácia softvérového sériového rozhrania pre GPS
unsigned long lastSaveTime = 0; // Premenná pre uloženie času posledného uloženia
unsigned long lastWriteTime = 0; // Premenná pre uloženie času posledného zápisu
float angleX, angleY, angleZ;   // Premenné pre rotáciu
// Definícia ofsetov pre MPU6050 hodnotami, ktoré som dostal po kalibrácií
const float offsetX = 665.0;
const float offsetY = -38.0;
const float offsetZ = 104.0;

void setup() {
    Serial.begin(9600);    // Inicializácia sériovej komunikácie
    ss.begin(GPSBaud);     // Inicializácia softvérového sériového rozhrania pre GPS

    if (!SD.begin(10)) {  // Kontrola inicializácie SD karty
        Serial.println(F("Incializácia SD karty zlyhala!"));
        return;
    }
    if (!bmp280.begin(0x76)) {  // Kontrola inicializácie senzora BMP280
        Serial.println(F("Senzor BMP280 nenájdený, skontrolujte zapojenie!"));
        while (1);
    }
    mpu.initialize();  // Inicializácia MPU6050
}
void loop() {
    // Získanie údajov o zrýchlení z MPU6050
    int16_t rawAx, rawAy, rawAz;
    mpu.getAcceleration(&rawAx, &rawAy, &rawAz);
    // Aplikácia ofsetov na čítania zrýchlenia
    float ax = rawAx - offsetX;
    float ay = rawAy - offsetY;
    float az = rawAz - offsetZ;

    // Konverzia na stupne, ak je potrebné  
    angleX = atan2(ay, az) * 180 / PI;
    angleY = atan2(-ax, az) * 180 / PI;
    angleZ = atan2(az, sqrt(ax * ax + ay * ay)) * 180 / PI;

    // Cyklus na čítanie údajov z GPS
    while (ss.available() > 0) {
        if (gps.encode(ss.read())) {
            if (millis() - lastWriteTime >= 1000) { // Zápis údajov iba raz za sekundu
                saveLocation(); // Uloženie GPS údajov
                lastWriteTime = millis();
            }
        }
    }
    // Zápis údajov do súboru
    if (millis() - lastSaveTime >= 2000) {
        if (saveLocation()) {
            Serial.println(F("Údaje uložené"));
        }
        lastSaveTime = millis();
    }
    delay(100); // Čakanie pred ďalším čítaním údajov, slúži na uľahčenie kódu
}

bool saveLocation() {
    File dataFile = SD.open(F("data.txt"), FILE_WRITE); // Otvorenie súboru na zápis
    if (dataFile) { // Kontrola, či sa súbor úspešne otvoril
        // Ak je súbor prázdny, zapíše sa hlavička tabuľky
        if (dataFile.size() == 0) { 
            dataFile.println(F("Čas\t\tZemepisná šírka\tZemepisná dĺžka\tTeplota (°C)\tTlak (hPa)\tNadmorská výška (m)\tX\tY\tZ"));
        } //Ak je súbor prázdny zapíše sa hlavička tabuľky
        // Získanie aktuálneho času zo satelitu GPS
        String time = String((gps.time.hour() + 1) % 24) + ":" + (gps.time.minute() < 10 ? "0" : "") + String(gps.time.minute()) + ":" + (gps.time.second() < 10 ? "0" : "") + String(gps.time.second());
        // Zapíšeme údaje do súboru oddelené tabulátormi
        dataFile.print(time); // Zapíšeme čas
        dataFile.write('\t');
        dataFile.print(gps.location.lat(), 6);  // Zemepisná šírka s 6 desatinnými miestami
        dataFile.write('\t');
        dataFile.print(gps.location.lng(), 6);  // Zemepisná dĺžka s 6 desatinnými miestami
        dataFile.write('\t');
        dataFile.print(bmp280.readTemperature(), 2);    // Teplota s 2 desatinnými miestami
        dataFile.write('\t');
        dataFile.write('\t');
        dataFile.print(bmp280.readPressure() / 100.0, 2);  // Tlak s 2 desatinnými miestami
        dataFile.write('\t');
        dataFile.write('\t');
        dataFile.print(bmp280.readAltitude(1013.25)); // Nadmorská výška
        dataFile.write('\t');
        dataFile.write('\t');
        dataFile.print(angleX); // Rotácia v osi X
        dataFile.write('\t');
        dataFile.print(angleY); // Rotácia v osi Y
        dataFile.write('\t');
        dataFile.println(angleZ); // Rotácia v osi Z
        dataFile.close(); // Zatvorenie súboru
        // Výpis údajov do sériovej konzoly
        Serial.print(time);
        Serial.write('\t');
        Serial.print(gps.location.lat(), 6);
        Serial.write('\t');
        Serial.print(gps.location.lng(), 6);
        Serial.write('\t');
        Serial.print(bmp280.readTemperature());
        Serial.write('\t');
        Serial.print(bmp280.readPressure() / 100.0);
        Serial.write('\t');
        Serial.print(bmp280.readAltitude(1013.25));
        Serial.write('\t');
        Serial.print(angleX);
        Serial.write('\t');
        Serial.print(angleY);
        Serial.write('\t');
        Serial.println(angleZ);
        // Funkcia vráti hodnotu true, čo indikuje úspech zápisu
        return true;
    }
    // Ak sa súbor neotvoril, vypíše sa chybové hlásenie a funkcia vráti false
    Serial.println(F("Chyba zápisu na SD kartu"));
    return false;
}


