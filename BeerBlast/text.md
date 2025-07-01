**Integrantes**

  - Josefina Marin


**Descripción**

OBJETIVO DEL JUEGO:

Conformado de dos integrantes o de equipos de dos, compiten para ser los primeros
en anotar / quitar todos los vasos del otro contrincante, ganando el total de puntos y
ganando el juego; pudiendo visualizar el puntaje en la nube, en conjunto con el
ranking de personas que han jugado en el juego. Todos los espectadores pueden ver
en tiempo real el estado del juego y el ranking desde su celular o aparato electrónico.
Dependiendo de la modalidad, posee diferentes módulos de juego (que se escogen
por keypad):
  
  - Simple
  - Con tiempo
  - Aleatorio ( la luz led te avisa a cuál debes acertar).

El objetivo general del juego es acertar en los vasos del oponente las pelotas de pingpong; logrando así, apagar las luces y sumar punto automáticamente, hasta que el
oponente quede sin vasos iluminados, minuto cuándo el juego termina.

MECÁNICA DEL JUEGO:

Se colocan 6 vasos por jugador / equipo con un sensor y LED por cada vaso.
La pantalla LCD indica cuándo comienza el juego.
Se determina la modalidad del juego con un keypad, y si es la modalidad (2) aparecen
el tiempo restante en la pantalla LCD.
Cada jugador lanza por turno, hacia los vasos del otro jugador.

Si el jugador en su turno acierta, se remueve el vaso, y:
  - El contrincante al que le fue anotado toma el líquido que se encuentra en el vaso.
  - El sensor detecta que fue removido.
  - Se apaga la luz del vaso.
  - Se suma un punto al jugador que se encuentra jugando y esto aparece en la pantalla física LCD.
  - El Arduino actualiza la página web con los nuevos datos.
  - 
La página web actualiza los puntajes por turno.
Cuándo uno de los jugadores logra eliminar todos los vasos, se apagan todos los
LEDs y se actualizan los rankings en la página web, y en la pantalla LCD aparece
“GAME OVER”.

**INTERFÁZ FÍSICA-DIGITAL:**

FÍSICA:
Tablero tipo ping – pong con los círculos donde debe estar cada vaso, colocados en
orden de triángulo.
Vasos con sensores ( sensor infrarrojo).
LEDs se encuentran abajo de cada vaso.
Tiene un keypad con modalidades de juego para escoger al principio de cada juego.
DIGITAL:
Se visualizan los puntajes de los jugadores / integrantes con un display LCD.
Luz encendida en cada vaso y se apaga cuándo se remueven.
Dependiendo de las modalidad del juego, pueden destellar o no los LEDs para
indicar a cuál se debe acertar.
(DESDE EL CELULAR)

Página web que muestra:
  - El puntaje en tiempo real.
  - Quiénes son los jugadores actuales del juego (nombres).
  - Ranking global de los ganadores del juego.
  - La página se abre escaneando un QR que se encuentra impreso en el tablero.

COMPONENTES:

  - Arduino R4 WIFI.
  - Sensores infrarrojo por vaso (12 minimo).
  - LEDs por vaso (12minimo).
  - Pantalla LCD (puntaje por jugador).
  - Keypad (poner la modalidad de juego).
  - Protoboards.
  - Resistencias.
  - Cables.

