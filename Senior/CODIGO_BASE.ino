#include <MCUFRIEND_kbv.h>
#include <SD.h>

MCUFRIEND_kbv tft;
File bmpFile;

const int pinPot = A5;
const int pinBotonI = 0;
const int pinBotonD = 1;

// Variables generales
int lecturaPot;
int porcentaje;
int lecturaBotonI;
int lecturaBotonD;
int estadoAnteriorBotonI = 0;
int estadoAnteriorBotonD = 0;
int estadoPotAnterior = 0;
bool opcionSeleccionada = false;
bool pantallaActiva = false;
bool esperandoConfirmacion = false;

// Juego de reacción
int nivel = 1;
float velocidad = 5.0;
bool jugando = true;
int yLinea = 120;
int puntoRojoX;
int barraX;
bool direccion = true;
unsigned long tiempoInicioRonda = 0;
bool puedePresionar = false;
int estadoBotonJuegoAnteriorI = HIGH;
int estadoBotonJuegoAnteriorD = HIGH;
int xObjetivoInicio;
int xObjetivoFin;
int colorObjetivo;
int vidas = 3;
int modoJuego = 0;  // 0: no seleccionado, 1: práctica, 2: reto
bool seleccionandoModo = false;
int estadoPotModoAnterior = 0;
bool opcionModoSeleccionada = false;

// Juego de memoria
bool seleccionandoModoMemoria = false;
bool opcionModoMemoriaSeleccionada = false;
int estadoPotModoMemoriaAnterior = 0;

// Constantes para imagen
#define BUFFPIXEL 20

// Prototipos
void mostrarCuadro(int colorID);
void mostrarAvisoInversion();
void mostrarMensaje(const char* texto);


  void setup() {
    Serial.begin(9600);

    // Configuración pines botones con resistencia pull-up interna
    pinMode(pinBotonI, INPUT_PULLUP);
    pinMode(pinBotonD, INPUT_PULLUP);

    // Inicialización pantalla TFT
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    tft.begin(ID);
    tft.setRotation(1);

    mostrarPantallaApagada();  // Pantalla inicial apagada

    if (!SD.begin(10)) {
      Serial.println("Error al inicializar SD");
      while (1)
        ;
    }
  }

void loop() {
  // Leer entradas
  lecturaPot = analogRead(pinPot);
  porcentaje = map(lecturaPot, 0, 1023, 0, 100);
  lecturaBotonI = digitalRead(pinBotonI);
  lecturaBotonD = digitalRead(pinBotonD);

  if (!pantallaActiva) {
    if (lecturaBotonI == LOW && estadoAnteriorBotonI == HIGH) {
      imagen("portada.bmp");
      delay(1000);
      tft.fillScreen(0x0000);
      opcionSeleccionada = false;
      estadoPotAnterior = 0;
      dibujaMenuPrincipal(estadoPotAnterior);
      pantallaActiva = true;
    }
    estadoAnteriorBotonI = lecturaBotonI;
    return;  // evita que corra más código antes de activar pantalla
  }

  // Manejo del menú de selección de modo de juego
  if (seleccionandoModo) {
    static bool botonFueSoltado = false;
    int estadoPotModoActual = (porcentaje < 50) ? 0 : 1;

    // Actualizar menú si potenciómetro cambió y no se ha confirmado modo
    if (estadoPotModoActual != estadoPotModoAnterior && !opcionModoSeleccionada) {
      estadoPotModoAnterior = estadoPotModoActual;
      submenuReaccion(estadoPotModoAnterior);
    }

    // Detectar si botón derecho fue soltado
    if (lecturaBotonD == HIGH) {
      botonFueSoltado = true;
    }

    // Confirmar selección con botón derecho presionado después de soltar
    if (lecturaBotonD == LOW && estadoAnteriorBotonD == HIGH && !opcionModoSeleccionada && botonFueSoltado) {
      opcionModoSeleccionada = true;
      modoJuego = estadoPotModoAnterior + 1;  // 1 = práctica, 2 = reto
      seleccionandoModo = false;
      delay(300);

      if (modoJuego == 1) juegoReaccionPractica();
      else if (modoJuego == 2) juegoReaccionReto();

      return;
    }

    // Volver atrás con botón izquierdo
    if (lecturaBotonI == LOW && estadoAnteriorBotonI == HIGH && !opcionModoSeleccionada) {
      seleccionandoModo = false;
      esperandoConfirmacion = false;
      opcionSeleccionada = false;
      tft.fillScreen(0x0000);
      dibujaMenuPrincipal(estadoPotAnterior);
      return;
    }

    // Actualizar estados previos botones
    estadoAnteriorBotonD = lecturaBotonD;
    estadoAnteriorBotonI = lecturaBotonI;
    return;
  }

  // Continuar con la siguiente fase si no en selección de modo
  menuPrincipal();
}

// Muestra pantalla inicial apagada con instrucción para activar
void mostrarPantallaApagada() {
  tft.fillScreen(0x0000);
  tft.setTextSize(2);

  const char* parte1 = "Presiona boton";
  int anchoParte1 = strlen(parte1) * 12;
  int xParte1 = (320 - anchoParte1 - (5 * 12)) / 2;
  tft.setTextColor(0xFFFF);
  tft.setCursor(xParte1, 100);
  tft.print(parte1);

  tft.setTextColor(0xF800);
  tft.print(" rojo");

  const char* parte2 = "para encender";
  int anchoParte2 = strlen(parte2) * 12;
  int xParte2 = (320 - anchoParte2) / 2;
  tft.setTextColor(0xFFFF);
  tft.setCursor(xParte2, 130);
  tft.print(parte2);

  delay(2000);
  tft.fillScreen(0x0000);
}

