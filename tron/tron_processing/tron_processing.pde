
//tipografia de tron
PFont fuenteTron;


//Bibliotecas
import processing.sound.*;

import processing.serial.*; //  
Serial myPort; //  
String comandoArduino = ""; // 

//Sonidos
SoundFile SonidoMenu;
SoundFile SonidoJuego;
SoundFile SonidoFin;


int aTodoGasJ1 = 1;
int aTodoGasJ2 = 1;
//Variables globales
int borde = 20;
int tamañoJugador = 10;

//Posicion inicial
int posicionX1;
int posicionY1;
int posicionX2;
int posicionY2;

//Movimiento que tendra en X e Y 
int movX1 = 0;
int movX2 = 0;
int movY1 = 0;
int movY2 = 0;

//direccion que tendran al mover los jugadores
int direccionX1 = 0;
int direccionY1 = 0;
int direccionX2 = 0;
int direccionY2 = 0;

//colores jugadores
color cian = color(0,255,255);
color naranja = color(255,140,0);

//variables bandera del juego
boolean guardaGanador = false;
boolean bloquearMovimientoJ1 = true;
boolean juegoActivo = false;
boolean ganaRondaJugador1 = false;
boolean ganaRondaJugador2 = false;
boolean rondaFinalizada = false;
boolean pintarFondo = false;
boolean partidaFinalizada = false;
boolean aTodoGasUsadoJ1 = false;
int aTodoGasInicioJ1 = 0 ; 
int aTodoGasDuracionJ1 = 2000;
boolean aTodoGasUsadoJ2 = false;
int aTodoGasInicioJ2 = 0 ; 
int aTodoGasDuracionJ2 = 2000;

//vidas
int vidaJ1 = 3;
int vidaJ2 = 3;

//rondas de juego
int rondaActual = 1;
int rondaMaxima = 3;


void setup() {
  
  //asigno la tipografia para usarla
  fuenteTron = createFont("Tr2n.ttf", 64);
textFont(fuenteTron);
  
  //Pantalla completa en el monitor
  fullScreen(2);
  

  
  //posiciones iniciales de los jugadores
  posicionX1 = width / 2 - 100;
  posicionY1 = height / 2 ;
  posicionX2 = width / 2 + 100;
  posicionY2 = height / 2 ;
    
  myPort = new Serial(this, "COM4", 9600); //  
  myPort.bufferUntil('\n'); //  
  
  delay(500); // Espera a que Arduino esté listo // 
  myPort.write("LCD_INICIO\n"); // Muestra "PREPARENSE" al inicio ///////////////////////////////////////////////////////
}



void draw() {
  
    if (aTodoGasJ2 > 1 && millis() - aTodoGasInicioJ2 >= aTodoGasDuracionJ2) {
    aTodoGasJ2 = 1;
    // (opcional) ya podrías resetear aTodoGasUsado = false si quieres permitir otro turbo
  }
  
  if (aTodoGasJ1 > 1 && millis() - aTodoGasInicioJ1 >= aTodoGasDuracionJ1) {
    aTodoGasJ1 = 1;
    // (opcional) ya podrías resetear aTodoGasUsado = false si quieres permitir otro turbo
  }
  
//ve que el juego aun no comienza y dibuja la pantalla de inicio hasta que se presione "ENTER"
  if(juegoActivo == false){
  inicio();
  return;
  
   }
   
   //Pinta el fondo del juego
  if(pintarFondo == true){
    FondoJuego();
    pintarFondo= false;
    
    }
      //Revisa que la ronda actual sea menor que la maxima
  //y tambien revisa que ningun jugador alla ganado la ronda, para dibujarlos
  if (rondaActual <= rondaMaxima) {
    if (!ganaRondaJugador1 && !ganaRondaJugador2) {
      jugador1();
      jugador2();
    }

    // Registra el resultado mientras solo una vez por ronda
    if (!rondaFinalizada) {
      //si empatan se considera ronda nula
      if (ganaRondaJugador1 && ganaRondaJugador2) {
        rondaFinalizada = true;
        
        //si solo gana uno si cuenta como una ronda jugada
      } else if (ganaRondaJugador1) {
        rondaActual++;
        vidaJ2--;
        rondaFinalizada = true;
        
        myPort.write("J2HIT\n"); 
        
        
      }else if (ganaRondaJugador2) {
        rondaActual++;
        vidaJ1--;
        rondaFinalizada = true;
       myPort.write("J1HIT\n");
        
      }
    }

    // Mensajes en la pantalla por ganar o empatar
    if (ganaRondaJugador1 && ganaRondaJugador2) {
      empate();
    }
      else if (ganaRondaJugador1 && rondaActual <= rondaMaxima && vidaJ2 != 1) {
      victoriaJugador1();
    }
      else if (ganaRondaJugador1 && rondaActual <= rondaMaxima && vidaJ2 == 1) {
      muestraResultado();
    }
    
        else if (ganaRondaJugador2 && rondaActual <= rondaMaxima && vidaJ1 != 1) {
      victoriaJugador2();
    }
    
    else if (ganaRondaJugador2 && rondaActual <= rondaMaxima && vidaJ1 == 1) {
      muestraResultado();
    }
    
      else if (ganaRondaJugador2 && rondaActual <= rondaMaxima) {
      muestraResultado();
    }
    
     else if (ganaRondaJugador2 && rondaActual <= rondaMaxima) {
      muestraResultado();
    }
    
    
  } else {
    muestraResultado();
  }
}

