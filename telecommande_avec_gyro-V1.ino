#include <nRF24L01.h>
#include <RF24.h>
#include<SPI.h>

RF24 radio(9,10);
const byte address[6] = "00001";

// pour le pad et les boutons du joystick \\
  
#define PAD_X A0 // Direction servo Barre
#define PAD_Y A1
#define BOUTON_PAD 8
#define BOUTON_A 2 // Ecoute max
#define BOUTON_B 3 // Ecoute min
#define BOUTON_C 4 // Ecoute -
#define BOUTON_D 5 // Ecoute +
#define BOUTON_E 6 // Mode Auto

#define DELAY 1000    // pour debug

const int threshold = 20;

int Barre = 0; //mettre le seuil moyen Barre
int Ecoute = 0; //mettre le seuil moyen Ecoute 

int angleBarre = 0;
int angleEcoute = 0;

int checkBarre = 0;
int checkEcoute = 0;

int pasEcoute = 10;

const char var1[32] = "ServoBarre";
const char var2[32] = "ServoEcoute";
const char var3[32] = "ModeVent";


void setup(){
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();

  for (int GPIO = 0; GPIO <= 8; GPIO++) {
    pinMode(GPIO, INPUT);
    digitalWrite(GPIO, HIGH);
  }
}

void loop(){
  Barre = analogRead(PAD_X);
  if(Barre > checkBarre + threshold || Barre < checkBarre - threshold){
    radio.write(&var1, sizeof(var1));
    angleBarre = map(Barre, 0, 720, 0, 180);
    radio.write(&angleBarre, sizeof(angleBarre));
    checkBarre = Barre;
    Serial.println("INPUT:1");
    Serial.print("Angle:");
    Serial.println(angleBarre);
    Serial.print("Voltage Level:");
    Serial.println(Barre);
    Serial.println("----------------------------------");
  }
  if(digitalRead(BOUTON_E) == LOW) && modeVent = 0){
  modeVent = 1;
  radio.write(&modeVent, sizeof(modeVent));
  Serial.println("Mode Auto");
  }
  else if(digitalRead(BOUTON_E) == LOW) && modeVent = 1){
  modeVent = 0;
  radio.write(&modeVent, sizeof(modeVent));
  Serial.println("Mode Manuel");
  }
  if (digitalRead(BOUTON_A) == LOW) {
    Serial.println(" BOUTON A ");    // pour debug
    angleEcoute = 180;
    delay(DELAY);    
  }
   if (digitalRead(BOUTON_B) == LOW) {
    Serial.println(" BOUTON B ");    // pour debug
    angleEcoute = 0;
    delay(DELAY);    
  }
   if (digitalRead(BOUTON_C) == LOW && angleEcoute >= 0 + pasEcoute) {
    Serial.println(" BOUTON C ");    // pour debug
    angleEcoute = angleEcoute - pasEcoute;
    delay(DELAY);    
  }
   if (digitalRead(BOUTON_D) == LOW && angleEcoute <= 180 - pasEcoute) {
    Serial.println(" BOUTON D ");    // pour debug
    angleEcoute = angleEcoute + pasEcoute;
    delay(DELAY);    
  }
  if(angleEcoute != checkEcoute){
    radio.write(&var2, sizeof(var2));
    radio.write(&angleEcoute, sizeof(angleEcoute));
    checkEcoute = angleEcoute;
    Serial.println("INPUT:2");
    Serial.print("Valeur d'ecoute:");
    Serial.println(angleEcoute);
    Serial.println("----------------------------------");
}
}
