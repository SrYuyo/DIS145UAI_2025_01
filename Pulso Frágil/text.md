**Integrantes**

  - Giovanna Ruiz
  - Suiyu Ma

**Descripción**

Descripción:
● Objeto del juego: Este juego, inspirado en el reto de la galleta Dalgona de la serie El
juego del calamar, busca ofrecer una experiencia interactiva que permita a personas
de todas las edades medir y mejorar su motricidad fina. A través del uso de sensores
físicos conectados a Arduino y una interfaz visual en Processing, el jugador debe
recortar con precisión una figura virtual sin desviarse ni aplicar demasiada presión,
evitando así "romper la galleta". El sistema responde con estímulos visuales,
sonoros y temporales que aumentan la dificultad, fomentando la concentración, el
control del pulso y la coordinación mano-ojo en un entorno lúdico y desafiante.

● Mecánica del juego: El juego comienza cuando el jugador presiona el botón de inicio,
lo que activa el sistema y muestra en la pantalla (Processing) una figura Dalgona
aleatoria, como un círculo, estrella o paraguas. Al mismo tiempo, se inicia un
cronómetro regresivo y se reinician los sensores. Usando el joystick, el jugador debe
desplazar un puntero por el contorno de la figura mostrada en pantalla, simulando el
proceso de recortar la galleta. El puntero se mueve en tiempo real según las señales
enviadas por el joystick a través de Arduino, y el sistema evalúa si el jugador se
mantiene dentro de los márgenes establecidos de la figura. Si el jugador se sale del
borde se considera un fallo. Si logra recorrer todo el contorno correctamente, sin
errores y dentro del tiempo límite, el jugador gana.

● Interfaz físico-digital: Pantalla de Processing donde se muestre la galleta y la forma
que tiene, junto con su cronómetro. En el plano físico, el jugador interactúa con un
joystick que permite controlar con precisión el movimiento de un puntero en la
pantalla, representando la acción de cortar la galleta. Además, cuenta con botones:
uno de inicio y otro de finalización. El botón de inicio activa el juego y genera una
figura aleatoria en la interfaz digital, mientras que el botón de finalización permite
terminar la partida manualmente y guardar los segundos transcurridos desde que
inició el juego para luego comparar la rapidez de los jugadores. Todos estos
componentes están conectados a una placa Arduino, que se encarga de captar las
señales del jugador (movimientos y acciones) y enviarlas a Processing mediante
comunicación serial. En el plano digital, Processing interpreta esos datos y genera
una representación visual del juego: una figura Dalgona sobre un fondo limpio, un
puntero que se desplaza en tiempo real y elementos gráficos adicionales como un
cronómetro y alertas visuales. La interfaz gráfica responde dinámicamente a los
movimientos del joystick, indicando con colores o mensajes si el jugador mantiene la
precisión o comete errores.

● Componentes: Placa Arduino R4 wifi, Protoboard, Shield Joystick con botones,
Pantalla LCD 16x2, memoria micro SD.