void FondoJuego(){
  
//colores fondo
color azul = color(0,0,40);
color gris = color(10,10,80);
color bordeFondo = color(150,200,255);

//Fondo azul
background(azul);
  
//Grilla
int col = 20;
int fil = 20;
stroke(gris);
strokeWeight(2);

//Altura y Ancho de cada celda
float anchoGrilla =(width - borde* 2) / float(col);
float alturaGrilla =(height - borde* 2) / float(fil);

//filas
for (int i = 1; i < fil; i++ ){
  float y = borde + i * alturaGrilla;
  line(borde, y, width - borde, y);
}
//columnas
for (int i = 1; i < col; i++) {
    float x = borde + i * anchoGrilla;
    line(x, borde, x, height - borde);
}

//borde del juego
stroke(bordeFondo);
strokeWeight(borde);
noFill();
rect(borde/2, borde/2, width - borde, height - borde);

}

void jugador1() {
  
  
  fill(cian);
  noStroke();
  
  //Revision de choque con estela solo cuando se mueve
  if(movX1 !=0|| movY1 != 0){
    
//Nueva posicion del pixel  
int nuevaX1 = posicionX1 + (movX1 * tamañoJugador / 3)* aTodoGasJ1;
int nuevaY1 = posicionY1 + (movY1 * tamañoJugador / 3)* aTodoGasJ1;

//Revisa que esta nueva posicion no pase por la estela
int revisionX1 = nuevaX1;
int revisionY1 = nuevaY1;

//revisa hacia la derecha, izquierda,arriba y abajo
if(movX1 == 1){
  revisionX1 = nuevaX1 + tamañoJugador + 1;
      revisionY1 = nuevaY1 + tamañoJugador / 2;
    } else if (movX1 == -1) {
      revisionX1 = nuevaX1 - 1;
      revisionY1 = nuevaY1 + tamañoJugador / 2;
    } else if (movY1 == 1) {
      revisionX1 = nuevaX1 + tamañoJugador / 2;
      revisionY1 = nuevaY1 + tamañoJugador + 1;
    } else if (movY1 == -1) {
      revisionX1 = nuevaX1 + tamañoJugador / 2;
      revisionY1 = nuevaY1 - 1;
    }

//Revisa solamente si esta dentro del espacio de trabajo
if (revisionX1 >= 0 && revisionX1 < width && revisionY1 >= 0 && revisionY1 < height) {
      color pixel1 = get(revisionX1, revisionY1);
      if (pixel1 == cian || pixel1 == naranja){
        ganaRondaJugador2 = true;
          
      
      }
    }


    //limite con el borde
  if(nuevaX1 >= borde && nuevaX1 + tamañoJugador<= width - borde){
  posicionX1 = nuevaX1;
  
  } else{
  ganaRondaJugador2 = true;
    
    }
  if(nuevaY1 >= borde && nuevaY1 + tamañoJugador<= height - borde){
  posicionY1 = nuevaY1;
  }else{
  ganaRondaJugador2 = true;
    
    }

    // Guardar la dirección actual
    direccionX1 = movX1;
    direccionY1 = movY1;

}
  rect(posicionX1, posicionY1 ,tamañoJugador, tamañoJugador);
  
}


