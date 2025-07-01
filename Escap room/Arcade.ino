//Librerías
#include <WiFiS3.h>                       // Wifi
#include <DIYables_TFT_Shield.h>          // Pantalla
#include "VCR_OSD_MONO_1_00115pt7b.h"     // Fuente de texto icónica
#include "snake_game.h"                   // Juego Snake
#include "tetris_game.h"                  // Juego Tetris
#include "Secreto.h"                     // Secreto

//WiFi
const char* ssid = "RIFT";                // Nombre de la red WiFi
const char* password = "99999999";        // Contraseña
WiFiServer server(8080);                  // Servidor en puerto 8080

//Define pantalla
DIYables_TFT_ILI9486_Shield TFT_display;  // Objeto para manejar la pantalla TFT

//Define menú
const char* menuItems[] = {"SNAKE", "TETRIS", "SALIR", "Secreto"}; // Array con las opciones
int selected = 0;                         // Opción seleccionada
const int totalItems = sizeof(menuItems) / sizeof(menuItems[0]); // Número total de ítems
bool inGame = false;                      // Indica si se está ejecutando un juego


//Control de entrada fluido
unsigned long lastInputTime = 0;          // Última vez que se detectó una entrada válida
const int inputDelay = 40;                // Retardo mínimo entre entradas para evitar rebotes

//Uso de punteros
//A recomendación de ChatGPT para hacer uso eficiente de la pantalla TFT usó punteros
//para agilizar el uso de recursos dentro del código sin hacer uso exhaustivo de la memoria flotante

//Colores
#define WHITE     DIYables_TFT::colorRGB(255, 255, 255)
#define BLACK     DIYables_TFT::colorRGB(0, 0, 0)
#define YELLOW    DIYables_TFT::colorRGB(255, 255, 0)
#define RED       DIYables_TFT::colorRGB(255, 0, 0)
#define BLUE      DIYables_TFT::colorRGB(0, 0, 255)

void setup() {
  Serial.begin(9600);                     

  TFT_display.setFont(&VCR_OSD_MONO_1_00115pt7b);  
  TFT_display.begin();                    // Inicializa pantalla
  TFT_display.setRotation(1);             // Orientación horizontal

  showSplash();                           // Muestra pantalla de bienvenida

  WiFi.begin(ssid, password);             // Conecta al WiFi
  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) { // Espera hasta conectar
    delay(100);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado.");    // /n para poner el mensajo debajo de los puntos suspensivos
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());         // Muestra IP asignada

  server.begin();                         // Inicia el servidor
  drawCarouselMenu(true);                 // Dibuja el menú por primera vez
}

