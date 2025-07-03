#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <DIYables_TFT_Shield.h>
#include <WiFiS3.h>

// === COLORES DEFINIDOS EN RGB ===
#define WHITE   DIYables_TFT::colorRGB(255, 255, 255)
#define BLACK   DIYables_TFT::colorRGB(0, 0, 0)
#define RED     DIYables_TFT::colorRGB(255, 0, 0)
#define GREEN   DIYables_TFT::colorRGB(0, 255, 0)
#define BLUE    DIYables_TFT::colorRGB(0, 0, 255)
#define YELLOW  DIYables_TFT::colorRGB(255, 255, 0)

// === PARÁMETROS DEL TABLERO ===
const int BLOCK_SIZE = 10;      // Tamaño de cada bloque de Tetris
const int BOARD_WIDTH = 10;     // Ancho en bloques
const int BOARD_HEIGHT = 20;    // Alto en bloques
const int ORIGIN_X = 60;        // Margen izquierdo del tablero en pantalla
const int ORIGIN_Y = 20;        // Margen superior del tablero en pantalla

// Tablero de juego y puntaje
int board[BOARD_HEIGHT][BOARD_WIDTH];
int score_tetris = 0;

// === Estructura para una pieza ===
struct Piece {
  int x, y;                    // Posición de la pieza en el tablero
  int shape[4][4];             // Forma 4x4 de la pieza
  uint16_t color;              // Color de la pieza
};

// Pieza actual y siguiente
Piece current;
Piece next;

// === Definición de formas (piezas) ===
const int SHAPE_I[4][4] = {
  {0, 0, 0, 0},
  {1, 1, 1, 1},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};
const int SHAPE_O[4][4] = {
  {0, 0, 0, 0},
  {0, 1, 1, 0},
  {0, 1, 1, 0},
  {0, 0, 0, 0}
};
const int SHAPE_T[4][4] = {
  {0, 0, 0, 0},
  {1, 1, 1, 0},
  {0, 1, 0, 0},
  {0, 0, 0, 0}
};
const int SHAPE_L[4][4] = {
  {0, 0, 0, 0},
  {1, 1, 1, 0},
  {1, 0, 0, 0},
  {0, 0, 0, 0}
};
const int SHAPE_J[4][4] = {
  {0, 0, 0, 0},
  {1, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 0}
};

// Arreglo de punteros a las formas
const int* SHAPES[] = { (int*)SHAPE_I, (int*)SHAPE_O, (int*)SHAPE_T, (int*)SHAPE_L, (int*)SHAPE_J };
const int NUM_SHAPES = sizeof(SHAPES) / sizeof(SHAPES[0]);

// === FUNCIONES DE DIBUJO ===

// Dibuja un bloque individual en pantalla
void drawBlock(DIYables_TFT* tft, int x, int y, uint16_t color) {
  tft->fillRect(ORIGIN_X + x * BLOCK_SIZE, ORIGIN_Y + y * BLOCK_SIZE,
                BLOCK_SIZE - 1, BLOCK_SIZE - 1, color);
}

// Muestra el puntaje de Tetris
void drawScoreTetris(DIYables_TFT* tft) {
  tft->fillRect(ORIGIN_X + BLOCK_SIZE * BOARD_WIDTH + 5, 80, 60, 30, BLACK);
  tft->setTextColor(YELLOW);
  tft->setTextSize(1);
  tft->setCursor(ORIGIN_X + BLOCK_SIZE * BOARD_WIDTH + 5, 100);
  tft->print("Puntaje:");
  tft->setCursor(ORIGIN_X + BLOCK_SIZE * BOARD_WIDTH + 5, 130);
  tft->print(score_tetris);
}

// Dibuja todo el tablero con bloques ocupados
void drawBoard(DIYables_TFT* tft) {
  tft->drawRect(ORIGIN_X - 1, ORIGIN_Y - 1,
                BLOCK_SIZE * BOARD_WIDTH + 2,
                BLOCK_SIZE * BOARD_HEIGHT + 2, WHITE);
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      drawBlock(tft, x, y, board[y][x] ? BLUE : BLACK);
    }
  }
}

// Dibuja una pieza completa
void drawPiece(DIYables_TFT* tft, Piece p, uint16_t color) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (p.shape[y][x]) {
        drawBlock(tft, p.x + x, p.y + y, color);
      }
    }
  }
}

// === FUNCIONES DE LÓGICA DEL JUEGO ===

// Verifica si una pieza está en posición válida
bool isValid(Piece p) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (p.shape[y][x]) {
        int nx = p.x + x;
        int ny = p.y + y;
        if (nx < 0 || nx >= BOARD_WIDTH || ny >= BOARD_HEIGHT || (ny >= 0 && board[ny][nx])) {
          return false;
        }
      }
    }
  }
  return true;
}

