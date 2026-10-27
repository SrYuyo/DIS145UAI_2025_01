# Arcade Office

## Integrantes

  - Pablo Sandoval
  - Matías Valenzuela

## Descripción

Consola arcade con menú tipo carrusel en una pantalla TFT. Desde el menú se puede jugar **Snake** o **Tetris**, ver una imagen secreta o apagar la consola. Se controla con un joystick conectado a una segunda placa, que envía los comandos por WiFi.

## Archivos

| Carpeta | Placa | Qué hace |
| :--- | :--- | :--- |
| `arcade/` | Arduino UNO R4 WiFi + shield TFT ILI9486 | Pantalla, menú y juegos. Levanta un servidor en el puerto 8080 |
| `control_joystick/` | Arduino UNO R4 WiFi + joystick | Lee el joystick y envía `ARRIBA`, `ABAJO`, `IZQUIERDA`, `DERECHA` o `SELECT` a la pantalla |

`arcade/` incluye `snake_game.h`, `tetris_game.h`, la fuente `VCR_OSD_MONO_1_00115pt7b.h` y la imagen `Secreto.h`.

## Puesta en marcha

1. Librerías: **DIYables_TFT_Shield** (pantalla) y **Joystick** (control).
2. Escribe tu red en `ssid` y `password` en **ambos** sketches.
3. Carga `arcade/` y anota la IP que aparece en el Monitor Serie.
4. En `control_joystick.ino` cambia `serverIP` por esa IP y cárgalo en la segunda placa.

← [Volver al índice](../README.md)
