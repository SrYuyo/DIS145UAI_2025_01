# Spin to Win

## Integrantes

  - Bernardita Alessandri
  - Clemente Baez

## Descripción

Objetivo del Juego: El objetivo es medir el tiempo de reacción del jugador al alcanzar el LED que se ilumina aleatoriamente. El jugador debe mover un manubrio conectado a un potenciómetro multigiro y posicionarlo correctamente sobre el LED encendido. Un led indica la posición, y este se va moviendo de acuerdo con el giro del potenciómetro, hasta alcanzar el Led Aleatoriamente encendido. Se otorgan puntos según la rapidez de reacción.

## Mecánica del Juego

  - Al comenzar, el sistema enciende un LED de forma aleatoria entre los 4 grupos disponibles.
  - El jugador gira el manubrio (potenciómetro) para alinear el led de posición, con el LED iluminado.
  - Cuando el led de posicionamiento coincide con el LED aleatorio objetivo, se registra el tiempo de reacción.
  - El LED siguiente se enciende de forma aleatoria.
  - Esto se repite 30 veces. El objetivo es completar todas las rondas en el menor tiempo posible y batir el récord anterior.
    
## Interfaz

-  Física: El jugador interactúa con el manubrio físico que mueve un led (se va encendiendo secuencialmente) sobre una fila (o semicírculo) de leds. Una pantalla LCD o el monitor serial muestra tiempos, puntaje y récords.
-  Visual: Los LEDs indican la posición objetivo.

## Archivos

- `spin_to_win/spin_to_win.ino`: tira de 45 NeoPixels, 4 LEDs objetivo, potenciómetro multigiro, botón de reinicio y LCD I2C con el tiempo y el récord.

> El código actual juega **10 rondas** por intento (variable `prendidas`); la descripción menciona 30.

## Puesta en marcha

Librerías: **Adafruit NeoPixel** y **LiquidCrystal_I2C**.

← [Volver al índice](../README.md)
