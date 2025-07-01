#include <Joystick.h>
#include <WiFiS3.h>

Joystick joystick(A0, A1, 2);

// WiFi
const char* ssid = "RIFT";
const char* password = "99999999";
const int port = 8080;
const IPAddress serverIP(192, 168, 254, 26);
WiFiClient client;
const int joyX = A0;
const int joyY = A1;
const int btnPin = 2;
int lastX = 50;
int lastY = 50;
bool lastBtn = HIGH;
int x;
int y;
unsigned long tAnterior = 0;
const int intervaloLectura = 10; // milisegundos


void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
  pinMode(btnPin, INPUT_PULLUP);

  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(150);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
  Serial.print("📡 IP local: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long ahora = millis();

  if (ahora - tAnterior >= intervaloLectura) {
    tAnterior = ahora;

    x = analogRead(joyX);
    y = analogRead(joyY);
    x = map(x, 0, 1024, 0, 100);
    y = map(y, 0, 1024, 0, 100);
    bool btn = digitalRead(btnPin);

    // Movimiento sensible en eje Y
    if (x > 30 && x < 70 && y >= 70) {
      sendCommand("ARRIBA");
    } else if (x > 30 && x < 70 && y <= 30) {
      sendCommand("ABAJO");
    }

    // Movimiento sensible en eje X
    if (y > 30 && y < 70 && x >= 70) {
      sendCommand("IZQUIERDA");
    } else if (y > 30 && y < 70 && x <= 30) {
      sendCommand("DERECHA");
    }

    // Botón
    if (btn == LOW && lastBtn == HIGH) {
      sendCommand("SELECT");
    }

    lastX = x;
    lastY = y;
    lastBtn = btn;
  }
}

void sendCommand(const char* cmd) {
  if (client.connect(serverIP, port)) {
    client.print(String("GET /") + cmd + " HTTP/1.1\r\nHost: pantalla\r\nConnection: close\r\n\r\n");
    Serial.print("📤 Comando enviado: ");
    Serial.println(cmd);
    client.stop();
  } else {
    Serial.println("❌ No se pudo conectar al servidor.");
  }
}