// Función que maneja la interacción básica del menú principal
void menuPrincipal() {
  // Detecta cambio en botón izquierdo
  if (lecturaBotonI != estadoAnteriorBotonI) {
    if (lecturaBotonI == HIGH) {
      if (esperandoConfirmacion) {
        esperandoConfirmacion = false;
        opcionSeleccionada = false;
        tft.fillRect(0, 200, 320, 40, 0x0000);
        dibujaMenuPrincipal(estadoPotAnterior);
      }
    }
    estadoAnteriorBotonI = lecturaBotonI;
  }

  // Detecta cambio en botón derecho
  if (lecturaBotonD != estadoAnteriorBotonD) {
    if (lecturaBotonD == HIGH && !opcionSeleccionada) {
      opcionSeleccionada = true;
      confirmarSeleccion(estadoPotAnterior);
    }
    estadoAnteriorBotonD = lecturaBotonD;
  }

  // Determina posición actual del potenciómetro en 3 rangos
  int estadoPotActual;
  if (porcentaje < 33) estadoPotActual = 0;
  else if (porcentaje < 66) estadoPotActual = 1;
  else estadoPotActual = 2;

  // Actualiza menú si cambia la posición y no se ha seleccionado opción
  if (estadoPotActual != estadoPotAnterior && !opcionSeleccionada) {
    estadoPotAnterior = estadoPotActual;
    dibujaMenuPrincipal(estadoPotActual);
  }

  delay(100);
}

// Dibuja el menú principal con la opción resaltada
void dibujaMenuPrincipal(int opcion) {
  // Limpia área de opciones
  tft.fillRect(0, 70, 320, 40, 0x0000);
  tft.fillRect(0, 110, 320, 40, 0x0000);
  tft.fillRect(0, 150, 320, 40, 0x0000);

  tft.setTextSize(2);
  tft.setTextColor(0xFFFF);
  tft.setCursor(40, 30);
  tft.print("ELIGE EL JUEGO");

  // Resalta opción actual
  if (opcion == 0) tft.fillRect(0, 70, 320, 40, 0x001F);
  else if (opcion == 1) tft.fillRect(0, 110, 320, 40, 0x001F);
  else if (opcion == 2) tft.fillRect(0, 150, 320, 40, 0x001F);

  // Escribe las opciones
  tft.setCursor(10, 80);
  tft.print("   Juego de Reaccion");
  tft.setCursor(10, 120);
  tft.print("   Juego Memorice");
  tft.setCursor(10, 160);
  tft.print("   Apagar");
}

// Muestra el menú para seleccionar modalidad del juego reacción
void submenuReaccion(int opcion) {
  // Limpia las opciones
  tft.fillRect(0, 70, 320, 40, 0x0000);
  tft.fillRect(0, 110, 320, 40, 0x0000);

  tft.setTextSize(2);
  tft.setTextColor(0xFFFF);
  tft.setCursor(20, 30);
  tft.print("SELECCIONE MODALIDAD");

  // Resalta opción seleccionada y ajusta color texto
  if (opcion == 0) {
    tft.fillRect(0, 70, 320, 40, 0x001F);
    tft.setTextColor(0xFFFF);
  } else tft.setTextColor(0xFFFF);
  tft.setCursor(10, 80);
  tft.print("   1. Practica");

  if (opcion == 1) {
    tft.fillRect(0, 110, 320, 40, 0x001F);
    tft.setTextColor(0xFFFF);
  } else tft.setTextColor(0xFFFF);
  tft.setCursor(10, 120);
  tft.print("   2. Reto");
}

// Muestra mensaje de confirmación según opción elegida
void confirmarSeleccion(int opcion) {
  esperandoConfirmacion = true;

  tft.fillRect(0, 200, 320, 40, 0xFFFF);
  tft.setCursor(10, 210);
  tft.setTextColor(0x0000);
  tft.setTextSize(2);

  switch (opcion) {
    case 0:
      tft.print("Seleccionado: Reaccion");
      delay(1000);
      seleccionandoModo = true;
      estadoPotModoAnterior = 0;
      opcionModoSeleccionada = false;
      tft.fillScreen(0x0000);
      submenuReaccion(estadoPotModoAnterior);
      break;
    case 1:
      tft.print("Seleccionado: Memorice");
      delay(1000);
      juegoMemoria();
      break;
    case 2:
      tft.print("Apagando...");
      delay(1000);
      pantallaActiva = false;
      esperandoConfirmacion = false;
      mostrarPantallaApagada();
      break;
  }
}

