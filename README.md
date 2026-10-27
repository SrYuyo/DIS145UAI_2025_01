# DIS145 UAI · 2025-1 · Sección 2

Proyectos finales del curso **Diseño y Construcción de Interfaces (DIS145)** de la Universidad Adolfo Ibáñez, primer semestre 2025, sección 2. Cada proyecto es un juego con una interfaz física-digital construida con Arduino (principalmente **Arduino UNO R4 WiFi**) y, en algunos casos, Processing o Arduino Cloud.

## Proyectos

| Proyecto | De qué se trata | Código |
| :--- | :--- | :--- |
| [Arcade Office](arcade-office/) | Consola arcade con Snake y Tetris en pantalla TFT, controlada por un joystick WiFi | Arduino ×2 |
| [BeerBlast](beer-blast/) | Beer pong con sensores en los vasos, LCD y ranking web | Solo descripción |
| [Cuarto Rey 2.0](cuarto-rey-2.0/) | Ruleta de 13 LEDs que reemplaza la baraja del juego "Cuarto Rey" | Arduino |
| [Escape Room](escape-room/) | Escape room por fases: código, potenciómetro, cryptex y bomba | Arduino + Arduino Cloud |
| [Maldito Número](maldito-numero/) | — | Sin archivos aún |
| [Pachinko](pachinko/) | Pachinko de dos jugadores con sensores de luz y marcador en la nube | Arduino ×2 + Arduino Cloud |
| [Pulso Frágil](pulso-fragil/) | Reto de la galleta Dalgona con joystick: recortar la figura sin salirse | Arduino + Processing |
| [Senior](senior/) | Juegos de reacción y memoria en pantalla TFT | Arduino |
| [Spin to Win](spin-to-win/) | Juego de reacción: alinear un LED con un manubrio (potenciómetro) | Arduino |
| [TRON](tron/) | Duelo de motos de luz para 2 jugadores con botones físicos | Arduino + Processing |
| [werewOLF](werewolf/) | Moderador electrónico de fases día/noche para "Hombres Lobo" | Arduino |

## Estructura

Cada proyecto tiene su carpeta con un `README.md` y una subcarpeta por programa. El IDE de Arduino y Processing exigen que **el archivo principal tenga el mismo nombre que su carpeta**, por eso la estructura es así:

```
nombre-proyecto/
├── README.md                    ← integrantes, descripción y cómo usarlo
└── nombre_sketch/
    ├── nombre_sketch.ino        ← se abre directamente con el IDE de Arduino
    └── *.h                      ← archivos auxiliares del mismo sketch
```

## Cómo abrir un proyecto

1. Abre el `.ino` (o `.pde`) con el IDE de Arduino (o Processing).
2. Instala las librerías indicadas en el README del proyecto (*Herramientas → Administrar bibliotecas*).
3. Si el proyecto usa WiFi, reemplaza `TU_RED_WIFI` y `TU_CLAVE_WIFI` por los datos de tu red. **No subas tu clave real a GitHub.**
4. Los proyectos con Arduino Cloud (Escape Room, Pachinko, Pulso Frágil) necesitan el archivo `thingProperties.h` que genera el *Thing* en [Arduino Cloud](https://app.arduino.cc/things); para Escape Room ya está incluido.
