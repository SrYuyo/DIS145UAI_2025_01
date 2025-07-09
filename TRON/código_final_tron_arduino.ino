#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int cs = 2;
File lectologo;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pines para botones
int interruptorJ1 = 4;
int botonj1arriba = 8;
int botonj1derecha = 9;
int botonj1izquierda = 10;
int botonj1abajo = A0;
int interruptorJ2 = A1;
int botonj2arriba = 5;
int botonj2derecha = 6;
int botonj2izquierda = 3;
int botonj2abajo = 7;

bool juegoIniciado = false;
int vidasP1 = 2;
int vidasP2 = 2;
bool estadoAnteriorInterruptorJ1 = LOW;
bool estadoAnteriorInterruptorJ2 = LOW;

byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};

void setup() {
  Serial.begin(9600);

  if (!SD.begin(cs)) {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Error SD init");
    return;
  }

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, heart);
  lcd.setCursor(2, 0);
  lcd.print("PREPARENSE...");

  pinMode(interruptorJ1, INPUT);
  pinMode(interruptorJ2, INPUT);

  pinMode(botonj1arriba, INPUT);
  pinMode(botonj1derecha, INPUT);
  pinMode(botonj1izquierda, INPUT);
  pinMode(botonj1abajo, INPUT);
  pinMode(botonj2arriba, INPUT);
  pinMode(botonj2derecha, INPUT);
  pinMode(botonj2izquierda, INPUT);
  pinMode(botonj2abajo, INPUT);
}

void loop() {
  // Lectura de botones y envío a Processing
  if (digitalRead(botonj1arriba) == LOW)    Serial.println("J1U");
  if (digitalRead(botonj1abajo) == LOW)     Serial.println("J1D");
  if (digitalRead(botonj1derecha) == LOW)   Serial.println("J1R");
  if (digitalRead(botonj1izquierda) == LOW) Serial.println("J1L");

  if (digitalRead(botonj2arriba) == LOW)    Serial.println("J2U");
  if (digitalRead(botonj2abajo) == LOW)     Serial.println("J2D");
  if (digitalRead(botonj2derecha) == LOW)   Serial.println("J2R");
  if (digitalRead(botonj2izquierda) == LOW) Serial.println("J2L");

  bool estadoActualInterruptorJ1 = digitalRead(interruptorJ1);
  if (estadoActualInterruptorJ1 != estadoAnteriorInterruptorJ1) {
    Serial.println("cambio1");
    estadoAnteriorInterruptorJ1 = estadoActualInterruptorJ1;
    delay(10);
  }

  bool estadoActualInterruptorJ2 = digitalRead(interruptorJ2);
  if (estadoActualInterruptorJ2 != estadoAnteriorInterruptorJ2) {
    Serial.println("cambio2");
    estadoAnteriorInterruptorJ2 = estadoActualInterruptorJ2;
    delay(10);
  }

  // Comandos desde Processing
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "J1HIT") {
      if (vidasP1 > 0) vidasP1--;
      juegoIniciado = true;
      actualizarPantalla();
    }
    else if (comando == "J2HIT") {
      if (vidasP2 > 0) vidasP2--;
      juegoIniciado = true;
      actualizarPantalla();
    }
    else if (comando == "REINICIAR") {
      juegoIniciado = false;
      vidasP1 = 2;
      vidasP2 = 2;
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("PREPARENSE...");
    }
    else if (comando == "LCD_VIDAS") {
      actualizarPantalla();
      juegoIniciado = true;
    }
    else if (comando == "G1HIT") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Gano Jugador 1");

      lectologo = SD.open("DATOS.TXT", FILE_WRITE);
      if (lectologo) {
        lectologo.println("GANADOR J1");
        lectologo.close();
      } else {
        lcd.setCursor(0, 1);
        lcd.print("Error SD G1");
      }
    }
    else if (comando == "G2HIT") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Gano Jugador 2");

      lectologo = SD.open("DATOS.TXT", FILE_WRITE);
      if (lectologo) {
        lectologo.println("GANADOR J2");
        lectologo.close();
      } else {
        lcd.setCursor(0, 1);
        lcd.print("Error SD G2");
      }
    }
  }

  delay(10);
}

void mostrarPantallaJuego() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P1 ");
  for (int i = 0; i < vidasP1; i++) lcd.write(byte(0));
  lcd.print("  |  ");
  lcd.print("P2 ");
  for (int i = 0; i < vidasP2; i++) lcd.write(byte(0));
}

void actualizarPantalla() {
  mostrarPantallaJuego();
}

