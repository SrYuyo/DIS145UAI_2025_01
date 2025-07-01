#include <DIYables_Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

String fase;
int ledSol = 11;
int ledLuna = 12;

// TIEMPOS
const unsigned long t_turno = 30000;
unsigned long t_duracion = 0;
unsigned long t_fase_inicio = 0;
unsigned long t_ultimo_segundo = 0;

bool fase_iniciada = false;

const int numFilas = 4;
const int numCols = 4;
char enter = '#';
char skip = 'D';
char borrar = 'A';

String entrada = "";
int jugadores = 0;
int ronda = 0;
int rondas_totales;

LiquidCrystal_I2C pantalla(0x27, 16, 2);

char matrizKeypad[numFilas][numCols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};

byte pinFilas[numFilas] = { 2,3,4,5 };
byte pinCols[numCols] = { 6,7,8,9 };

DIYables_Keypad teclado = DIYables_Keypad(makeKeymap(matrizKeypad), pinFilas, pinCols, numFilas, numCols);

void setup() {
  Serial.begin(9600);
  pinMode(ledSol, OUTPUT);
  pinMode(ledLuna, OUTPUT);
  
  pantalla.init();        
  pantalla.backlight();
  
  pantalla.setCursor(0, 0);
  pantalla.print("N de jugadores:");
  fase = "comienzo";
}

void loop() {
  char tecla_presionada = teclado.getKey();

  if (fase == "comienzo") {
    digitalWrite(ledLuna, LOW);
    digitalWrite(ledSol, LOW);

    if (tecla_presionada != NULL) { /*El máximo de carácteres es 2 y entre 0 a 9*/
      if (tecla_presionada >= '0' && tecla_presionada <= '9') {
        if (entrada.length() < 2) {
          entrada = entrada + tecla_presionada;
          pantalla.setCursor(0, 1);
          pantalla.print(entrada);
          delay(50);
          jugadores = entrada.toInt(); /*Convierte la variable jugadores a un número*/
        }
      } else if (tecla_presionada == enter) { /*Guarda el número de jugadores seleccionados*/
        pantalla.clear();
        pantalla.setCursor(0, 0);
        pantalla.print("Total: " + String(jugadores));
        pantalla.setCursor(0, 1);
        pantalla.print("Comienza!");
        delay(1500);
        pantalla.clear();
        t_fase_inicio = millis();
        t_duracion = jugadores * t_turno;
        t_ultimo_segundo = millis();
        fase_iniciada = false;
        fase = "noche";
      } else if (tecla_presionada == borrar) { /*Puedes borrar si te equivocas al insertar la cantidad de jugadores*/
        entrada = "";
        pantalla.setCursor(0, 1);
        pantalla.print("                ");
      }
    }
  }

  if (fase == "noche") { /*Comienzo de la fase nocturna, enciende el led correspondiente y anuncia la llegada de la noche*/
    if (!fase_iniciada) {
      digitalWrite(ledLuna, HIGH);
      digitalWrite(ledSol, LOW);
      pantalla.clear();
      pantalla.print("Es de noche");
      delay(500);
      pantalla.clear();
      t_fase_inicio = millis();
      t_ultimo_segundo = millis();
      fase_iniciada = true;
      
    }

    if (millis() - t_ultimo_segundo >= 1000) {
      unsigned long tiempo_restante = (t_duracion - (millis() - t_fase_inicio)) / 1000;
      pantalla.setCursor(0, 1);
      pantalla.print("Tiempo: ");
      pantalla.print(tiempo_restante);
      pantalla.print("     ");
      t_ultimo_segundo = millis();
    }

    if (millis() - t_fase_inicio >= t_duracion || tecla_presionada == skip) {
      fase = "dia";
      t_duracion = jugadores * t_turno;/*Tiempo de la fase de día */
      fase_iniciada = false;
	  return; /*Vuelve al inicio del loop para que no se salte la fase de día*/
    }
  }

  if (fase == "dia") {
    if (!fase_iniciada) {
      digitalWrite(ledLuna, LOW);
      digitalWrite(ledSol, HIGH);
      pantalla.clear();
      pantalla.print("Es de dia");
      delay(500);
      pantalla.clear();
      t_fase_inicio = millis();
      t_ultimo_segundo = millis();
      fase_iniciada = true;
    }

    if (millis() - t_ultimo_segundo >= 1000) {
      unsigned long tiempo_restante = (t_duracion - (millis() - t_fase_inicio)) / 1000;
      pantalla.setCursor(0, 1);
      pantalla.print("Tiempo: ");
      pantalla.print(tiempo_restante);
      pantalla.print("     ");
      t_ultimo_segundo = millis();
      unsigned long t_total = t_duracion/1000;
      
      /*Subfases con tiempos concretos votacion = 10 segundos, discusion y defensa duran la mitad del total del turno menos 20 segundos*/
      if(t_total*2/3 < tiempo_restante && tiempo_restante < t_total){
      pantalla.setCursor(0, 0);
      pantalla.print("Discusion");
      }
      else if((t_total*2/3)-10 < tiempo_restante && tiempo_restante < t_total*2/3){
      pantalla.setCursor(0, 0);
      pantalla.print("Votacion  ");
      }
      else if(t_total/3 < tiempo_restante && tiempo_restante < (t_total*2/3)-10){
      pantalla.setCursor(0, 0);
      pantalla.print("Defensa   ");
      }
      else if (tiempo_restante < 10){
      pantalla.setCursor(0, 0);
      pantalla.print("Votacion   ");
      }
    }

    if (millis() - t_fase_inicio+10000 >= t_duracion || tecla_presionada == skip) {
      fase = "noche";
      t_duracion = jugadores * t_turno; /*Tiempo del turno de noche*/
      fase_iniciada = false;
      return; 
    }
  }
  else if(tecla_presionada =='B'){
      digitalWrite(ledLuna, LOW);
      digitalWrite(ledSol, LOW);
      pantalla.clear();
      pantalla.print("Gracias");
      delay(1000);
      fase = "comienzo";   
  }
}
