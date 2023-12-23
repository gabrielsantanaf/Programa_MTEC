//CODIGO BASE
#include <LiquidCrystal_I2C.h>    // Inclui a Biblioteca
#include <RTClib.h>               // Inclui a Biblioteca
#include <Keypad.h>
#include <Wire.h>
#include <HX711.h>
 float f=0;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Inicia o display 16x2 no endereço 0x27

const byte ROWS = 4; // Quatro linhas do teclado
const byte COLS = 4; // Quatro colunas do teclado
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Conecte as linhas aos pinos 6, 7, 8 e 9
byte colPins[COLS] = {5, 4, 3, 2}; // Conecte as colunas aos pinos 2, 3, 4 e 5

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// DEFINIÇÕES DE PINOS
#define pinDT  10
#define pinSCK  11
#define pinBotao 12
// DEFINIÇÕES
#define pesoMin 0.010
#define pesoMax 30.0
#define escala -307792.0f
 

// INSTANCIANDO OBJETOS
HX711 scale;

// DECLARAÇÃO DE VARIÁVEIS balança
float medida = 0;

// Definir os pinos do sensor HC-SR04
#define TRIG_PIN 35
#define ECHO_PIN 36

// Definir a altura total do reservatório em cm
#define RESERVOIR_HEIGHT 60

int nivel=0;
char texto[16];
char textoh1[16];

char estadoh = 0;
int h1=0;
int m1=0;

int h2=0;
int m2=0;

int h3=0;
int m3=0;

int IN1 = 30; //Pinos motor
int IN2 = 31;
int IN3 = 32;
int IN4 = 33;

int porte=0;
char quantidade[16];
//int giroBase=0;
float pesoBase=0;
int bazzer=34;


 
void setup()  {
  rtc.begin();                                        // Inicia o módulo RTC
  Serial.begin(57600);

  //Setup da balanca
  scale.begin(pinDT, pinSCK); // CONFIGURANDO OS PINOS DA BALANÇA
  scale.set_scale(escala); // ENVIANDO O VALOR DA ESCALA CALIBRADO
  delay(2000);
  scale.tare(); // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA
  Serial.println("Setup Finalizado!");

  lcd.init();           // Inicia o Display
  lcd.backlight();      // Inicia o Backlight
  lcd.clear();

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  //Pinos do ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(bazzer, OUTPUT);

  for(int i=0;i<3;i++){
    lcd.setCursor(4,0);
    lcd.print("_");
    lcd.setCursor(5,0);
    lcd.print("_");
    lcd.setCursor(9,0);
    lcd.print("_");
    lcd.setCursor(10,0);
    lcd.print("_");
  
    lcd.setCursor(5,1);
    lcd.print("O");
    lcd.setCursor(9,1);
    lcd.print("O");
    delay(600);
    lcd.setCursor(5,1);
    lcd.print("_");
    lcd.setCursor(9,1);
    lcd.print("_");
    delay(600);
    lcd.clear();
  } 
}

int tempo = 10;
 
void loop() {
  DateTime now = rtc.now();
  char key = keypad.getKey();
 
  deligarBobina();
  reservatorio(); 
  rtcR();

  if(key == '*') {
    if(!estadoh)
      estadoh=1;
    configurarH();
  }

  if(key == '#') {
    lcd.clear();
    lcd.setCursor(0,0);
    sprintf(quantidade, "Escolha porte:%c", porte);
    lcd.print(quantidade);
    lcd.setCursor(14,0);
    lcd.blink();

    key='\0';
    while(!key){
      key = keypad.getKey();
    }
    lcd.setCursor(14,0);
    lcd.print(key);
    lcd.setCursor(15,0);
    lcd.blink();
    porte=key;

    if(porte == 'A') {
      lcd.clear();
      //giroBase = 15;
      pesoBase = 0.200;
    }
    if(porte == 'B') {
      lcd.clear();
      //giroBase = 25;
      pesoBase = 0.300;
    }
    if(porte == 'C') {
      lcd.clear();
      //giroBase = 35;
      pesoBase = 0.400;
    }
    lcd.noBlink();
  }

  if(nivel == 3){
    lcd.clear();
    lcd.print("Falta racao!!");
    digitalWrite(bazzer, HIGH); 
  } else {
    digitalWrite(bazzer, LOW); 
  }

  int pesoAtual;
  int resultGiro;

  if((h1 == rtc.now().hour()) && (m1 == rtc.now().minute()) && (nivel == 1 || nivel == 2)) {
    unsigned long int i = millis();
    f=0;
    while(1) {
      if (millis()-i>1000){
        f = balanca();
        
        i=millis();
      }
      if (f >= pesoBase){
          break;
        }
      motorpasso();
          
    }
    deligarBobina();
    while(m1==rtc.now().minute()){}
  } else if((h2 == rtc.now().hour()) && (m2 == rtc.now().minute()) && (nivel == 1 || nivel == 2)) {
    unsigned long int i = millis();
    f=0;
    while(1) {
      if (millis()-i>1000){
        f = balanca();
        
        i=millis();
      }
      if (f >= pesoBase){
          break;
        }
      motorpasso();
          
    }
    deligarBobina();
    while(m2==rtc.now().minute()){}
  } else if((h3 == rtc.now().hour()) && (m3 == rtc.now().minute()) && (nivel == 1 || nivel == 2)) {
    unsigned long int i = millis();
    f=0;
    while(1) {
      if (millis()-i>1000){
        f = balanca();
        
        i=millis();
      }
      if (f >= pesoBase){
          break;
        }
      motorpasso();
          
    }
    deligarBobina();
    while(m3==rtc.now().minute()){}
  }
}

