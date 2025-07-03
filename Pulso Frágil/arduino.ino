#include "thingProperties.h"
#define xAxis A0
#define yAxis A1
#define buttonPress 8



String input = "";



int tiempoNivel1_J1 = 0;
int tiempoNivel2_J1 = 0;
int tiempoNivel3_J1 = 0;

int tiempoNivel1_J2 = 0;
int tiempoNivel2_J2 = 0;
int tiempoNivel3_J2 = 0;

int contadorNivelesJ1 = 0;
int contadorNivelesJ2 = 0;


void setup() {



  Serial.begin(9600);
  
  pinMode(buttonPress, INPUT_PULLUP); // el botón está normalmente en HIGH



  
   initProperties();       // Inicializa Arduino Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
}

void actualizarTiemposJugador1() {
  int totalJ1 = tiempoNivel1_J1 + tiempoNivel2_J1 + tiempoNivel3_J1;

  tiemposJugador1 = 
    "Nivel 1: " + String(tiempoNivel1_J1) + " s\n" +
    "Nivel 2: " + String(tiempoNivel2_J1) + " s\n" +
    "Nivel 3: " + String(tiempoNivel3_J1) + " s\n" +
    "Total: "   + String(totalJ1)        + " s";
}


void actualizarTiemposJugador2() {
  int totalJ2 = tiempoNivel1_J2 + tiempoNivel2_J2 + tiempoNivel3_J2;

  tiemposJugador2 = 
    "Nivel 1: " + String(tiempoNivel1_J2) + " s\n" +
    "Nivel 2: " + String(tiempoNivel2_J2) + " s\n" +
    "Nivel 3: " + String(tiempoNivel3_J2) + " s\n" +
    "Total: "   + String(totalJ2)        + " s";
}



void loop() {

  ArduinoCloud.update();  // Necesario para enviar datos a la nube
  int x = analogRead(xAxis);
  int y = analogRead(yAxis);
  int btn = digitalRead(buttonPress); // LOW si está presionado
  int botonX = digitalRead(3);
  int botonReiniciar = digitalRead(5);

  
Serial.print(x);
Serial.print(",");
Serial.print(y);
Serial.print(",");
Serial.print(btn);
Serial.print(",");
Serial.print(botonX);  
Serial.print(",");
Serial.println(botonReiniciar);  



  delay(50); // puedes ajustar el delay

  if (Serial.available()) {
  input = Serial.readStringUntil('\n');
  input.trim();

  if (input.startsWith("T1J1:")) {
    tiempoNivel1_J1 = input.substring(5).toInt();
    contadorNivelesJ1++;
  } else if (input.startsWith("T2J1:")) {
    tiempoNivel2_J1 = input.substring(5).toInt();
    contadorNivelesJ1++;
  } else if (input.startsWith("T3J1:")) {
    tiempoNivel3_J1 = input.substring(5).toInt();
    contadorNivelesJ1++;
  } else if (input.startsWith("T1J2:")) {
    tiempoNivel1_J2 = input.substring(5).toInt();
    contadorNivelesJ2++;
  } else if (input.startsWith("T2J2:")) {
    tiempoNivel2_J2 = input.substring(5).toInt();
    contadorNivelesJ2++;
  } else if (input.startsWith("T3J2:")) {
    tiempoNivel3_J2 = input.substring(5).toInt();
    contadorNivelesJ2++;
  }

  // Si ya se completaron los 3 niveles del Jugador 1, actualiza la nube
  if (contadorNivelesJ1 == 3) {
    actualizarTiemposJugador1();
    contadorNivelesJ1 = 0; // resetea para que no se repita
  }

  // Si ya se completaron los 3 niveles del Jugador 2, actualiza la nube
  if (contadorNivelesJ2 == 3) {
    actualizarTiemposJugador2();
    contadorNivelesJ2 = 0;
  }}}

void onTiempoNivel1Jugador1Change()  {
  
}

void onTiempoNivel2Jugador1Change()  {

}

void onTiempoNivel3Jugador1Change()  {
}

void onTiempoNivel1Jugador2Change()  {
}


void onTiempoNivel2Jugador2Change()  {
}

void onTiemposJugador1Change()  {
}

void onTiemposJugador2Change()  {
}
