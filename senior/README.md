# Senior

## Descripción

Consola de juegos en pantalla TFT con un potenciómetro para navegar y dos botones:

- **Reacción**: modo práctica y modo reto (con vidas).
- **Memoria**: modo fácil y modo extremo.

## Archivos

- `senior/senior.ino`: programa completo (menús, juegos y lectura de imágenes BMP).

## Puesta en marcha

1. Librerías: **MCUFRIEND_kbv** y **SD**.
2. Copia `portada.bmp` y `final.bmp` (BMP de 24 bits sin compresión) en la tarjeta SD de la pantalla. No están en el repositorio.
3. Los botones usan los pines 0 y 1. En un Arduino UNO R3 esos pines son los del puerto serial, así que conviene no usar el Monitor Serie o mover los botones a otros pines; en el UNO R4 no hay conflicto.

← [Volver al índice](../README.md)
