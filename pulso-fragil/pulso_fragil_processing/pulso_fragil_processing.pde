import processing.serial.*;

Serial miPuerto;
String input = "";
PGraphics trail, Triangulo, Circulo, Estrella;

int jugadorActual = 1; 
int tiempoCambioJugador = 0;
int duracionCambioJugador = 5000; 
int joyX = 512, joyY = 512;
int joycentroX = 355;
int joycentroY = 377;
int tiempoInicioNivel1, tiempoFinNivel1, tiempoNivel1;
int tiempoInicioNivel2, tiempoFinNivel2, tiempoNivel2;
int tiempoInicioNivel3, tiempoFinNivel3, tiempoNivel3;
int zonaNegra = 40;
int tamañoPunto = 10;
int nivel = 1;
int btnX, btnY, btnW = 130, btnH = 30;

float jugadorX, jugadorY;
float velocidad = 0.71;
float angleAccum3 = 0;
float prevAngle3 = 0;
float goalX1, goalY1;
float goalX2, goalY2;
float goalX3, goalY3;
float angleAccum1 = 0;
float prevAngle1 = 0;
float angleAccum = 0;
float prevAngle = 0;

boolean gameOver = false;
boolean juegoTerminado = false;
boolean botonPresionado = false;
boolean startedMoving3 = false;
boolean pasarNivel2 = false;
boolean pasarNivel3 = false;
boolean startedMoving1 = false;
boolean startedMoving = false;
boolean leftStart = false;
boolean pantallaInicio = true; 
boolean mostrarCambioJugador = false;
boolean reinicioPorCambioJugador = false;

