# TRON

## Descripción

Duelo de motos de luz para dos jugadores. Cada jugador maneja su moto con cuatro botones físicos; el juego se ve en Processing, las vidas se muestran en una pantalla LCD y los ganadores se guardan en una tarjeta SD.

## Archivos

| Carpeta | Qué hace |
| :--- | :--- |
| `tron_arduino/` | Lee los botones de ambos jugadores y los envía por serial (`J1U`, `J2L`, …). Recibe los golpes y ganadores para la LCD y la SD |
| `tron_processing/` | Juego visual en Processing |

## Puesta en marcha

1. Arduino: librerías **SD** y **LiquidCrystal_I2C**. Si la SD falla, el juego funciona igual pero sin guardar ganadores.
2. Processing: librerías **Serial** y **Sound**. La tipografía `Tr2n.ttf` debe estar en la carpeta `data/` del sketch.
3. El puerto serial está fijo como `"COM4"` en `tron_processing.pde`; cámbialo por el de tu Arduino (en Mac/Linux será algo como `/dev/cu.usbmodem…`).

← [Volver al índice](../README.md)