void jugador2() {

  fill(naranja);
  noStroke();
  
  //Revision de choque con estela solo cuando se mueve

if (movX2 != 0 || movY2 != 0) {
  //Nueva posicion del pixel
int nuevaX2 = posicionX2 + (movX2 * tamañoJugador / 3)* aTodoGasJ2;
int nuevaY2 = posicionY2 + (movY2 * tamañoJugador / 3)* aTodoGasJ2;

//Revisa que esta nueva posicion no pase por la estela
int revisionX2 = nuevaX2;
int revisionY2 = nuevaY2;

//revisa hacia la derecha, izquierda,arriba y abajo
if (movX2 == 1) {
      revisionX2 = nuevaX2 + tamañoJugador + 1;
      revisionY2 = nuevaY2 + tamañoJugador / 2;
    } else if (movX2 == -1) {
      revisionX2 = nuevaX2 - 1;
      revisionY2 = nuevaY2 + tamañoJugador / 2;
    } else if (movY2 == 1) {
      revisionX2 = nuevaX2 + tamañoJugador / 2;
      revisionY2 = nuevaY2 + tamañoJugador + 1;
    } else if (movY2 == -1) {
      revisionX2 = nuevaX2 + tamañoJugador / 2;
      revisionY2 = nuevaY2 - 1;
    }

//Revisa solamente si esta dentro del espacio de trabajo
    if (revisionX2 >= 0 && revisionX2 < width && revisionY2 >= 0 && revisionY2 < height) {
      color pixel2 = get(revisionX2, revisionY2);
      if (pixel2 == cian || pixel2 == naranja) {
        ganaRondaJugador1 = true;
          
      }
    }


  //Limites con el borde
  if(nuevaX2 >= borde && nuevaX2 + tamañoJugador<= width - borde){
  
  posicionX2 = nuevaX2;
  }else{
   ganaRondaJugador1 = true;
     
    }
  
  if(nuevaY2 >= borde && nuevaY2 + tamañoJugador<= height - borde){
  posicionY2 = nuevaY2;
  }else{
  ganaRondaJugador1 = true;
    
    }
    
    direccionX2 = movX2;
    direccionY2 = movY2;
    }

  rect(posicionX2, posicionY2 ,tamañoJugador, tamañoJugador);

}


void serialEvent(Serial p) { 
  comandoArduino = p.readStringUntil('\n'); 
  comandoArduino = comandoArduino.trim(); 

  if (juegoActivo == true) { 
    // Movimientos jugador 1 (J2)
    if (comandoArduino.equals("J2U") && direccionY1 != 1) { movX1 = 0; movY1 = -1; } 
    else if (comandoArduino.equals("J2D") && direccionY1 != -1) { movX1 = 0; movY1 = 1; } 
    else if (comandoArduino.equals("J2L") && direccionX1 != 1) { movX1 = -1; movY1 = 0; } 
    else if (comandoArduino.equals("J2R") && direccionX1 != -1) { movX1 = 1; movY1 = 0; }
    else if (comandoArduino.equals("cambio1") && aTodoGasUsadoJ1 == false) {aTodoGasJ1 = 2; aTodoGasInicioJ1 = millis();aTodoGasUsadoJ1 = true; }
    

    // Movimientos jugador 2 (J1), con desbloqueo de movimiento
    if (bloquearMovimientoJ1 == false) {
      if (comandoArduino.equals("J1U") && direccionY2 != 1) { movX2 = 0; movY2 = -1;  } 
      else if (comandoArduino.equals("J1D") && direccionY2 != -1) { movX2 = 0; movY2 = 1;  } 
      else if (comandoArduino.equals("J1L") && direccionX2 != 1) { movX2 = -1; movY2 = 0;  }
      else if (comandoArduino.equals("J1R") && direccionX2 != -1) { movX2 = 1; movY2 = 0;  }
      else if (comandoArduino.equals("cambio2") && aTodoGasUsadoJ2 == false) {aTodoGasJ2 = 2; aTodoGasInicioJ2 = millis();aTodoGasUsadoJ2 = true; }
    }
  }

  // === Control del sistema ===
  if (comandoArduino.equals("J1U") && rondaFinalizada && !partidaFinalizada) {
    reiniciarJuego();
    pintarFondo = true;
    bloquearMovimientoJ1 = false;
  
    

  }

  if (comandoArduino.equals("J1L") && partidaFinalizada == true) {
    partidaNueva();
    bloquearMovimientoJ1 = false;
    myPort.write("REINICIAR\n"); 
    
  }

  if (comandoArduino.equals("J1R") && juegoActivo == false) {
    pintarFondo = true;
    juegoActivo = true;
    bloquearMovimientoJ1 = false;
    myPort.write("LCD_VIDAS\n");
     
  }
}