// Modo práctica: jugador mueve barra con potenciómetro y presiona botón correcto
void juegoReaccionPractica() {
  nivel = 1;
  vidas = 3;
  jugando = true;
  int rondasJugadas = 0;
  int barraXAnterior = -1;
  tiempoInicioRonda = millis();
  puedePresionar = false;

  while (jugando && rondasJugadas < 3) {
    tft.fillScreen(0x0000);

    // Configurar zona objetivo aleatoria y color
    int anchoZona = 100;
    int centroX = random(60, 260);
    xObjetivoInicio = centroX - anchoZona / 2;
    xObjetivoFin = centroX + anchoZona / 2;
    colorObjetivo = random(0, 2) == 0 ? 0xF800 : 0x001F;

    int corcheteAltura = 10;
    int separacionY = 25;

    // Dibujar zona objetivo (corchetes)
    tft.fillRect(xObjetivoInicio, yLinea - separacionY, anchoZona, corcheteAltura, colorObjetivo);
    tft.fillRect(xObjetivoInicio, yLinea + separacionY - corcheteAltura, anchoZona, corcheteAltura, colorObjetivo);

    // Mostrar nivel y vidas
    tft.setTextColor(0xFFFF);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Nivel: ");
    tft.print(nivel);

    for (int i = 0; i < 3; i++) {
      tft.setTextSize(2);
      tft.setTextColor(i < vidas ? 0xF800 : 0x7BEF);
      tft.setCursor(200 + i * 20, 10);
      tft.print((char)3);  // Corazón
    }

    // Leer posición inicial de la barra
    lecturaPot = analogRead(pinPot);
    barraX = map(lecturaPot, 0, 1023, 0, 319);
    barraXAnterior = barraX;
    tft.fillRect(barraX - 7, yLinea - 7, 15, 15, 0xFFFF);  // Dibujar barra blanca

    bool rondaActiva = true;

    while (rondaActiva && jugando) {
      // Permitir presionar botón tras 500 ms
      if (!puedePresionar && millis() - tiempoInicioRonda > 500) {
        puedePresionar = true;
      }

      if (rondaActiva) {
        // Actualizar posición barra si cambió el potenciómetro
        lecturaPot = analogRead(pinPot);
        barraX = map(lecturaPot, 0, 1023, 0, 319);

        if (barraX != barraXAnterior) {
          tft.fillRect(barraXAnterior - 7, yLinea - 7, 15, 15, 0x0000);  // Borrar barra anterior
          tft.fillRect(barraX - 7, yLinea - 7, 15, 15, 0xFFFF);          // Dibujar nueva barra
          barraXAnterior = barraX;
        }
      }

      // Leer botones
      lecturaBotonI = digitalRead(pinBotonI);
      lecturaBotonD = digitalRead(pinBotonD);

      bool botonRojoPresionado = lecturaBotonI == LOW && estadoBotonJuegoAnteriorI == HIGH;
      bool botonAzulPresionado = lecturaBotonD == LOW && estadoBotonJuegoAnteriorD == HIGH;

      if (puedePresionar) {
        if ((colorObjetivo == 0xF800 && botonRojoPresionado) || (colorObjetivo == 0x001F && botonAzulPresionado)) {
          verificarPractica(rondaActiva);
        } else if (botonRojoPresionado || botonAzulPresionado) {
          vidas--;
          if (vidas <= 0) jugando = false;

          tft.fillScreen(0x0000);
          const char* mensaje = "Color incorrecto!";
          int anchoTexto = strlen(mensaje) * 12;
          int xTexto = (320 - anchoTexto) / 2;
          int yTexto = (240 - 16) / 2;

          // Parpadeo mensaje error
          for (int i = 0; i < 2; i++) {
            tft.setTextColor(0xF800);
            tft.setTextSize(2);
            tft.setCursor(xTexto, yTexto);
            tft.print(mensaje);
            delay(300);
            tft.setTextColor(0x0000);
            tft.setCursor(xTexto, yTexto);
            tft.print(mensaje);
            delay(300);
          }

          rondaActiva = false;
        }
      }

      estadoBotonJuegoAnteriorI = lecturaBotonI;
      estadoBotonJuegoAnteriorD = lecturaBotonD;

      delay(30);
    }

    if (jugando) nivel++;
    rondasJugadas++;

    // Actualizar barra tras ronda
    lecturaPot = analogRead(pinPot);
    barraX = map(lecturaPot, 0, 1023, 0, 319);

    if (barraX != barraXAnterior) {
      tft.fillRect(barraXAnterior - 7, yLinea - 7, 15, 15, 0x0000);
      tft.fillRect(barraX - 7, yLinea - 7, 15, 15, 0xFFFF);
      barraXAnterior = barraX;
    }

    lecturaBotonI = digitalRead(pinBotonI);
    lecturaBotonD = digitalRead(pinBotonD);

    bool botonRojoPresionado = lecturaBotonI == LOW && estadoBotonJuegoAnteriorI == HIGH;
    bool botonAzulPresionado = lecturaBotonD == LOW && estadoBotonJuegoAnteriorD == HIGH;

    if (puedePresionar) {
      if ((colorObjetivo == 0xF800 && botonRojoPresionado) || (colorObjetivo == 0x001F && botonAzulPresionado)) {
        verificarPractica(rondaActiva);
      }
    }

    estadoBotonJuegoAnteriorI = lecturaBotonI;
    estadoBotonJuegoAnteriorD = lecturaBotonD;

    delay(30);
  }

  nivel++;

  // Mensaje fin modo práctica
  tft.fillScreen(0x0000);
  const char* mensaje = "Fin modo practica";
  int anchoTexto = strlen(mensaje) * 12;
  int xTexto = (320 - anchoTexto) / 2;
  int yTexto = 100;

  tft.fillRect(xTexto - 10, yTexto - 10, anchoTexto + 20, 30, 0xFFFF);  // Recuadro blanco
  tft.setTextColor(0x0000);
  tft.setTextSize(2);
  tft.setCursor(xTexto, yTexto);
  tft.print(mensaje);

  delay(2000);

  // Volver a selección de modo
  seleccionandoModo = true;
  opcionModoSeleccionada = false;
  estadoPotModoAnterior = 0;
  estadoAnteriorBotonD = digitalRead(pinBotonD);
  tft.fillScreen(0x0000);
  submenuReaccion(estadoPotModoAnterior);

  modoJuego = 0;
  opcionModoSeleccionada = false;
  estadoPotModoAnterior = 0;
}

