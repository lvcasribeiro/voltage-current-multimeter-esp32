#include <PZEM004Tv30.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Bibliotecas para comunicação com a base de dados em nuvem:
#include <FirebaseESP32.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// Conexão banco de dados em nuvem - Firebase, host e auth:
#define FIREBASE_AUTENTICADOR "2Bhe9kS0ocZNvcstR66iplAzJXxbNkk9inLbXgfS"
#define FIREBASE_HOST "multimedidor-tensao-corrente-default-rtdb.firebaseio.com"

FirebaseData firebaseData;

// Conexão wi-fi, SSID e senha da rede:
#define WIFI_IDENTIFICADOR "NET_2G3709CE"
#define WIFI_SENHA "LLCN6641"


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
PZEM004Tv30 pzem(Serial2, 16, 17);



void setup() {
    Serial.begin(115200);
    // Uncomment in order to reset the internal energy counter
    // pzem.resetEnergy()

    // Conexão wi-fi:
    WiFi.begin(WIFI_IDENTIFICADOR, WIFI_SENHA);

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(2500);
    }

     if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
      }
      delay(2000);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10, 10);
      // Display static text
      SplashScreen();
      display.display();
      delay(3000);

      // Conexão banco de dados em nuvem - Firebase:
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTENTICADOR);
      Firebase.reconnectWiFi(true);
}

void loop() {
        
    Serial.print("Custom Address:");
    Serial.println(pzem.readAddress(), HEX);
    
    // Read the data from the sensor
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    if (current < 0.05) {
        current = 0;
        power = 0;
    }

    // Check if the data is valid
    if(isnan(voltage)){
        Serial.println("Error reading voltage");
    } else if (isnan(current)) {
        Serial.println("Error reading current");
    } else if (isnan(power)) {
        Serial.println("Error reading power");
    } else if (isnan(energy)) {
        Serial.println("Error reading energy");
    } else if (isnan(frequency)) {
        Serial.println("Error reading frequency");
    } else if (isnan(pf)) {
        Serial.println("Error reading power factor");
    } else {

        // Print the values to the Serial console
        Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
        Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
        Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
        Serial.print("Energy: ");       Serial.print(energy,3);     Serial.println("kWh");
        Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
        Serial.print("PF: ");           Serial.println(pf);

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);

 
        display.setCursor(0, 0);
        display.println("Voltage: " + String(voltage) + "V");

        display.setCursor(0, 10);
        display.println("Current: " + String(current) + "A" );

        display.setCursor(0, 20);
        display.println("Power: " + String(power) + "W" );
        

        display.setCursor(0, 30);
        display.println("Energy: " + String(energy) + "kWh");

        display.setCursor(0, 40);
        display.println("Frequency: " + String(frequency) + "Hz");

        display.setCursor(0, 50);
        display.println("PF: " + String(pf) + "%");

        display.display(); 
    }

    // Atualização dos valores:
    if(Firebase.get(firebaseData, "/VariaveisMedidas/Tensao")) {
        Firebase.set(firebaseData, "/VariaveisMedidas/Tensao", voltage);
    } else {
        Serial.print("[!] Erro!");
    }

    if(Firebase.get(firebaseData, "/VariaveisMedidas/corrente")) {
        Firebase.set(firebaseData, "/VariaveisMedidas/corrente", current);
    } else {
        Serial.print("[!] Erro!");
    }

    if(Firebase.get(firebaseData, "/VariaveisMedidas/energia")) {
        Firebase.set(firebaseData, "/VariaveisMedidas/energia", energy);
    } else {
        Serial.print("[!] Erro!");
    }

    if(Firebase.get(firebaseData, "/VariaveisMedidas/potencia")) {
        Firebase.set(firebaseData, "/VariaveisMedidas/potencia", power);
    } else {
        Serial.print("[!] Erro!");
    }

    if(Firebase.get(firebaseData, "/VariaveisMedidas/frequencia")) {
        Firebase.set(firebaseData, "/VariaveisMedidas/frequencia", frequency);
    } else {
        Serial.print("[!] Erro!");
    }

    if(Firebase.get(firebaseData, "/VariaveisMedidas/fator_de_potencia")) {
        Firebase.set(firebaseData, "/VariaveisMedidas/fator_de_potencia", pf);
    } else {
        Serial.print("[!] Erro!");
    }
   
    delay(2000);
    Serial.println();

}

void SplashScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //Desenha um circulo, nas coodenadas cartesianas x=48 e y=6, e com raio 6
  display.fillCircle(48, 6, 6, WHITE);
  display.fillRoundRect(42, 15,  12, 12,  2, WHITE);
  display.fillRoundRect(42, 29,  12, 12,  2, WHITE);
  display.fillRoundRect(42, 43,  12, 12,  2, WHITE);
  display.fillRoundRect(57, 1,  12, 12,  2, WHITE);
  display.fillRoundRect(57, 15,  12, 12,  2, WHITE);
  display.fillRoundRect(57, 29,  12, 12,  2, WHITE);
  display.fillRoundRect(57, 43,  12, 12,  2, WHITE);
  display.fillRoundRect(72, 1,  12, 12,  2, WHITE);
  display.fillRoundRect(72, 15,  12, 12,  2, WHITE);
  display.setCursor(30, 57); //Posiciona o cursor na posição x=30 e y=57
  display.println("MULTIMEDIDOR"); //Escreve o texto iniciando da posição do cursor
  display.display(); //Aplica todas as alteraçoes no display

}