void loop() {
  if (inGame) return;                     // Si hay un juego en ejecución, no procesa entradas

  WiFiClient client = server.available(); // Espera a un cliente WiFi
  if (client) {
    String clientRequest = client.readStringUntil('\r'); // Espera a recibir señal
    client.flush();                      // Limpia el buffer de espera

    unsigned long now = millis();        // Tiempo actual
    bool updated = false;                // Variable para saber si se debe actualizar el menú

    // Movimiento a la derecha
    if (clientRequest.indexOf("DERECHA") != -1 && now - lastInputTime > inputDelay) { // el "-1" es un valor propio de la función indexOf, es el valor
                                                                                      //que retorna cuando no encuentra el comando que se busca
      selected = (selected - 1 + totalItems) % totalItems;  // Cambia selección, el uso de "%" es para generar un bucle que vaya utilizando de índice
                                                            // el resto de la división, de este modo cuando llegue al total, el resultado será 1, resto 0
                                                            // y vuelve al inicio de las opciones
      updated = true;                           // Reproduce beep
    } 
    // Movimiento a la izquierda
    else if (clientRequest.indexOf("IZQUIERDA") != -1 && now - lastInputTime > inputDelay) {
      selected = (selected + 1) % totalItems;               // Cambia selección (% es un int)
      updated = true;
    } 
    // Selecciona la opción actual
    else if (clientRequest.indexOf("SELECT") != -1) {
      if (selected == 0) {                // Opción Snake
        inGame = true;
        runSnakeGame(&TFT_display, &server, 250); // Llama a la función Snake
        inGame = false;
        drawCarouselMenu(true);
      } else if (selected == 1) {         // Opción Tetris
        inGame = true;
        runTetrisGame(&TFT_display, &server, 250); 
        inGame = false;
        drawCarouselMenu(true);
      } else if (selected == 2) {         // Opción Salir
        apagarArcade();                   // Muestra pantalla de apagado
      } else if (selected == 3){
        secreto();
        drawCarouselMenu(true);
      }
      else {                            
        showSelection();                  // Muestra texto de selección
      }
    }

    if (updated) {
      drawCarouselMenu(false);            // Redibuja menú si cambió selección
      lastInputTime = now;                // Actualiza última entrada
    }

    // Responde al cliente
    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
}

//Imagen de inicio
void showSplash() {
  TFT_display.fillScreen(BLUE);           // Fondo azul
  TFT_display.setTextColor(YELLOW);       // Texto amarillo
  TFT_display.setTextSize(3);             // Tamaño grande

  int16_t x1, y1;
  uint16_t w, h;
  TFT_display.getTextBounds("ARCADE", 0, 0, &x1, &y1, &w, &h);
  int x_center = (320 - w) / 2;           // Centra el texto horizontalmente

  TFT_display.setCursor(x_center, 100);   // Coloca cursor
  TFT_display.println("ARCADE");          
  delay(1500);                            
  TFT_display.fillScreen(WHITE);          // Termina
}

//Dibuja el menú carrusel
void drawCarouselMenu(bool fullRedraw) {
  if (fullRedraw) {
    TFT_display.fillScreen(WHITE);        // Fondo
    TFT_display.fillRect(0, 40, 70, 160, BLACK);   // Cuadro izquierdo
    TFT_display.fillRect(250, 40, 160, 160, BLACK); // Cuadro derecho
  }

  TFT_display.fillRect(80, 40, 160, 160, BLACK);  // Cuadro central para texto
  TFT_display.setTextColor(WHITE);        // Texto blanco
  TFT_display.setTextSize(1);

  int16_t x1, y1;
  uint16_t w, h;
  TFT_display.getTextBounds(menuItems[selected], 0, 0, &x1, &y1, &w, &h); //Genera coordenadas para las esquinas imaginarias del texto, como un bounding box
  int x_center = (320 - w) / 2;           // Centra texto
  TFT_display.setCursor(x_center, 110);
  TFT_display.println(menuItems[selected]);

  // Flecha izquierda
  uint16_t leftColor;
  if (selected > 0) {
  leftColor = YELLOW;
  TFT_display.fillTriangle(10, 120, 40, 160, 40, 80, leftColor);
  leftColor = WHITE;
  TFT_display.fillTriangle(10, 120, 40, 160, 40, 80, leftColor);
  } 
  // Flecha derecha
  uint16_t rightColor;
  if (selected < totalItems - 1) {
  rightColor = YELLOW;
  TFT_display.fillTriangle(310, 120, 280, 160, 280, 80, rightColor);
  rightColor = WHITE;
  TFT_display.fillTriangle(310, 120, 280, 160, 280, 80, rightColor);}
}

void showSelection() {
  TFT_display.fillRect(0, 200, 320, 40, YELLOW); // Barra inferior amarilla
  TFT_display.setCursor(10, 210);
  TFT_display.setTextColor(BLACK);
  TFT_display.setTextSize(1);
  TFT_display.print("Seleccionado: ");
  TFT_display.println(menuItems[selected]);
}

void apagarArcade() {
  TFT_display.fillScreen(BLACK);          // Fondo negro
  TFT_display.setTextColor(RED);          // Texto rojo
  TFT_display.setTextSize(2);

  const char* linea1 = "Apagando";
  const char* linea2 = "Arcade";
  int16_t x1, y1;
  uint16_t w, h;

  TFT_display.getTextBounds(linea1, 0, 0, &x1, &y1, &w, &h);
  TFT_display.setCursor((320 - w) / 2, 90);
  TFT_display.println(linea1);

  TFT_display.getTextBounds(linea2, 0, 0, &x1, &y1, &w, &h);
  TFT_display.setCursor((320 - w) / 2, 130);
  TFT_display.println(linea2);

  TFT_display.fillScreen(BLACK);          // Pantalla negra final
  while (true);                           // Detiene el programa
}



void secreto(){
int img_width = 160;
int img_height = 160;
  TFT_display.drawRGBBitmap(80, 40, Secreto, img_width, img_height);
  delay(800);
}