#include "Ultrasonic.h"
#include <Servo.h>
#define buzzer 13

Ultrasonic ultras_1(6,5);  // Esquerda
Ultrasonic ultras_2(4,3);  // Direita

Servo servo1;
Servo servo2;

// LEDs para o semaforo
int led_Vermelho = 11;
int led_verde = 12;

int pin_led[] = {7,8,9,10};
int current_pos = 0;  // Posição atual dos servos

float distancia_1 = 50, distancia_2 = 50; // Iniciar com um valor seguro

bool ponte_aberta = false;

void setup() {
  Serial.begin(9600);
  
  servo1.attach(A1); // Servo da esquerda
  servo2.attach(A2); // Servo da direita
  servo1.write(81);
  servo2.write(80);

  
 // configurando os pinos
  pinMode(A0, INPUT); // LDR
  pinMode(buzzer, OUTPUT);
  pinMode(led_Vermelho,OUTPUT);
  pinMode(led_verde,OUTPUT);

  for (int pin = 0; pin <= 3; pin++) {
    pinMode(pin_led[pin], OUTPUT);
  }

  digitalWrite(led_Vermelho,LOW);
  digitalWrite(led_verde,HIGH);
}

void loop() {
  int ldr = analogRead(A0);
  int ldr_vals = map(ldr, 10, 975, 0, 100);

  float leitura1 = ultras_1.Ranging(CM);
  float leitura2 = ultras_2.Ranging(CM);

  if (leitura1 > 1 && leitura1 < 100) distancia_1 = leitura1;
  if (leitura2 > 1 && leitura2 < 100) distancia_2 = leitura2;

  Serial.print("Esquerda: ");
  Serial.print(distancia_1);
  Serial.print(" cm | Direita: ");
  Serial.print(distancia_2);
  Serial.print(" cm | LDR: ");
  Serial.println(ldr_vals);

  // Controle do poste de luz
  poste(ldr_vals <= 20 ? HIGH : LOW);

  // Abrir a ponte se o barco for detectado em qualquer sensor
  if ((distancia_1 <= 18 || distancia_2 <= 18) && !ponte_aberta) {
    abrir_ponte();
  }

  // Fechar a ponte apenas se os dois sensores estiverem livres (barco já passou)
  if (distancia_1 > 19 && distancia_2 > 19 && ponte_aberta) {
    fechar_ponte();
  }
}

void abrir_ponte() {
  digitalWrite(buzzer, HIGH);
  digitalWrite(led_Vermelho,HIGH);
  digitalWrite(led_verde,LOW);
  delay(800);
  for (int x = current_pos; x <= 90; x++) {
    servo_12(x);
  }
  current_pos = 90;
  ponte_aberta = true;
  digitalWrite(buzzer, LOW);
}

void fechar_ponte() {
  delay(2000);  // Espera um pouco antes de fechar
  
  for (int x = current_pos; x >= 0; x--) {
    servo_12(x);
  }
  current_pos = 0;
  ponte_aberta = false;
  digitalWrite(led_Vermelho,LOW);
  digitalWrite(led_verde,HIGH);
  digitalWrite(buzzer, LOW);
}

// Controle dos LEDs do poste
void poste(boolean estado) {
  for (int pin = 0; pin <= 3; pin++) {
    digitalWrite(pin_led[pin], estado);
  }
}

// Controle dos dois serv       os sincronizados
void servo_12(int angulo) {
  servo1.write(81 - angulo); // Servo da esquerda
  delay(10);
  servo2.write(80 - angulo); // Servo da direita
  delay(10);
}