void keyPressed() {
  // controles jugador 1 WASD



  if(juegoActivo == true){
    
    //Aumento de velocidad
        if (key == 'o' || key == 'O' && aTodoGasUsadoJ1 == false) {
        
        aTodoGasJ1 = 2;
        aTodoGasInicioJ1 =millis();
        aTodoGasUsadoJ1 = true;
        }
        
        if (key == 'p' || key == 'P' && aTodoGasUsadoJ2 == false) {
        
        aTodoGasJ2 = 2;
        aTodoGasInicioJ2 =millis();
        aTodoGasUsadoJ2 = true;
        }
    
    
    //Permite ir hacia arriba y no permite ir directamente hacia la direccion contraria
    if ((key == 'w' || key == 'W') && direccionY1 != 1) {
        movX1 = 0;
        movY1 = -1;

        //Permite ir hacia abajo y no permite ir directamente hacia la direccion contraria
        }else if ((key == 's' || key == 'S')  && direccionY1 != -1) {
        movX1 = 0;
        movY1 = 1;
}
        //Permite ir hacia la izquierda y no permite ir directamente hacia la direccion contraria
        else if ((key == 'a' || key == 'A') && direccionX1 != 1) {
        movX1 = -1;
        movY1 = 0;
}
        //Permite ir hacia la derechay no permite ir directamente hacia la direccion contraria
        else if ((key == 'd' || key == 'D') && direccionX1 != -1){
        movX1 = 1;
        movY1 = 0;
}


//controles jugador 2 flechas

//Permite ir hacia arriba y no permite ir directamente hacia la direccion contraria
   if(keyCode == UP && direccionY2 != 1){
        movX2 = 0;
        movY2 = -1;
}

//Permite ir hacia abajo y no permite ir directamente hacia la direccion contraria
else if (keyCode == DOWN && direccionY2 != -1) {
        movX2 = 0;
        movY2 = 1;
}

//Permite ir hacia la derecha y no permite ir directamente hacia la direccion contraria
else if (keyCode == RIGHT && direccionX2 != -1) {
        movX2 = 1;
        movY2 = 0;
}

//Permite ir hacia la izquierda y no permite ir directamente hacia la direccion contraria
else if (keyCode == LEFT && direccionX2 != 1) {
        movX2 = -1;
        movY2 = 0;
}


}

//reinicio de Ronda
if ((key == 'r' || key == 'R') && rondaFinalizada == true && partidaFinalizada == false){
  reiniciarJuego();
  }
 
 //Nueva partida
 if(partidaFinalizada == true && key == 'y' || key == 'Y'){
    myPort.write("REINICIAR\n"); ///////////
  partidaNueva(); 
  
}
// Comenzar el juego
 if(juegoActivo == false && keyCode == ENTER ){
 pintarFondo= true;
 juegoActivo = true;
  myPort.write("LCD_VIDAS\n"); ////////////////////////////////////
}
}

