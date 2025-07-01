/* ---------------------------------------------------------
   ¿Cuál LDR se cubrió?  →  envía flags al Arduino IoT Cloud
   Placa: Arduino UNO R4 WiFi
   --------------------------------------------------------- */
#include "thingProperties.h"

/* --- Pines y constantes propias ------------------------- */
const byte LDR1_PIN   = A0;
const byte LDR2_PIN   = A1;

const byte CAL_SAMPLES = 100;   // lecturas para calibrar
const int  TRIGGER1    = -100;  // umbral “tapado”
const int  TRIGGER2    = -30;   // zona muerta

int base1 = 0, base2 = 0;       // luz ambiente de cada LDR
/* -------------------------------------------------------- */

void setup() {
  // Serial para depuración
  Serial.begin(9600);
  delay(1500);                        // da tiempo a abrir el monitor

  // Funciones generadas por el Cloud
  initProperties();                   // declara las variables en red
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  /* ---------- Calibración de luz ambiente ---------------- */
  long acc1 = 0, acc2 = 0;
  for (byte i = 0; i < CAL_SAMPLES; i++) {
    acc1 += analogRead(LDR1_PIN);
    acc2 += analogRead(LDR2_PIN);
    delay(50);
  }
  base1 = acc1 / CAL_SAMPLES;
  base2 = acc2 / CAL_SAMPLES;

  Serial.print(F("Calibrado  Base1="));
  Serial.print(base1);
  Serial.print(F("  Base2="));
  Serial.println(base2);
  Serial.println(F("LDR1\tLDR2\tΔ1\tΔ2\tEstado"));
}

void loop() {
  ArduinoCloud.update();              // ¡sincroniza con la nube!

  int ldr1   = analogRead(LDR1_PIN);
  int ldr2   = analogRead(LDR2_PIN);
  int delta1 = ldr1 - base1;          // negativo ⇒ se oscureció
  int delta2 = ldr2 - base2;

  // ------------ lógica de detección ----------------------
  if (delta1 < TRIGGER1 && delta2 < TRIGGER1) {
    lDR1Tapado = true;
    lDR2Tapado = true;
  } else if (delta1 < TRIGGER2 && delta2 > TRIGGER2) {
    lDR1Tapado = true;
    lDR2Tapado = false;
  } else if (delta1 > TRIGGER2 && delta2 < TRIGGER1) {
    lDR1Tapado = false;
    lDR2Tapado = true;
  } else {
    lDR1Tapado = false;
    lDR2Tapado = false;
  }

  // ---------- depuración en Monitor Serie ----------------
  Serial.print(ldr1);   Serial.print('\t');
  Serial.print(ldr2);   Serial.print('\t');
  Serial.print(delta1); Serial.print('\t');
  Serial.print(delta2); Serial.print('\t');
  Serial.print(lDR1Tapado); Serial.print('\t');
  Serial.println(lDR2Tapado);

  delay(200);   // 0,2 s entre lecturas
}