// Verifica si la barra está dentro de la zona objetivo en modo práctica
void verificarPractica(bool& rondaActiva) {
  if (barraX >= xObjetivoInicio && barraX <= xObjetivoFin) {
    rondaActiva = false;
  } else {
    vidas--;
    if (vidas <= 0) {
      jugando = false;
      rondaActiva = false;
      tft.fillScreen(0x0000);
      tft.setCursor(50, 100);
      tft.setTextColor(0xF800);
      tft.setTextSize(3);
      tft.print("Sin vidas!");
      delay(2000);
    } else {
      // Mostrar "Vida perdida" parpadeando
      tft.fillScreen(0x0000);
      const char* mensaje = "Vida perdida!";
      int anchoTexto = strlen(mensaje) * 12;
      int xTexto = (320 - anchoTexto) / 2;
      int yTexto = (240 - 16) / 2;

      for (int i = 0; i < 3; i++) {
        tft.setTextColor(0xF800);
        tft.setTextSize(2);
        tft.setCursor(xTexto, yTexto);
        tft.print(mensaje);
        delay(300);
        tft.setTextColor(0x0000);
        tft.setCursor(xTexto, yTexto);
        tft.print(mensaje);
        delay(300);
      }

      rondaActiva = false;
    }
  }
}

// Modo reto: barra se mueve automáticamente, jugador debe presionar en zona correcta
void juegoReaccionReto() {
  nivel = 1;
  velocidad = 5.0;
  vidas = 3;
  jugando = true;
  estadoBotonJuegoAnteriorI = HIGH;
  estadoBotonJuegoAnteriorD = HIGH;

  nuevaRonda();

  while (jugando) {
    if (!puedePresionar && millis() - tiempoInicioRonda > 500) {
      puedePresionar = true;
    }

    int barraXAnterior = barraX;

    // Mover barra según dirección
    if (direccion) barraX += 2;
    else barraX -= 2;

    // Cambiar dirección si bordes alcanzados
    if (barraX >= 315) direccion = false;
    if (barraX <= 5) direccion = true;

    // Actualizar pantalla: borrar barra vieja y dibujar nueva
    tft.fillRect(barraXAnterior - 7, yLinea - 7, 15, 15, 0x0000);
    tft.fillRect(barraX - 7, yLinea - 7, 15, 15, 0xFFFF);

    lecturaBotonI = digitalRead(pinBotonI);
    lecturaBotonD = digitalRead(pinBotonD);

    if (puedePresionar) {
      if ((lecturaBotonI == LOW && estadoBotonJuegoAnteriorI == HIGH) || (lecturaBotonD == LOW && estadoBotonJuegoAnteriorD == HIGH)) {
        verificarReto();
      }
    }

    estadoBotonJuegoAnteriorI = lecturaBotonI;
    estadoBotonJuegoAnteriorD = lecturaBotonD;

    delay((int)velocidad);
  }

  delay(2000);
  tft.fillScreen(0x0000);
  opcionSeleccionada = false;
  esperandoConfirmacion = false;
  dibujaMenuPrincipal(estadoPotAnterior);
}

// Inicia una nueva ronda en el juego de reacción
void nuevaRonda() {
  tft.fillScreen(0x0000);

  // Configura dirección y posición inicial de la barra
  direccion = random(0, 2);
  barraX = direccion ? 10 : 310;
  puedePresionar = false;
  tiempoInicioRonda = millis();

  // Determina tamaño y posición de la zona objetivo
  int anchoZona = max(40, 200 - (nivel - 1) * 20);
  int centroX = random(60, 260);
  xObjetivoInicio = centroX - anchoZona / 2;
  xObjetivoFin = centroX + anchoZona / 2;

  // Color objetivo aleatorio (rojo o azul)
  colorObjetivo = random(0, 2) == 0 ? 0xF800 : 0x001F;

  // Altura y separación de corchetes visuales
  int corcheteAltura = 10;
  int separacionY = 25;

  // Dibuja corchetes en pantalla que delimitan zona objetivo
  tft.fillRect(xObjetivoInicio, yLinea - separacionY, anchoZona, corcheteAltura, colorObjetivo);
  tft.fillRect(xObjetivoInicio, yLinea + separacionY - corcheteAltura, anchoZona, corcheteAltura, colorObjetivo);

  // Muestra nivel actual
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Nivel: ");
  tft.print(nivel);

  // Dibuja corazones representando vidas restantes
  for (int i = 0; i < 3; i++) {
    tft.setTextSize(2);
    if (i < vidas) tft.setTextColor(0xF800);  // rojo
    else tft.setTextColor(0x7BEF);            // gris claro
    tft.setCursor(200 + i * 20, 10);
    tft.print((char)3);  // Símbolo corazón
  }
}