// Bloquea la pieza actual en el tablero
void lockPiece() {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (current.shape[y][x]) {
        int nx = current.x + x;
        int ny = current.y + y;
        if (ny >= 0 && ny < BOARD_HEIGHT && nx >= 0 && nx < BOARD_WIDTH) {
          board[ny][nx] = 1;
        }
      }
    }
  }
}

// Elimina líneas completas y suma puntaje
void clearFullLines(DIYables_TFT* tft) {
  int linesCleared = 0;
  for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
    bool full = true;
    for (int x = 0; x < BOARD_WIDTH; x++) {
      if (!board[y][x]) {
        full = false;
        break;
      }
    }
    if (full) {
      linesCleared++;
      for (int ty = y; ty > 0; ty--) {
        for (int tx = 0; tx < BOARD_WIDTH; tx++) {
          board[ty][tx] = board[ty - 1][tx];
        }
      }
      for (int tx = 0; tx < BOARD_WIDTH; tx++) board[0][tx] = 0;
      y++;
    }
  }
  if (linesCleared > 0) {
    score_tetris += linesCleared;
    drawScoreTetris(tft);
    drawBoard(tft);
  }
}

// Genera una nueva pieza aleatoria
void spawnPiece(Piece& piece) {
  memcpy(piece.shape, SHAPES[random(NUM_SHAPES)], sizeof(piece.shape));
  piece.x = 3;
  piece.y = 0;

  uint8_t r = random(50, 256);
  uint8_t g = random(50, 256);
  uint8_t b = random(50, 256);
  piece.color = DIYables_TFT::colorRGB(r, g, b);
}

// Rota la pieza 90 grados
void rotatePiece(Piece& p) {
  int temp[4][4];
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      temp[x][3 - y] = p.shape[y][x];
  memcpy(p.shape, temp, sizeof(temp));
}

// Dibuja la próxima pieza
void drawNextPiece(DIYables_TFT* tft) {
  tft->fillRect(ORIGIN_X + BLOCK_SIZE * BOARD_WIDTH + 5, 20, 50, 50, BLACK);
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (next.shape[y][x]) {
        tft->fillRect(ORIGIN_X + BLOCK_SIZE * BOARD_WIDTH + 5 + x * 10,
                      20 + y * 10, 9, 9, next.color);
      }
    }
  }
}

// === BUCLE PRINCIPAL DEL TETRIS ===
void runTetrisGame(DIYables_TFT* tft, WiFiServer* server, int gameSpeed) {
  memset(board, 0, sizeof(board));
  score_tetris = 0;
  spawnPiece(current);
  spawnPiece(next);

  tft->fillScreen(BLACK);
  drawBoard(tft);
  drawPiece(tft, current, current.color);
  drawNextPiece(tft);
  drawScoreTetris(tft);

  unsigned long lastFall = millis();
  int fastSpeed = gameSpeed / 4;

  while (true) {
    WiFiClient client = server->available();
    if (client) {
      String clientRequest = client.readStringUntil('\r');  // CAMBIADO a clientRequest
      client.flush();

      Piece moved = current;
      bool fastDrop = false;

      if (clientRequest.indexOf("DERECHA") != -1) moved.x++;
      else if (clientRequest.indexOf("IZQUIERDA") != -1) moved.x--;
      else if (clientRequest.indexOf("ABAJO") != -1) fastDrop = true;
      else if (clientRequest.indexOf("SELECT") != -1) {
        Piece rotated = current;
        rotatePiece(rotated);
        if (isValid(rotated)) {
          drawPiece(tft, current, BLACK);
          current = rotated;
          drawPiece(tft, current, current.color);
        }
      }

      if (moved.x != current.x) {
        if (isValid(moved)) {
          drawPiece(tft, current, BLACK);
          current = moved;
          drawPiece(tft, current, current.color);
        }
      }

      client.println("HTTP/1.1 200 OK");
      client.println("Connection: close");
      client.println();
      client.stop();

      if (fastDrop) lastFall = millis() - fastSpeed;
    }

    if (millis() - lastFall > gameSpeed) {
      Piece moved = current;
      moved.y++;
      if (isValid(moved)) {
        drawPiece(tft, current, BLACK);
        current = moved;
        drawPiece(tft, current, current.color);
      } else {
        lockPiece();
        clearFullLines(tft);
        current = next;
        spawnPiece(next);
        if (!isValid(current)) break;
        drawPiece(tft, current, current.color);
        drawNextPiece(tft);
      }
      lastFall = millis();
    }
  }

  tft->fillScreen(BLACK);
  tft->setTextColor(RED);
  tft->setTextSize(2);
  tft->setCursor(100, 80);
  tft->println("GAME");
  tft->setCursor(100, 120);
  tft->println("OVER");
  delay(2000);
}

#endif
