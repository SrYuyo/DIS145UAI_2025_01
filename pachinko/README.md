# Pachinko

## Descripción

Pachinko para dos jugadores (verde y rojo). Ambos se declaran listos con su botón y lanzan la bola; un sensor de luz (LDR) en cada meta detecta dónde cae y el marcador de victorias queda guardado en Arduino Cloud.

## Archivos

| Carpeta | Qué hace |
| :--- | :--- |
| `sensores_ldr/` | Calibra la luz ambiente de los dos LDR y publica en la nube cuál fue tapado |
| `marcador/` | Botones de "listo", pantalla LCD y contador de victorias de cada jugador |

## Puesta en marcha

- Ambas placas son Arduino UNO R4 WiFi conectadas al mismo *Thing* de Arduino Cloud.
- Falta en el repositorio el `thingProperties.h` de cada sketch: se genera desde Arduino Cloud con las variables `lDR1Tapado`, `lDR2Tapado` (sensores) y `estadoLDR1`, `estadoLDR2`, `verdeVictorias`, `rojoVictorias` (marcador).
- Librería adicional para el marcador: **LiquidCrystal_I2C**.

← [Volver al índice](../README.md)
