/**********************************************************************
   SEGUNDO ARDUINO – Juego + marcador persistente en Arduino Cloud
   Placa: Arduino UNO R4 WiFi
**********************************************************************/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "thingProperties.h"      // generado por Arduino Cloud

/* ----------------- LCD y botones locales ------------------------- */
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int pushVerde = 3;
const int pushRojo  = 5;

/* ----------------- Estados del juego ----------------------------- */
bool verdeListo  = false;
bool rojoListo   = false;
bool juegoActivo = false;
bool hayGanador  = false;
bool cloudSynced = false;   // true cuando la placa ya recibió los datos

/* ================================================================= */
void setup() {
  Serial.begin(9600);
  delay(1500);

  pinMode(pushVerde, INPUT_PULLUP);
  pinMode(pushRojo , INPUT_PULLUP);

  lcd.init(); lcd.backlight();

  /* -------- Conexión IoT Cloud ----------------------------------- */
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*  callback cuando se establece la conexión MQTT                  */
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::CONNECT, onCloudConnect);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();          // mantiene la sincronización

  if (!cloudSynced) return;       // espera a tener los contadores

  /* -------------- Lógica de botones ------------------------------ */
  if (!juegoActivo && !hayGanador) {
    bool pulsVerde = !digitalRead(pushVerde);
    bool pulsRojo  = !digitalRead(pushRojo);

    if (pulsVerde && !verdeListo) {
      verdeListo = true;
      lcd.setCursor(0,0); lcd.print("Verde listo     ");
      delay(300);
    }
    if (pulsRojo && !rojoListo) {
      rojoListo = true;
      lcd.setCursor(0,1); lcd.print("Rojo listo      ");
      delay(300);
    }

    if (verdeListo && rojoListo) {
      lcd.clear();
      lcd.print("  Todo listo!");
      lcd.setCursor(0,1); lcd.print(" Comenzamos... ");
      juegoActivo = true;
      delay(1500);
      lcd.clear(); lcd.print("Tira la bola");
    }
  }

  /* Reinicio después de destapar ambos LDR ------------------------ */
  if (hayGanador && !estadoLDR1 && !estadoLDR2) {
    resetJuego();
  }
  delay(100);
}

/* ===================== Callbacks de la nube ====================== */
void onCloudConnect() {
  Serial.println("Conectado y sincronizado");
  mostrarMensajeInicial();        // ya tenemos los contadores correctos
  cloudSynced = true;
}

void onEstadoLDR1Change() {       // generado por el Cloud
  if (juegoActivo && !hayGanador && estadoLDR1) {
    mostrarGanador(true);         // ganó verde
  }
}

void onEstadoLDR2Change() {
  if (juegoActivo && !hayGanador && estadoLDR2) {
    mostrarGanador(false);        // ganó rojo
  }
}

/* Al cambiar los contadores, refrescamos sólo la cifra en pantalla */
void onVerdeVictoriasChange() {
  lcd.setCursor(10,0);
  lcd.print("  ");
  lcd.setCursor(10,0);
  lcd.print(verdeVictorias);
}

void onRojoVictoriasChange() {
  lcd.setCursor(10,1);
  lcd.print("  ");
  lcd.setCursor(10,1);
  lcd.print(rojoVictorias);
}

/* ===================== Funciones auxiliares ====================== */
void mostrarGanador(bool esVerde) {
  lcd.clear();
  if (esVerde) {
    verdeVictorias++;           // valor queda guardado en la nube
    lcd.print("Gana Jug VERDE!");
  } else {
    rojoVictorias++;
    lcd.print("Gana Jug ROJO!");
  }
  hayGanador  = true;
  juegoActivo = false;
}

void resetJuego() {
  verdeListo = rojoListo = false;
  hayGanador = false;
  juegoActivo = false;
  mostrarMensajeInicial();
}

void mostrarMensajeInicial() {
  lcd.clear();
  lcd.setCursor(0,0);  lcd.print("Verde? ");
  lcd.setCursor(10,0); lcd.print(verdeVictorias);

  lcd.setCursor(0,1);  lcd.print("Rojo?  ");
  lcd.setCursor(10,1); lcd.print(rojoVictorias);
}