void mostrarPortada() {
  background(0);  

  // Tamaño y posiciones para las figuras
  float centroY = height / 2;
  float sizeFig = min(width, height) / 8;
  float spacing = sizeFig * 1.5;
  float centroX = width / 2;

  // Triángulo a la izquierda
  fill(0);
  stroke(#00FFFF);
  strokeWeight(5);
  beginShape();
  vertex(centroX - spacing - sizeFig/2, centroY + sizeFig/2);
  vertex(centroX - spacing, centroY - sizeFig/2);
  vertex(centroX - spacing + sizeFig/2, centroY + sizeFig/2);
  endShape(CLOSE);

  // Círculo al centro
  noFill();
  stroke(#FFA500);
  strokeWeight(5);
  ellipse(centroX, centroY, sizeFig, sizeFig);

  // Estrella a la derecha
  noFill();
  stroke(#BA6AFF);
  strokeWeight(5);
  beginShape();
  for (int i = 0; i < 10; i++) {
    float angle = TWO_PI / 10 * i - HALF_PI;
    float r = (i % 2 == 0) ? sizeFig / 2 : sizeFig / 4;
    float x = centroX + spacing + cos(angle) * r;
    float y = centroY + sin(angle) * r;
    vertex(x, y);
  }
  endShape(CLOSE);

  // Texto y botón
  fill(255);
  noStroke();
  textAlign(CENTER, CENTER);
  textSize(40);
  text("¡Bienvenido al Juego!", width / 2, centroY - sizeFig * 1.5);
  textSize(25);
  text("Presiona el botón X para comenzar", width / 2, centroY - sizeFig * -1);

  // Botón
  fill(200);
  rect(btnX, btnY, btnW, btnH, 20);
  fill(0);
  textSize(20);
  text("INICIAR JUEGO", btnX + btnW / 2, btnY + btnH / 2);
}

void setup() {
  fullScreen();
  miPuerto = new Serial(this, Serial.list()[0], 9600);
  miPuerto.bufferUntil('\n');

  trail = createGraphics(width, height);
  Triangulo = createGraphics(width, height);
  Circulo = createGraphics(width, height);
  Estrella = createGraphics(width, height);

  textSize(20);

  goalX1 = width / 2;
  goalY1 = height / 2.2;
  goalX2 = width / 2;
  goalY2 = height / 2.1;
  goalX3 = width / 2;
  goalY3 = height / 2;

  btnX = width / 2 - 65;
  btnY = height - 100;

  dibujaTriangulo();
  dibujaCirculo();
  dibujaEstrella();

  jugadorX = goalX1;
  jugadorY = goalY1 - (min(width, height) / 6);  

  prevAngle1 = atan2(jugadorY - goalY1, jugadorX - goalX1);
  if (prevAngle1 < 0) prevAngle1 += TWO_PI;

  prevAngle = atan2(jugadorY - goalY2, jugadorX - goalX2);
  if (prevAngle < 0) prevAngle += TWO_PI;
  
  tiempoInicioNivel1 = millis();
}

void draw() {
    float centroY = height / 2;
    float dx = abs(joyX - joycentroX) > zonaNegra ? map(joyX - joycentroX, -512, 511, -velocidad, velocidad) : 0;
    float dy = abs(joyY - joycentroY) > zonaNegra ? map(joyY - joycentroY, -512, 511, velocidad, -velocidad) : 0;
    float nextX = jugadorX + dx;
    float nextY = jugadorY + dy;
  
  if (pantallaInicio) {
    mostrarPortada();
    return;
  }

if (mostrarCambioJugador) {
  background(0);
  fill(255);
  textAlign(CENTER, CENTER);
  textSize(40);
  text("Turno del Jugador 2", width / 2, height / 2);
  
  if (millis() - tiempoCambioJugador > duracionCambioJugador) {
    mostrarCambioJugador = false;
    reiniciar(); 
  }
  return;
}

  background(0);
  if (nivel == 1) image(Triangulo, 0, 0);
  else if (nivel == 2) image(Circulo, 0, 0);
  else image(Estrella, 0, 0);
  image(trail, 0, 0);

  if (juegoTerminado) {
  background(0);
  fill(255);
  textAlign(CENTER);
  textSize(30);
  text("¡Juego Terminado!", width / 2, height / 2 - 40);
  textSize(22);
  text("Presiona el botón X para volver al inicio", width / 2, height / 2 + 10);
  return;
  }
  if (gameOver) {
    mensaje("¡Perdiste! Presiona Δ para reiniciar.");
    boton("REINICIAR");
    dibujaJugador(); 
    return;
  }
  if (pasarNivel2 || pasarNivel3) {
    mensaje("¡Nivel superado!");
    fill(255);
    noStroke();
    textAlign(CENTER, CENTER);
    textSize(25);
 
  text("Presiona el botón X para continuar", width / 2, centroY * 1.5 + 20);
    boton("Siguiente Nivel");
    mostrarCronometro();
    dibujaJugador(); 
    return;
  }
  if (isOutside(nextX, nextY)) {
    gameOver = true;
    return;
  }

  jugadorX = nextX;
  jugadorY = nextY;

  if (nivel == 1) {
    float currentAngle1 = atan2(jugadorY - goalY1, jugadorX - goalX1);
    if (currentAngle1 < 0) currentAngle1 += TWO_PI;
    float delta1 = currentAngle1 - prevAngle1;
    if (delta1 > PI) delta1 -= TWO_PI;
    else if (delta1 < -PI) delta1 += TWO_PI;
    if (!startedMoving1 && delta1 > 0.001) startedMoving1 = true;
    if (startedMoving1 && delta1 > 0) angleAccum1 += delta1;
    prevAngle1 = currentAngle1;
    if (angleAccum1 >= TWO_PI) {
      pasarNivel2 = true;
      trail.beginDraw(); trail.clear(); trail.endDraw();
      tiempoFinNivel1 = millis();
tiempoNivel1 = (tiempoFinNivel1 - tiempoInicioNivel1) / 1000;
      return;
    }}

  if (nivel == 2) {
    float currentAngle = atan2(jugadorY - goalY2, jugadorX - goalX2);
    if (currentAngle < 0) currentAngle += TWO_PI;
    float delta = currentAngle - prevAngle;
    if (delta > PI) delta -= TWO_PI;
    else if (delta < -PI) delta += TWO_PI;
    if (!startedMoving && delta > 0.001) startedMoving = true;
    if (startedMoving && delta > 0) angleAccum += delta;
    prevAngle = currentAngle;
    if (angleAccum >= TWO_PI) {
      pasarNivel3 = true;
      trail.beginDraw(); trail.clear(); trail.endDraw();
      tiempoFinNivel2 = millis();
tiempoNivel2 = (tiempoFinNivel2 - tiempoInicioNivel2) / 1000;
      return;
    }}
  if (nivel == 3) {
  float currentAngle3 = atan2(jugadorY - goalY3, jugadorX - goalX3);
  if (currentAngle3 < 0) currentAngle3 += TWO_PI;
  float delta3 = currentAngle3 - prevAngle3;
  if (delta3 > PI) delta3 -= TWO_PI;
  else if (delta3 < -PI) delta3 += TWO_PI;

  if (!startedMoving3 && delta3 > 0.001) startedMoving3 = true;
  if (startedMoving3 && delta3 > 0) angleAccum3 += delta3;

  prevAngle3 = currentAngle3;

 if (angleAccum3 >= TWO_PI) {
  trail.beginDraw(); trail.clear(); trail.endDraw();
  tiempoFinNivel3 = millis();
  tiempoNivel3 = (tiempoFinNivel3 - tiempoInicioNivel3) / 1000;

  // Enviar los tiempos al Arduino
  miPuerto.write("T1J" + jugadorActual + ":" + tiempoNivel1 + "\n");
  delay(100);
  miPuerto.write("T2J" + jugadorActual + ":" + tiempoNivel2 + "\n");
  delay(100);
  miPuerto.write("T3J" + jugadorActual + ":" + tiempoNivel3 + "\n");
  delay(100);

  // Verifica si fue el segundo jugador
  if (jugadorActual == 2) {
    juegoTerminado = true;
  } else {
    jugadorActual++;
    mostrarCambioJugador = true;
    tiempoCambioJugador = millis();
    reinicioPorCambioJugador = true;
  }
  return;
  }}
  trail.beginDraw();
  trail.fill(0); trail.noStroke();
  trail.ellipse(jugadorX, jugadorY, 4, 4);
  trail.endDraw();
  mostrarCronometro();
  dibujaJugador();
}

void dibujaJugador() {
  fill(255); noStroke();
  ellipse(jugadorX, jugadorY, tamañoPunto, tamañoPunto);
}

void mensaje(String t) {
  fill(255);
  textAlign(CENTER, CENTER);
  textSize(24);
  text(t, width/2, height/7);
}

void boton(String t) {
  noStroke(); fill(200);
  rect(btnX, btnY, btnW, btnH, 20);
  fill(0); textSize(20);
  text(t, btnX + btnW/2, btnY + btnH/2);
}

boolean isOutside(float x, float y) {
  x = constrain(x, 0, width - 1);
  y = constrain(y, 0, height - 1);
  if (nivel == 1) return Triangulo.get(int(x), int(y)) == color(0);
  if (nivel == 2) return Circulo.get(int(x), int(y)) == color(0);
  return Estrella.get(int(x), int(y)) == color(0);
}

//funciones figuras
void dibujaTriangulo() {
  float cx = goalX1;
  float cy = goalY1;
  float size = min(width, height) / 3;

  Triangulo.beginDraw();
  Triangulo.background(0);
  Triangulo.stroke(#00FFFF);
  Triangulo.strokeWeight(20);
  Triangulo.noFill();
  Triangulo.beginShape();
  Triangulo.vertex(cx, cy - size / 2);
  Triangulo.vertex(cx + size / 2, cy + size / 2);
  Triangulo.vertex(cx - size / 2, cy + size / 2);
  Triangulo.endShape(CLOSE);
  Triangulo.endDraw();
}

void dibujaCirculo() {
  Circulo.beginDraw();
  Circulo.background(0);
  Circulo.stroke(#FFA500);
  Circulo.strokeWeight(20);
  Circulo.noFill();
  Circulo.ellipse(goalX2, goalY2, min(width, height)/2, min(width, height)/2);
  Circulo.endDraw();
}

void dibujaEstrella() {
  float cx = goalX3;
  float cy = goalY3;
  float size = min(width, height) / 3;

  Estrella.beginDraw();
  Estrella.background(0);
  Estrella.stroke(#BA6AFF);
  Estrella.strokeWeight(20);
  Estrella.noFill();
  Estrella.beginShape();
  for (int i = 0; i < 10; i++) {
    float angle = TWO_PI / 10 * i;
    float r = i % 2 == 0 ? size / 2 : size / 4;
    float x = cx + cos(angle - HALF_PI) * r;
    float y = cy + sin(angle - HALF_PI) * r;
    Estrella.vertex(x, y);
  }
  Estrella.endShape(CLOSE);
  Estrella.endDraw();
}

void serialEvent(Serial p) {
  input = trim(p.readStringUntil('\n'));
  if (input == null || input.length() == 0) return;

  String[] parts = split(input, ',');
  if (parts.length == 5) {  // Ahora esperamos 5 datos
      try {
      joyX = constrain(int(parts[0]), 0, 1023);
      joyY = constrain(int(parts[1]), 0, 1023);
      int botonReiniciar = int(parts[4]); // Nuevo botón en pin 5

      int botonX = int(parts[3]);  // Nuevo valor desde el pin 3

      // Inicio del juego con botón X (valor 0 cuando presionado)
      if (botonX == 0 && !botonPresionado) {
        if (pantallaInicio) {
          pantallaInicio = false;
          tiempoInicioNivel1 = millis();
          trail.beginDraw(); trail.clear(); trail.endDraw();
          botonPresionado = true;
          return;
        }

        if (pasarNivel2) {
          nivel = 2;
          pasarNivel2 = false;
          jugadorX = goalX2;
          jugadorY = goalY2 - (min(width, height) / 4);
          angleAccum = 0;
          startedMoving = false;
          tiempoInicioNivel2 = millis();
          trail.beginDraw(); trail.clear(); trail.endDraw();
        } else if (pasarNivel3) {
          nivel = 3;
          pasarNivel3 = false;
          jugadorX = goalX3;
          jugadorY = goalY3 - (min(width, height) / 5) + 30;
          angleAccum3 = 0;
          startedMoving3 = false;
          prevAngle3 = atan2(jugadorY - goalY3, jugadorX - goalX3);
          if (prevAngle3 < 0) prevAngle3 += TWO_PI;
          tiempoInicioNivel3 = millis();
          leftStart = false;
          trail.beginDraw(); trail.clear(); trail.endDraw();
        }  else if (gameOver) {
  reiniciar();
} else if (juegoTerminado) {
  pantallaInicio = true;
  nivel = 1;
  reiniciar();
}
        botonPresionado = true;
      } else if (botonX == 1) {
        botonPresionado = false;
      }
// Reiniciar el juego si se perdió, usando botón en pin 5 (triangulo)
if (botonReiniciar == 0 && gameOver) {
  reinicioPorCambioJugador = false;  // 🔒 Asegúrate de que no fuerce el nivel 1
  reiniciar();
}

} 
    catch (Exception e) {
      println("Dato corrupto: " + input);
    }}}

void mostrarCronometro() {
  int tiempoActual;

  if (nivel == 1) {
    tiempoActual = (millis() - tiempoInicioNivel1) / 1000;
  } else if (nivel == 2) {
    tiempoActual = (millis() - tiempoInicioNivel2) / 1000;
  } else {
    tiempoActual = (millis() - tiempoInicioNivel3) / 1000;
  }
  fill(255);
  textAlign(LEFT, TOP);
  textSize(25);
  text("Tiempo: " + tiempoActual + " s", 20, 20);
}

//funcion reiniciar
void reiniciar() {
  gameOver = false;
  juegoTerminado = false;
  pasarNivel2 = false;
  pasarNivel3 = false;
  leftStart = false;
  trail.beginDraw();
  trail.clear();
  trail.endDraw();

 // Reinicio completo para el nuevo jugador
  if (reinicioPorCambioJugador) {    
    nivel = 1;
    tiempoNivel1 = 0;
    tiempoNivel2 = 0;
    tiempoNivel3 = 0;
    jugadorX = goalX1;
    jugadorY = goalY1 - (min(width, height) / 6);
    angleAccum1 = 0;
    startedMoving1 = false;
    prevAngle1 = atan2(jugadorY - goalY1, jugadorX - goalX1);
    if (prevAngle1 < 0) prevAngle1 += TWO_PI;
    tiempoInicioNivel1 = millis();
    reinicioPorCambioJugador = false; // vuelve a desactivar
    return;
  } 
  // Reinicio solo del nivel actual tras perder
  if (nivel == 1) {
    jugadorX = goalX1;
    jugadorY = goalY1 - (min(width, height) / 6);
    angleAccum1 = 0;
    startedMoving1 = false;
    prevAngle1 = atan2(jugadorY - goalY1, jugadorX - goalX1);
    if (prevAngle1 < 0) prevAngle1 += TWO_PI;
    tiempoInicioNivel1 = millis();
    tiempoNivel1 = 0;
  } else if (nivel == 2) {
    jugadorX = goalX2;
    jugadorY = goalY2 - (min(width, height) / 4);
    angleAccum = 0;
    startedMoving = false;
    prevAngle = atan2(jugadorY - goalY2, jugadorX - goalX2);
    if (prevAngle < 0) prevAngle += TWO_PI;
    tiempoInicioNivel2 = millis();
    tiempoNivel2 = 0;
  } else if (nivel == 3) {
    jugadorX = goalX3;
    jugadorY = goalY3 - (min(width, height) / 5) + 30;
    angleAccum3 = 0;
    startedMoving3 = false;
    prevAngle3 = atan2(jugadorY - goalY3, jugadorX - goalX3);
    if (prevAngle3 < 0) prevAngle3 += TWO_PI;
    tiempoInicioNivel3 = millis();
    tiempoNivel3 = 0;
  }}
