#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <DIYables_TFT_Shield.h>
#include <WiFiS3.h>

// === DEFINICIÓN DE COLORES ===
#define WHITE   DIYables_TFT::colorRGB(255, 255, 255)
#define BLACK   DIYables_TFT::colorRGB(0, 0, 0)
#define RED     DIYables_TFT::colorRGB(255, 0, 0)
#define GREEN   DIYables_TFT::colorRGB(0, 255, 0)
#define YELLOW  DIYables_TFT::colorRGB(255, 255, 0)

// === PARÁMETROS DEL JUEGO ===
#define GRID_SIZE 10
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define GAME_X 10
#define GAME_Y 30
#define GAME_WIDTH 300
#define GAME_HEIGHT 200
#define MAX_LENGTH 100
#define INIT_LENGTH 5

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
  int x, y;
};

// Variables globales del juego
Point snake[MAX_LENGTH];
int snakeLength;
Direction dir;
Point food;
bool gameOver = false;
int score = 0;

// Dibuja un bloque de la serpiente o comida
void drawSnakeBlock(DIYables_TFT* tft, int x, int y, uint16_t color) {
  tft->fillRoundRect(x, y, GRID_SIZE, GRID_SIZE, 2, color);
}

// Dibuja el borde del área de juego
void drawBorder(DIYables_TFT* tft) {
  tft->drawRect(GAME_X - 2, GAME_Y - 2, GAME_WIDTH + 4, GAME_HEIGHT + 4, WHITE);
}

// Muestra el puntaje
void drawScore(DIYables_TFT* tft) {
  tft->setTextSize(2);
  tft->fillRect(10, 0, 160, 20, BLACK);
  tft->setTextColor(YELLOW);
  tft->setCursor(10, 0);
  tft->print("Puntos: ");
  tft->print(score);
}

// Genera nueva comida en posición aleatoria
void spawnFood() {
  food.x = GAME_X + (random(GAME_WIDTH / GRID_SIZE)) * GRID_SIZE;
  food.y = GAME_Y + (random(GAME_HEIGHT / GRID_SIZE)) * GRID_SIZE;
}

// Reinicia todos los parámetros del juego
void resetGame(DIYables_TFT* tft) {
  tft->fillScreen(BLACK);
  drawBorder(tft);
  score = 0;
  drawScore(tft);

  snakeLength = INIT_LENGTH;
  for (int i = 0; i < snakeLength; i++) {
    snake[i].x = GAME_X + 50 - i * GRID_SIZE;
    snake[i].y = GAME_Y + 50;
  }
  dir = RIGHT;
  spawnFood();
  gameOver = false;
}

// Dibuja toda la serpiente
void drawSnake(DIYables_TFT* tft) {
  for (int i = 0; i < snakeLength; i++) {
    drawSnakeBlock(tft, snake[i].x, snake[i].y, GREEN);
  }
}

// Borra la cola de la serpiente
void eraseTail(DIYables_TFT* tft, Point tail) {
  drawSnakeBlock(tft, tail.x, tail.y, BLACK);
}

// Dibuja la comida
void drawFood(DIYables_TFT* tft) {
  drawSnakeBlock(tft, food.x, food.y, RED);
}

// Verifica colisiones con bordes y cuerpo
void checkCollision() {
  Point head = snake[0];
  if (head.x < GAME_X || head.x >= GAME_X + GAME_WIDTH ||
      head.y < GAME_Y || head.y >= GAME_Y + GAME_HEIGHT) {
    gameOver = true;
  }

  for (int i = 1; i < snakeLength; i++) {
    if (snake[i].x == head.x && snake[i].y == head.y) {
      gameOver = true;
    }
  }
}

// Actualiza posición de la serpiente y comida
void updateSnake(DIYables_TFT* tft) {
  Point tail = snake[snakeLength - 1];

  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  switch (dir) {
    case UP:    snake[0].y -= GRID_SIZE; break;
    case DOWN:  snake[0].y += GRID_SIZE; break;
    case LEFT:  snake[0].x -= GRID_SIZE; break;
    case RIGHT: snake[0].x += GRID_SIZE; break;
  }

  if (snake[0].x == food.x && snake[0].y == food.y) {
    if (snakeLength < MAX_LENGTH) {
      snake[snakeLength] = tail;
      snakeLength++;
    }
    score++;
    drawScore(tft);
    spawnFood();
    drawFood(tft);
  } else {
    eraseTail(tft, tail);
  }
}

// Procesa comando WiFi y cambia dirección
void processCommand(String clientRequest) {
  if (clientRequest.indexOf("ARRIBA") != -1 && dir != DOWN) dir = UP;
  else if (clientRequest.indexOf("ABAJO") != -1 && dir != UP) dir = DOWN;
  else if (clientRequest.indexOf("IZQUIERDA") != -1 && dir != RIGHT) dir = LEFT;
  else if (clientRequest.indexOf("DERECHA") != -1 && dir != LEFT) dir = RIGHT;
}

// Bucle principal del juego Snake
void runSnakeGame(DIYables_TFT* tft, WiFiServer* server, int gameSpeed) {
  resetGame(tft);
  drawFood(tft);

  unsigned long lastUpdate = millis();

  while (!gameOver) {
    WiFiClient client = server->available();
    if (client) {
      String clientRequest = client.readStringUntil('\r');  // Cambio a clientRequest
      client.flush();
      processCommand(clientRequest);
      client.println("HTTP/1.1 200 OK");
      client.println("Connection: close");
      client.println();
      client.stop();
    }

    if (millis() - lastUpdate >= gameSpeed) {
      updateSnake(tft);
      drawSnake(tft);
      checkCollision();
      lastUpdate = millis();
    }
  }

  // GAME OVER
  tft->fillScreen(BLACK);
  tft->setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;

  tft->setTextColor(RED);
  tft->getTextBounds("GAME", 0, 0, &x1, &y1, &w, &h);
  tft->setCursor((SCREEN_WIDTH - w) / 2, 60);
  tft->println("GAME");

  tft->getTextBounds("OVER", 0, 0, &x1, &y1, &w, &h);
  tft->setCursor((SCREEN_WIDTH - w) / 2, 100);
  tft->println("OVER");

  tft->setTextColor(WHITE);
  char buffer[32];
  sprintf(buffer, "Puntaje:%d", score);
  tft->getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  tft->setCursor((SCREEN_WIDTH - w) / 2, 150);
  tft->println(buffer);

  delay(3000);
}

#endif
