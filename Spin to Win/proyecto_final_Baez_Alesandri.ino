#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

#define PIN 2
#define NUMPIXELS 45 

LiquidCrystal_I2C lcd(0x27,16,2);

unsigned long start;
unsigned long current;
unsigned long elapsed;
unsigned long record=10000000000;

int prendidas=10;

int lectura;
int lecturamap;
int potenciometro = A0;
int led1=4;
int led2=5;
int led3=6;
int led4=7;
int boton=10;
int lecturaboton;
int controlador=0;

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

int objetivo=random(1, 4);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.clear();

  start = millis();
  

  pinMode(boton, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  pixels.setBrightness(50);

  pixels.begin();
  pinMode(potenciometro, INPUT);
  Serial.begin(9600);
}

void loop() {
  lecturaboton=digitalRead(boton);
  current=millis();
  
  lectura=analogRead(potenciometro); 
  lecturamap=map(lectura, 1003, 20, 0, 44);
  Serial.println(lecturaboton);
  
  if(lecturamap!=(objetivo*13)-10 && controlador<prendidas){
    delay(10);
    if (objetivo==1){
      digitalWrite(led1, HIGH);
      pixel();
    }
    else if (objetivo==2){
      digitalWrite(led2, HIGH);
      pixel();
    }
    else if (objetivo==3){
      digitalWrite(led3, HIGH);
      pixel();
    }    
    else if (objetivo==4){
      digitalWrite(led4, HIGH);
      pixel();
    }
  }
  else if(lecturamap==(objetivo*13)-10 && controlador<prendidas){
    objetivo=random(1, 5);
    controlador+=1;
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
  }
  else if(controlador==20){
    setColor();
    
    for(int i=0; i < NUMPIXELS && controlador !=0; i++){
      pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
      pixels.show();
      lecturaboton=digitalRead(boton);
      if(lecturaboton==1){
        controlador=0;
        for(int c=0; c < NUMPIXELS; c++){
      		pixels.setPixelColor(c, pixels.Color(0, 0, 0));
      		pixels.show();
          delay(10);
        }
        start = millis();
        break;
      }
      delay(100);
    }
  }
  else{
    elapsed = current - start;
    controlador=20;
    lcd.clear();
    if(record>elapsed){
      record=elapsed;
    }
    Serial.println(record);
    lcd.setCursor(0,0);
    lcd.print("Record: ");
    lcd.setCursor(8,0);
    lcd.print(record);
    lcd.setCursor(0,1);
    lcd.print("Intento: ");
    lcd.setCursor(9,1);
    lcd.print(elapsed);

  }
  
  pixels.show();
  }

void setColor(){
  redColor = random(0, 255);
  greenColor = random(0,255);
  blueColor = random(0, 255);
}
void pixel(){
  pixels.setPixelColor(lecturamap-1, pixels.Color(0, 0, 0));
  pixels.setPixelColor(lecturamap+1, pixels.Color(0, 0, 0));
  pixels.setPixelColor(lecturamap-2, pixels.Color(0, 0, 0));
  pixels.setPixelColor(lecturamap+2, pixels.Color(0, 0, 0));
  pixels.setPixelColor(lecturamap-3, pixels.Color(0, 0, 0));
  pixels.setPixelColor(lecturamap+3, pixels.Color(0, 0, 0));
  pixels.setPixelColor(lecturamap, pixels.Color(0, 255, 255));
}