// Verifica si el jugador acertó la precisión en el juego de reacción
void verificarReto() {
  int centroBarraX = barraX;
  bool dentroZona = (centroBarraX >= xObjetivoInicio && centroBarraX <= xObjetivoFin);

  // Detecta si se presionó el botón correcto según el color objetivo
  bool botonRojo = (lecturaBotonI == LOW && estadoBotonJuegoAnteriorI == HIGH);
  bool botonAzul = (lecturaBotonD == LOW && estadoBotonJuegoAnteriorD == HIGH);
  bool botonCorrecto = (colorObjetivo == 0xF800 && botonRojo) || (colorObjetivo == 0x001F && botonAzul);

  if (dentroZona && botonCorrecto) {
    nivel++;
    velocidad = max(1.0, velocidad - 0.5);
    nuevaRonda();
  } else {
    vidas--;
    if (vidas <= 0) {
      jugando = false;

      // Limpia cualquier área residual
      tft.fillScreen(0x0000);

      // Mostrar imagen "final.bmp"
      imagen("final.bmp");

      // Texto de nivel
      String textoNivel = "Nivel: " + String(nivel);

      const int screenWidth = 320;
      tft.setTextSize(2);
      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(textoNivel, 0, 0, &x1, &y1, &w, &h);
      int16_t posX = (screenWidth - w) / 2;
      int16_t posY = 200;  // Puedes ajustar

      tft.setTextColor(0xFFFF);
      tft.setCursor(posX, posY);
      tft.print(textoNivel);

      delay(2500);

      // Limpia toda la pantalla antes de volver
      tft.fillScreen(0x0000);

      // Volver al menú principal
      opcionSeleccionada = false;
      esperandoConfirmacion = false;
      pantallaActiva = true;
      dibujaMenuPrincipal(estadoPotAnterior);

    } else {
      // Mensaje vida perdida
      tft.fillScreen(0x0000);
      const char* mensaje = "Vida perdida!";
      int anchoTexto = strlen(mensaje) * 12;
      int xTexto = (320 - anchoTexto) / 2;
      int yTexto = (240 - 16) / 2;

      for (int i = 0; i < 3; i++) {
        tft.setTextColor(0xF800);
        tft.setTextSize(2);
        tft.setCursor(xTexto, yTexto);
        tft.print(mensaje);
        delay(300);
        tft.setTextColor(0x0000);
        tft.setCursor(xTexto, yTexto);
        tft.print(mensaje);
        delay(300);
      }
      nuevaRonda();
    }
  }
}


// Muestra el menú para seleccionar modalidad del juego memoria
void submenuMemoria(int opcion) {
  tft.fillScreen(0x0000);
  tft.setTextSize(2);
  tft.setTextColor(0xFFFF);
  tft.setCursor(20, 30);
  tft.print("SELECCIONE MODALIDAD");

  // Opción 1: Reto Fácil
  if (opcion == 0) {
    tft.fillRect(0, 70, 320, 40, 0x001F);  // Fondo azul para opción activa
  }
  tft.setTextColor(0xFFFF);
  tft.setCursor(10, 80);
  tft.print("   1. Reto Facil");

  // Opción 2: Reto Extremo
  if (opcion == 1) {
    tft.fillRect(0, 110, 320, 40, 0x001F);  // Fondo azul para opción activa
  }
  tft.setTextColor(0xFFFF);
  tft.setCursor(10, 120);
  tft.print("   2. Reto Extremo");
}

// Función principal para seleccionar modalidad y lanzar el juego memoria
void juegoMemoria() {
  seleccionandoModoMemoria = true;
  opcionModoMemoriaSeleccionada = false;
  estadoPotModoMemoriaAnterior = -1;  // Forzar refresco del menú

  while (seleccionandoModoMemoria) {
    lecturaPot = analogRead(pinPot);
    porcentaje = map(lecturaPot, 0, 1023, 0, 100);
    int opcionActual = (porcentaje < 50) ? 0 : 1;

    if (opcionActual != estadoPotModoMemoriaAnterior && !opcionModoMemoriaSeleccionada) {
      estadoPotModoMemoriaAnterior = opcionActual;
      submenuMemoria(opcionActual);
    }

    lecturaBotonI = digitalRead(pinBotonI);
    lecturaBotonD = digitalRead(pinBotonD);

    // Detectar botón rojo y azul presionados (flanco descendente)
    bool botonRojoPresionado = (lecturaBotonI == LOW && estadoAnteriorBotonI == HIGH);
    bool botonAzulPresionado = (lecturaBotonD == LOW && estadoAnteriorBotonD == HIGH);

    if (!opcionModoMemoriaSeleccionada && botonAzulPresionado) {
      opcionModoMemoriaSeleccionada = true;
      seleccionandoModoMemoria = false;

      pantallaActiva = true;
      opcionSeleccionada = false;
      esperandoConfirmacion = false;

      delay(300);

      if (opcionActual == 0) {
        juegoMemoriaFacil();
      } else {
        juegoMemoriaExtremo();
      }
    }

    if (!opcionModoMemoriaSeleccionada && botonRojoPresionado) {
      seleccionandoModoMemoria = false;
      opcionSeleccionada = false;
      pantallaActiva = true;
      tft.fillScreen(0x0000);
      dibujaMenuPrincipal(estadoPotAnterior);
      return;
    }

    estadoAnteriorBotonI = lecturaBotonI;
    estadoAnteriorBotonD = lecturaBotonD;

    delay(50);
  }
}

