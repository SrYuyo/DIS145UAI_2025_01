//JUEGO SHOT!
  //Entrega final - Diseño de Construcción de Interfaces
  //Estudiantes: Carmen Luz Giraudo y Benjamín Salas Manzo
  //Profesor: Mauricio Loyola

#include <WiFiS3.h> //librería WiFi para conectar con Arduino R4
#include <WiFiServer.h> //librería para creación de servidor web básico en Arduino

char ssid[] = "Casa-61"; //nombre red WiFi 
char pass[] = "elefantito"; //contraseña red WiFi

 //SE INICIA SERVIDOR EN PUERTO 80 CORRESPONDIENTE A HTTP, IMPORTANTE PARA CONECTARLO DSPS A LA APP
WiFiServer server(80);

//PINES CONECTADOS A LEDS
int leds[] = {A0, A2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A1}; 

//LIMITAMOS REPETICIONES DE LOS LEDS SELECCIONADOS DURANTE EL JUEGO
int numLeds = 13;
int conteoLeds[13] = {0};
const int maxRepeticiones = 4; // para controlar número máx de veces (4) que se puede repetir un LED antes de reiniciar la ruleta

//CONTROLAMOS DURACIÓN DE SECUENCIA DE LEDS 
unsigned long startTime;
unsigned long stopTime;
bool detenido = true; //para saber si se detuvo ruleta de LEDs
int ledSeleccionado = -1; //almacena led del momento seleccionado. El -1 indica que NO hay LED seleccionado todavía

//PINES CONECTADOS A BOTONES
const int botonPlay = 2;
const int botonReset = A3;

void setup() {
  Serial.begin(115200); //se inicializa comunicación serial

//CONFIGURACIÓN DE PINES DE LEDS COMO SALIDA
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }

//CONFIGURACIÓN DE PINES DE ENTRADA COMO BOTONES (CON RESISTENCIA PULL UP)
  pinMode(botonPlay, INPUT_PULLUP);
  pinMode(botonReset, INPUT_PULLUP);

//FUNCIÓN PARA CONECTAR A RED WIFI
  conectarWifi();

//INICIAMOS SERVIDOR SI CONEXIÓN WIFI FUE EFECTIVA
  if (WiFi.status() == WL_CONNECTED) {
    server.begin();
    Serial.println("Servidor iniciado, conexión WiFi efectiva");
  } else {
    Serial.println("No se inició el servidor por fallo de conexión WiFi");
  }
}

void loop() {
  //PARA VERIFICAR QUE SE HAYA CONECTADO AL SERVIDOR
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Cliente conectado");

    //PARA LEER SOLICITUD DE QUIEN SE CONECTÓ AL SERVIDOR
    String request = client.readStringUntil('\r');
    Serial.print("Petición recibida: ");
    Serial.println(request);
    client.flush();

    //SOLICITUD QUE SERÍA OBTENER EL LED ACTUAL SELECCIONADO EN LA RULETA
    if (request.indexOf("GET /led") >= 0) {
      Serial.print("Enviando LED seleccionado: ");
      Serial.println(ledSeleccionado);

      //RESPUESTA HTTP CON NÚMERO DEL LED OBTENIDO SELECCIONADO
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println(ledSeleccionado);
    }

    delay(10); 
    client.stop(); //finaliza desconexión con servidor
    Serial.println("Cliente desconectado\n");
  }

//SI SE PRESIONA BOTÓN RESET PARA REINICIAR JUEGO
  if (digitalRead(botonReset) == LOW) {
    delay(200);
    parpadeoLeds(); //se llama a función para animación de parpadeo de LEDs, los cuales parpadean al mismo tiempo 3 veces para indicar que se reinició juego
    ledSeleccionado = -1; //informa a la app que se reinició el juego, por ende, que no hay 1 LED seleccionado
  }

//SI SE PRESIONA BOTÓN PLAY Y JUEGO ESTÁ APAGADO/DETENIDO
  if (digitalRead(botonPlay) == LOW && detenido) {
    delay(200);
    iniciarSecuencia(); //se llama a función para iniciar encendido de secuencia/ruleta de LEDs
  }

//SI ESTÁ DURANTE LA SECUENCIA/RULETA DE LEDS
  if (!detenido) {
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(leds[i], HIGH);
      delay(30);
      digitalWrite(leds[i], LOW);
      delay(10);

      if (millis() >= stopTime) {
        detenido = true; //se detiene secuencia/ruleta de LEDs
        seleccionarLed(); //se llama función que selecciona LED final (que cumple con condición de no haberse repetido más de 4 veces) donde se detuvo la ruleta
        break;
      }
    }
  } else {
    //APAGAR TODOS LOS OTROS LEDS DE LA RULETA
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(leds[i], LOW);
    }
    //Y QUE QUEDE SOLO ENCENDIDO EL LED FINAL 
    if (ledSeleccionado >= 0 && ledSeleccionado < numLeds) {
      digitalWrite(leds[ledSeleccionado], HIGH);
    }
  }
}

//FUNCIÓN QUE INICIA SECUENCIA/RULETA DE LEDS
void iniciarSecuencia() {
  detenido = false;
  startTime = millis(); //tiempo de inicio
  stopTime = startTime + random(3000, 6000); //la secuencia/ruleta gira en una duración aleatoria entre 3 y 6 seg
}

//FUNCIÓN QUE HACE PARPADEAR 3 VECES LEDS DE RULETA PARA INDICAR REINICIO DEL JUEGO
void parpadeoLeds() {
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(leds[i], HIGH);
    }
    delay(600);
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(leds[i], LOW);
    }
    delay(600);
  }
}
//FUNCIÓN QUE SELECCIONA LED ALEATORIO DE LA RULETA DE LEDs QUE NO HAYA ALCANZADO EL N° MÁX (4) DE REPETICIONES
void seleccionarLed() {
  int intentos = 0;
  do {
    ledSeleccionado = random(0, numLeds); //selecciona un LED al azar
    intentos++;
    if (intentos > 100) { //si no encuentra LED que no haya alcanzado con el máx de repeticiones, dsps de 100 intentos fallidos, reinicia conteo. Se evita bucle infinito cuando ya no hay LEDs disponibles por haber llegado al máx de repeticiones.
      for (int i = 0; i < numLeds; i++) {
        conteoLeds[i] = 0;
      }
      break;
    }
  } while (conteoLeds[ledSeleccionado] >= maxRepeticiones);

  conteoLeds[ledSeleccionado]++;
}

//FUNCIÓN QUE CONECTA ARDUINO A RED WiFi
void conectarWifi() {
  WiFi.begin(ssid, pass);
  Serial.print("Conectando a WiFi");

//ESPERA HASTA 15 SEG PARA CONECTARSE A RED
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    IPAddress ip = WiFi.localIP();
    //ESPERA HASTA QUE SE OBTENGA UNA IP VÁLIDA QUE SIRVA PARA CONECTARLA A LA APP DSPS
    while (ip[0] == 0) {
      Serial.println("Esperando IP válida...");
      delay(500);
      ip = WiFi.localIP();
    }

    Serial.println("Conectado a WiFi");
    Serial.print("IP asignada: ");
    Serial.println(ip);
  } else {
    Serial.println("No se pudo conectar al WiFi");
    Serial.print("Estado WiFi: ");
    Serial.println(WiFi.status());
  }
}