void rtcR() {
  lcd.setCursor(1, 0);
  lcd.print("Hora: ");
  lcd.print(rtc.now().hour());
  lcd.print(":");
  lcd.print(rtc.now().minute());
  lcd.print(":");
  lcd.print(rtc.now().second());

  lcd.setCursor(2, 1);
  lcd.print("-PetFeeder-");

  delay(1000);
  lcd.clear();
}

void configurarH() {
  int ah1 = 0;
  int am1 = 0;
  char key = keypad.getKey();

  lcd.clear();
  lcd.print("Escolha as horas: ");

  lcd.setCursor(7, 1);
  sprintf(textoh1, "%02d:%02d", ah1, am1);
  lcd.print(textoh1);
  lcd.setCursor(7,1);
  lcd.blink();
  key = keypad.getKey();
  while(!key){
    key = keypad.getKey();
  }

  ah1 = ah1 + 10 * (key - 48);
  lcd.setCursor(7, 1);
  sprintf(textoh1, "%02d:%02d", ah1, am1);
  lcd.print(textoh1);
  
  lcd.setCursor(8,1);
  lcd.blink();
  key = keypad.getKey();
  while(!key){
    key = keypad.getKey();
  }
  
  ah1 = ah1 + (key - 48);
  lcd.setCursor(7, 1);
  sprintf(textoh1, "%02d:%02d", ah1, am1);
  lcd.print(textoh1);

  lcd.setCursor(10,1);
  lcd.blink();
  key = keypad.getKey();
  while(!key){
    key = keypad.getKey();
  }
  
  am1 = am1 + 10 * (key - 48);
  lcd.setCursor(7, 1);
  sprintf(textoh1, "%02d:%02d", ah1, am1);
  lcd.print(textoh1);
  
  lcd.setCursor(11,1);
  lcd.blink();
  key = keypad.getKey();
  while(!key){
    key = keypad.getKey();
  }

  am1 = am1 + (key - 48);
  lcd.setCursor(7, 1);
  sprintf(textoh1, "%02d:%02d", ah1, am1);
  lcd.print(textoh1);
  lcd.noBlink();
  
  if(estadoh == 1) {
    h1 = ah1;
    m1 = am1;
    estadoh = 2;
    Serial.print("H1:");
    Serial.println(h1);
    Serial.print("M1:");
    Serial.println(m1);
  } else if(estadoh == 2) {
    h2 = ah1;
    m2 = am1;
    estadoh = 3;
    Serial.print("H2:");
    Serial.println(h2);
    Serial.print("M2:");
    Serial.println(m2);
  } else {
    h3 = ah1;
    m3 = am1;
    estadoh = 1;
    Serial.print("H3:");
    Serial.println(h3);
    Serial.print("M3:");
    Serial.println(m3);
  }
}

void motorpasso() {
  Serial.println("Despejando");

  //Passo 1
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 1);
  delay(tempo);  

  //Passo 2
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 1);
  delay(tempo);

  //Passo 3
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);
  delay(tempo);

  //Passo 4
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);
  delay(tempo);
}

void deligarBobina() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void reservatorio() {
  // Enviar um pulso de 10 microssegundos ao pino do gatilho
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Aguardar até que o pino de eco fique alto
  while (digitalRead(ECHO_PIN) == LOW);

  // Medir o tempo que leva para o pino de eco subir
  long startTime = micros();
  while (digitalRead(ECHO_PIN) == HIGH);
  long endTime = micros();

  // Calcular a distância até a superfície da água em cm
  long timeElapsed = endTime - startTime;
  double distance = timeElapsed * 0.034 / 2;

  // Calcular a porcentagem cheia do reservatório
  double percentage = (RESERVOIR_HEIGHT - distance) / RESERVOIR_HEIGHT * 100;

  if(percentage >= 80) {
    Serial.println("Cheio");
    nivel = 1;
  }
  if(percentage >= 42 && percentage <= 80) {
    Serial.println("Mediano");
    nivel = 2;
  }
  if(percentage <= 42) {
    Serial.println("Precisa repor");
    nivel = 3;
  }
}

// ...

float balanca() {
  scale.power_up(); // LIGANDO O SENSOR
  medida = scale.get_units(5); // SALVANDO NA VARIAVEL O VALOR DA MÉDIA DE 5 MEDIDAS
  Serial.println(medida, 3); // ENVIANDO PARA MONITOR SERIAL A MEDIDA COM 3 CASAS DECIMAIS
  scale.power_down(); // DESLIGANDO O SENSOR
  

  return medida;
}