// Juego memoria modo fácil
void juegoMemoriaFacil() {
  nivel = 1;
  vidas = 3;
  jugando = true;

  int secuencia[50];
  int longitudSecuencia = 0;

  while (jugando) {
    // Generar y agregar nuevo color a la secuencia
    secuencia[longitudSecuencia++] = random(0, 2);  // 0=rojo, 1=azul

    // Mostrar nivel y vidas
    tft.fillScreen(0x0000);
    tft.setTextSize(2);
    tft.setTextColor(0xFFFF);
    tft.setCursor(10, 10);
    tft.print("Nivel: ");
    tft.print(nivel);

    for (int i = 0; i < 3; i++) {
      tft.setTextColor(i < vidas ? 0xF800 : 0x7BEF);
      tft.setCursor(200 + i * 20, 10);
      tft.print((char)3);
    }
    delay(1000);

    // Mostrar secuencia
    for (int i = 0; i < longitudSecuencia; i++) {
      mostrarCuadro(secuencia[i]);
      delay(nivel >= 7 ? 800 : (nivel >= 4 ? 1000 : 1200));
      tft.fillRect(110, 70, 100, 100, 0x0000);
      delay(500);
    }

    // Mostrar "TU TURNO"
    tft.fillRect(60, 40, 200, 40, 0xFFFF);
    tft.setTextColor(0x0000);
    tft.setTextSize(2);
    tft.setCursor(100, 55);
    tft.print("TU TURNO!");
    delay(1000);
    tft.fillScreen(0x0000);

    // Copiar secuencia para comparación
    int copiaSecuencia[50];
    memcpy(copiaSecuencia, secuencia, longitudSecuencia * sizeof(int));

    // Resetear estados botones para evitar detección errónea
    estadoBotonJuegoAnteriorI = digitalRead(pinBotonI);
    estadoBotonJuegoAnteriorD = digitalRead(pinBotonD);

    bool perderTurno = false;

    // Turno del jugador
    for (int i = 0; i < longitudSecuencia; i++) {
      bool entradaRecibida = false;
      unsigned long tiempoInicio = millis();
      int ultimoSegundoMostrado = 4;

      while (!entradaRecibida && (millis() - tiempoInicio < 3000)) {
        lecturaBotonI = digitalRead(pinBotonI);
        lecturaBotonD = digitalRead(pinBotonD);

        bool botonRojo = lecturaBotonI == LOW && estadoBotonJuegoAnteriorI == HIGH;
        bool botonAzul = lecturaBotonD == LOW && estadoBotonJuegoAnteriorD == HIGH;

        if (botonRojo || botonAzul) {
          entradaRecibida = true;
          int eleccion = botonRojo ? 0 : 1;

          mostrarCuadro(eleccion);
          delay(300);
          tft.fillRect(110, 70, 100, 100, 0x0000);

          if (eleccion != copiaSecuencia[i]) {
            vidas--;
            if (vidas <= 0) {
              jugando = false;
              break;  // salir del while (jugando)
            }
            mostrarMensaje("Incorrecto!");
            perderTurno = true;
            break;
          }
        }

        int segundosRestantes = 3 - (millis() - tiempoInicio) / 1000;
        if (segundosRestantes != ultimoSegundoMostrado) {
          ultimoSegundoMostrado = segundosRestantes;

          tft.fillRect(140, 180, 40, 40, 0x0000);
          tft.setTextColor(0xFFFF);
          tft.setTextSize(2);
          tft.setCursor(150, 190);
          tft.print(segundosRestantes);
        }

        estadoBotonJuegoAnteriorI = lecturaBotonI;
        estadoBotonJuegoAnteriorD = lecturaBotonD;
        delay(30);
      }

      if (perderTurno) break;

      if (!entradaRecibida) {
        vidas--;
        if (vidas <= 0) {
          jugando = false;
          break;
        }
        mostrarMensaje("Muy lento!");
        perderTurno = true;
        break;
      }
    }

    if (jugando && !perderTurno) nivel++;

    delay(1000);
  }

  // Antes de mostrar la imagen, limpia el área del contador para que no quede pegado
  tft.fillRect(140, 180, 40, 40, 0x0000);  // Limpia el contador

  // Mostrar fin de juego con imagen
  imagen("final.bmp");  // Muestra la imagen "Game Over"

  // Texto a mostrar con el nivel
  String textoNivel = "Nivel: " + String(nivel - 1);

  // Tamaño de texto y ancho pantalla (ajusta screenWidth si tu pantalla es distinta)
  const int screenWidth = 320;
  tft.setTextSize(2);

  // Calcula el ancho y alto del texto para centrarlo
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(textoNivel, 0, 0, &x1, &y1, &w, &h);

  // Calcula posición X centrada
  int16_t posX = (screenWidth - w) / 2;
  int16_t posY = 200;  // Posición vertical

  // Muestra el texto centrado
  tft.setTextColor(0xFFFF);  // Blanco
  tft.setCursor(posX, posY);
  tft.print(textoNivel);

  delay(2500);

  // Limpia toda la pantalla antes de volver al submenu para evitar residuos
  tft.fillScreen(0x0000);

  // Reiniciar variables y mostrar menú
  pantallaActiva = true;
  opcionSeleccionada = false;
  esperandoConfirmacion = false;
  seleccionandoModoMemoria = true;
  opcionModoMemoriaSeleccionada = false;
  estadoPotModoMemoriaAnterior = -1;
  estadoAnteriorBotonI = digitalRead(pinBotonI);
  estadoAnteriorBotonD = digitalRead(pinBotonD);

  submenuMemoria(estadoPotModoMemoriaAnterior);
}