void partidaNueva(){
  
vidaJ1 = 3;
vidaJ2 = 3;
rondaActual = 1;
ganaRondaJugador1 = false;
ganaRondaJugador2 = false;
rondaFinalizada = false;
juegoActivo = false;
pintarFondo = false;
partidaFinalizada = false;
guardaGanador = false;

  posicionX1 = width / 2 - 100;
  posicionY1 = height / 2;
  posicionX2 = width / 2 + 100;
  posicionY2 = height / 2;
  
  //direccion en 0
  direccionX1 = 0;
  direccionY1 = 0;
  direccionX2 = 0;
  direccionY2 = 0;

  // Velocidades en 0
  movX1 = 0;
  movY1 = 0;
  movX2 = 0;
  movY2 = 0;

 // reinicia la velocida J1
 aTodoGasJ1 = 1;
 aTodoGasUsadoJ1 = false;
 
  // reinicia la velocida J2
  aTodoGasJ2 = 1;
 aTodoGasUsadoJ2 = false;
 bloquearMovimientoJ1 = false;

inicio();
myPort.write("LCD_PREP\n");
  
}
void muestraResultado() {
  partidaFinalizada = true;
  if(vidaJ1 < vidaJ2){
  background(0,0,0);
  textAlign(CENTER, CENTER);
  fill(naranja);
  textSize(80);
  text("GANADOR JUGADOR 2!", width / 2, height / 2);
  fill(255);
  textSize(22);
  text("J2 presiona 'IZQUIERDA' para comenzar una nueva partida", width / 2, height / 2 + 50);
  if(guardaGanador==false){
  myPort.write("G2HIT\n");
  guardaGanador = true;
  }
  }
else{
  background(0,0,0);
  textAlign(CENTER, CENTER);
  fill(cian);
  textSize(80);
  text("GANADOR JUGADOR 1!", width / 2, height / 2);
  fill(255);
  textSize(22);
  text("J2 presiona 'IZQUIERDA' para comenzar una nueva partida", width / 2, height / 2 + 50);
    if(guardaGanador==false){
  myPort.write("G1HIT\n");
  guardaGanador = true;
  }
}
}


void empate() {
  fill(0, 0, 0, 5);
  noStroke();
  rect(0, 0, width, height);
  textAlign(CENTER, CENTER);
  fill(255, 255, 255);
  textSize(64);
  text("EMPATE", width / 2, height / 2);
  fill(255);
  textSize(22);
  text("J2 Presiona 'DERECHA' para la siguiente ronda", width / 2, height / 2 + 50);
}

void inicio() {
    background(0,0,0);
  textAlign(CENTER, CENTER);
  fill(255, 255, 255);
  textSize(120);
  text("TRON", width / 2, height / 2);
  fill(255);
  textSize(22);
  text("J2 presiona 'DERECHA' para jugar", width / 2, height / 2 + 70);
  
}

void victoriaJugador1() {
  fill(0, 0, 0, 5);
  noStroke();
  rect(0, 0, width, height);
  textAlign(CENTER, CENTER);
  fill(255, 255, 255);
  textSize(64);
  text("GANA LA RONDA JUGADOR 1", width / 2, height / 2);
  fill(255);
  textSize(22);
  text("J2 presiona 'ARRIBA' para la siguiente ronda", width / 2, height / 2 + 50);
  
}

void victoriaJugador2() {
  
  fill(0, 0, 0, 5);
  noStroke();
  rect(0, 0, width, height);
  textAlign(CENTER, CENTER);
  fill(255, 255, 255);
  textSize(64);
  text("GANA LA RONDA JUGADOR 2", width / 2, height / 2);
  fill(255);
  textSize(22);
  text("J2 presiona 'ARRIBA' para la siguiente ronda", width / 2, height / 2 + 50);
}


void reiniciarJuego() {
  // Posiciones iniciales
  posicionX1 = width / 2 - 100;
  posicionY1 = height / 2;
  posicionX2 = width / 2 + 100;
  posicionY2 = height / 2;
  
  //direccion en 0
  direccionX1 = 0;
  direccionY1 = 0;
  direccionX2 = 0;
  direccionY2 = 0;

  // Velocidades en 0
  movX1 = 0;
  movY1 = 0;
  movX2 = 0;
  movY2 = 0;

 // reinicia la velocida J1
 aTodoGasJ1 = 1;
 aTodoGasUsadoJ1 = false;
 
  // reinicia la velocida J2
  aTodoGasJ2 = 1;
 aTodoGasUsadoJ2 = false;
 
 
//reinicia variables globales
  ganaRondaJugador1 = false;
  ganaRondaJugador2 = false;
  rondaFinalizada = false;
  bloquearMovimientoJ1 = true;
  
   // Redibuja el fondo
  FondoJuego();
  

}
