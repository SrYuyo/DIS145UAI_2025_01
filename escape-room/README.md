# Escape Room

## Descripción

Escape room por fases que se juega en una caja con pantalla OLED, teclado matricial, potenciómetro, botones de "cables" y buzzer. Las partidas ganadas y perdidas se registran en Arduino Cloud.

1. **Memorice**: memorizar y escribir el código que muestra la pantalla.
2. **Potenciómetro**: resolver la ecuación y fijar el valor de *x*; confirmar con `*`.
3. **Anillo**: abrir el cryptex y escribir la clave numérica (una sola oportunidad).
4. **Bomba**: elegir nivel (A: 3 vidas, B: 1 vida + pista) y "cortar" los cables en el orden correcto.

`#` reinicia el juego en cualquier momento.

## Archivos

- `escape_room/escape_room.ino`: programa principal.
- `escape_room/thingProperties.h`: variables de Arduino Cloud (`vecesGanadas`, `vecesPerdidas`).
- `escape_room/arduino_secrets.example.h`: plantilla de credenciales WiFi.

## Puesta en marcha

1. Librerías: **ArduinoIoTCloud**, **Arduino_ConnectionHandler**, **DIYables_Keypad**, **Adafruit SSD1306**.
2. Copia `arduino_secrets.example.h` como `arduino_secrets.h` y escribe tu red WiFi (ese archivo no se sube a GitHub).
3. Placa: Arduino UNO R4 WiFi.

← [Volver al índice](../README.md)