// Juego memoria modo extremo (colores invertidos)
void juegoMemoriaExtremo() {
  nivel = 1;
  vidas = 3;
  jugando = true;

  mostrarAvisoInversion();

  int secuencia[50];
  int longitudSecuencia = 0;

  while (jugando) {
    secuencia[longitudSecuencia++] = random(0, 2);  // 0=rojo, 1=azul

    // Mostrar nivel y vidas
    tft.fillScreen(0x0000);
    tft.setTextSize(2);
    tft.setTextColor(0xFFFF);
    tft.setCursor(10, 10);
    tft.print("Nivel: ");
    tft.print(nivel);

    for (int i = 0; i < 3; i++) {
      tft.setTextColor(i < vidas ? 0xF800 : 0x7BEF);
      tft.setCursor(200 + i * 20, 10);
      tft.print((char)3);
    }
    delay(1000);

    estadoBotonJuegoAnteriorI = digitalRead(pinBotonI);
    estadoBotonJuegoAnteriorD = digitalRead(pinBotonD);

    // Mostrar secuencia normal
    for (int i = 0; i < longitudSecuencia; i++) {
      mostrarCuadro(secuencia[i]);
      delay(nivel >= 7 ? 800 : (nivel >= 4 ? 1000 : 1200));
      tft.fillRect(110, 70, 100, 100, 0x0000);
      delay(500);
    }

    // Borrar cuadros residuales
    tft.fillRect(110, 70, 100, 100, 0x0000);

    // Mostrar "TU TURNO!"
    tft.fillRect(60, 40, 200, 40, 0xFFFF);
    tft.setTextColor(0x0000);
    tft.setTextSize(2);
    tft.setCursor(100, 55);
    tft.print("TU TURNO!");
    delay(1500);
    tft.fillRect(60, 40, 200, 40, 0x0000);

    // Resetear estados botones para evitar falso trigger
    estadoBotonJuegoAnteriorI = digitalRead(pinBotonI);
    estadoBotonJuegoAnteriorD = digitalRead(pinBotonD);

    // Preparar secuencia invertida para comparación
    int copiaInvertida[50];
    for (int i = 0; i < longitudSecuencia; i++) {
      copiaInvertida[i] = 1 - secuencia[i];  // Invierte color
    }

    bool perderTurno = false;

    for (int i = 0; i < longitudSecuencia; i++) {
      bool entradaRecibida = false;
      unsigned long tiempoInicio = millis();
      int ultimoSegundoMostrado = 4;

      while (!entradaRecibida && (millis() - tiempoInicio < 3000) && jugando) {
        lecturaBotonI = digitalRead(pinBotonI);
        lecturaBotonD = digitalRead(pinBotonD);

        bool botonRojo = lecturaBotonI == LOW && estadoBotonJuegoAnteriorI == HIGH;
        bool botonAzul = lecturaBotonD == LOW && estadoBotonJuegoAnteriorD == HIGH;

        if (botonRojo || botonAzul) {
          entradaRecibida = true;
          int eleccion = botonRojo ? 0 : 1;

          mostrarCuadro(eleccion);
          delay(300);
          tft.fillRect(110, 70, 100, 100, 0x0000);

          if (eleccion != copiaInvertida[i]) {
            vidas--;
            if (vidas <= 0) {
              jugando = false;
              break;
            }
            mostrarMensaje("Incorrecto!");
            perderTurno = true;
            break;
          }
        }

        int segundosRestantes = 3 - (millis() - tiempoInicio) / 1000;
        if (segundosRestantes != ultimoSegundoMostrado) {
          ultimoSegundoMostrado = segundosRestantes;

          tft.fillRect(140, 180, 40, 40, 0x0000);
          tft.setTextColor(0xFFFF);
          tft.setTextSize(2);
          tft.setCursor(150, 190);
          tft.print(segundosRestantes);
        }

        estadoBotonJuegoAnteriorI = lecturaBotonI;
        estadoBotonJuegoAnteriorD = lecturaBotonD;
        delay(30);
      }

      if (perderTurno) break;

      if (!entradaRecibida) {
        vidas--;
        if (vidas <= 0) {
          jugando = false;
          break;
        }
        mostrarMensaje("Muy lento!");
        perderTurno = true;
        break;
      }
    }

    if (jugando && !perderTurno) nivel++;

    delay(1000);
  }

  // Antes de mostrar la imagen, limpia contador por si queda visible
  tft.fillRect(140, 180, 40, 40, 0x0000);

  // Mostrar fin de juego con imagen
  imagen("final.bmp");  // Muestra la imagen "Game Over"

  // Texto a mostrar con el nivel
  String textoNivel = "Nivel: " + String(nivel - 1);

  // Tamaño de texto y ancho pantalla
  const int screenWidth = 320;
  tft.setTextSize(2);

  // Calcular ancho y alto para centrar texto
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(textoNivel, 0, 0, &x1, &y1, &w, &h);

  int16_t posX = (screenWidth - w) / 2;
  int16_t posY = 200;  // Ajusta si quieres

  // Mostrar texto centrado
  tft.setTextColor(0xFFFF);  // Blanco
  tft.setCursor(posX, posY);
  tft.print(textoNivel);

  delay(2500);

  // Limpiar pantalla antes de menú para evitar residuos
  tft.fillScreen(0x0000);

  // Volver al menú
  seleccionandoModoMemoria = true;
  opcionModoMemoriaSeleccionada = false;
  estadoPotModoMemoriaAnterior = 0;
  estadoAnteriorBotonD = digitalRead(pinBotonD);
  submenuMemoria(estadoPotModoMemoriaAnterior);
}

// Muestra aviso de inversión de colores antes de modo extremo
void mostrarAvisoInversion() {
  tft.fillScreen(0x0000);

  const char* mensaje = "Los colores se invierten";
  tft.setTextSize(2);

  int anchoTexto = strlen(mensaje) * 12;
  int altoTexto = 16;

  int xTexto = (320 - anchoTexto) / 2;
  int yTexto = (240 - altoTexto) / 2;

  tft.fillRect(xTexto - 10, yTexto - 10, anchoTexto + 20, altoTexto + 20, 0xFFFF);
  tft.setTextColor(0x0000);
  tft.setCursor(xTexto, yTexto);
  tft.print(mensaje);

  delay(2000);
  tft.fillScreen(0x0000);
}

// Muestra un cuadro de color (rojo o azul) en pantalla
void mostrarCuadro(int colorID) {
  uint16_t color = (colorID == 0) ? 0xF800 : 0x001F;
  tft.fillRect(110, 70, 100, 100, color);
}

// Muestra un mensaje parpadeando en pantalla
void mostrarMensaje(const char* texto) {
  tft.fillScreen(0x0000);

  int anchoTexto = strlen(texto) * 12;
  int xTexto = (320 - anchoTexto) / 2;
  int yTexto = (240 - 16) / 2;

  for (int i = 0; i < 3; i++) {
    tft.setTextColor(0xF800);
    tft.setTextSize(2);
    tft.setCursor(xTexto, yTexto);
    tft.print(texto);
    delay(300);

    tft.setTextColor(0x0000);
    tft.setCursor(xTexto, yTexto);
    tft.print(texto);
    delay(300);
  }
}

// Leer 16 bits desde archivo
uint16_t read16(File& f) {
  uint16_t result;
  ((uint8_t*)&result)[0] = f.read();  // LSB
  ((uint8_t*)&result)[1] = f.read();  // MSB
  return result;
}

// Leer 32 bits desde archivo
uint32_t read32(File& f) {
  uint32_t result;
  ((uint8_t*)&result)[0] = f.read();
  ((uint8_t*)&result)[1] = f.read();
  ((uint8_t*)&result)[2] = f.read();
  ((uint8_t*)&result)[3] = f.read();
  return result;
}

void drawBMP(const char* filename, int x, int y) {
  bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.println("No se pudo abrir el archivo BMP");
    return;
  }

  if (read16(bmpFile) != 0x4D42) {
    Serial.println("No es un archivo BMP válido");
    bmpFile.close();
    return;
  }

  (void)read32(bmpFile);  // Tamaño
  (void)read32(bmpFile);  // Reservado
  uint32_t offset = read32(bmpFile);
  (void)read32(bmpFile);  // Tamaño DIB
  int bmpWidth = read32(bmpFile);
  int bmpHeight = read32(bmpFile);
  if (bmpHeight < 0) bmpHeight = -bmpHeight;

  if (read16(bmpFile) != 1 || read16(bmpFile) != 24 || read32(bmpFile) != 0) {
    Serial.println("Solo se admiten BMPs de 24 bits sin compresión");
    bmpFile.close();
    return;
  }

  bmpFile.seek(offset);
  uint8_t sdbuffer[3 * BUFFPIXEL];
  uint16_t color;
  int rowSize = (bmpWidth * 3 + 3) & ~3;

  for (int row = 0; row < bmpHeight; row++) {
    int pos = offset + (bmpHeight - 1 - row) * rowSize;
    bmpFile.seek(pos);

    for (int col = 0; col < bmpWidth; col += BUFFPIXEL) {
      int readWidth = min(BUFFPIXEL, bmpWidth - col);
      bmpFile.read(sdbuffer, readWidth * 3);

      for (int i = 0; i < readWidth; i++) {
        uint8_t b = sdbuffer[i * 3];
        uint8_t g = sdbuffer[i * 3 + 1];
        uint8_t r = sdbuffer[i * 3 + 2];
        color = tft.color565(r, g, b);

        if ((x + col + i < tft.width()) && (y + row < tft.height())) {
          tft.drawPixel(x + col + i, y + row, color);
        }
      }
    }
  }

  bmpFile.close();
  Serial.println("Imagen BMP mostrada");
}

void imagen(const char* filename) {
  drawBMP(filename, (tft.width() - 320) / 2, (tft.height() - 240) / 2);
}
