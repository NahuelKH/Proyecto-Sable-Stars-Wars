#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <SimpleSDAudio.h>

#define DEBUGG_MODE 1

MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;
double normal;
double normal2;
int contadorDeTurno=0;

//Constantes para pines de sensores
#define PIN_SENSOR_LDR_ANALOGICO A3
#define PIN_SCL_6050_ANALOGICO A5
#define PIN_SDA_6050_ANALOGICO A4
#define PIN_REED_DIGITAL 8

//Constantes para pines de Actuadores
#define PIN_CS_TARJETA_SD_DIGITAL 4
#define PIN_VIBRADOR_DIGITAL 10
#define PIN_COLOR_AZUL 5
#define PIN_COLOR_ROJO 6

// Supuestamente se necesita esto para haer andar el LED RGB Anodo
#define COMMON_ANODE

#define DEFAULT_COLOR 0

//umbrales
#define UMBRAL_ACELEROMETRO 2000
#define UMBRAL_LUZ_AMBIENTAL 100
#define UMBRAL_LUZ_BRILLO 120
#define UMBRAL_LUZ_OSCURIDAD 90
//contadores de sonido
int attack = 0;
int nonAttack = 0;
int semAttack = 0;
int luz=0;
boolean encendido=true;
/**
  Método que inicializa/configura el arduino.
*/
void setup() {
  // Seteo de pines ANALOGICOS
  //pinMode(PIN_SENSOR_LDR_ANALOGICO, INPUT);
  Wire.begin();
  Serial.begin(9600);
  // Seteo de pines DIGITALES
  pinMode(PIN_REED_DIGITAL, INPUT);

  //configuracion de vibrador
    pinMode(PIN_VIBRADOR_DIGITAL, OUTPUT);
     Serial.println("Apagamos");
  //fin
  //configuracion luces
    pinMode(PIN_COLOR_ROJO, OUTPUT);
    pinMode(PIN_COLOR_AZUL, OUTPUT);
  //fin
  //configuracion sonido
  SdPlay.setSDCSPin(PIN_CS_TARJETA_SD_DIGITAL);
  if(!SdPlay.init(SSDA_MODE_FULLRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER)){
	  Serial.println("Error en el modulo SD");
  }else{
    Serial.println("Modulo ok");
    }
  if(!SdPlay.setFile("quieto.wav")){
  	Serial.println("No se encontro el archivo de audio");
  }else{
    Serial.println("sonido ok");
    }
  //fin
  Serial.println("Reproduciendo sonido");
   sonarSable();

Serial.println("Colores");
//activarColorYVibrador();
   
  //configuracion mpu
  mpu.initialize();
  pinMode(PIN_SCL_6050_ANALOGICO, INPUT);
  pinMode(PIN_SDA_6050_ANALOGICO, INPUT);
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
  mpu.setXAccelOffset(-1537);
  mpu.setYAccelOffset(53);
  mpu.setZAccelOffset(1190);
  mpu.setXGyroOffset(-70);
  mpu.setYGyroOffset(63);
  mpu.setZGyroOffset(-14);
  //fin

  Serial.print("ready");
}

/**
  Método en el que se programa la funcionalidad
*/
void loop() {
    analogWrite(PIN_COLOR_AZUL,0);
    analogWrite(PIN_COLOR_ROJO,0);
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
   encendido = reedEncendido();
  if(encendido){
    analogWrite(PIN_COLOR_AZUL,0);
    analogWrite(PIN_COLOR_ROJO,255);
    SdPlay.setFile("on.wav");
    SdPlay.play();
    while(!SdPlay.isStopped()){ 
      analogWrite(PIN_VIBRADOR_DIGITAL,255);
    }
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
    SdPlay.setFile("quieto.wav");
    SdPlay.play();
    while(encendido){      
          sensarMovimiento();
          sensarLuz();
          encendido=reedEncendido();
          if(SdPlay.isStopped()){
            SdPlay.play();
          }    
     }
    SdPlay.setFile("off.wav");
    SdPlay.play();
    analogWrite(PIN_VIBRADOR_DIGITAL,255);
    while(!SdPlay.isStopped()){}
    analogWrite(PIN_COLOR_AZUL,0);
    analogWrite(PIN_COLOR_ROJO,0);
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
  }
}

 
 /* 
  Serial.println("leyendo");
  sensarMovimiento();
  sensarLuz();
  Serial.print("El reed es: ");
  int valor=digitalRead(PIN_REED_DIGITAL);
   if(valor==1){
       analogWrite(PIN_COLOR_AZUL,0);
      analogWrite(PIN_COLOR_ROJO,0);
    }else{
      analogWrite(PIN_COLOR_AZUL,255);
      analogWrite(PIN_COLOR_ROJO,0);
      }
//  delay(2000);
}
*/

boolean reedEncendido(){
  if(digitalRead(PIN_REED_DIGITAL) == HIGH){
      return true;
  }
  else{
    return false;
  }
}

void sensarLuz(){
    luz=analogRead(PIN_SENSOR_LDR_ANALOGICO);
    Serial.print("Luz: ");
    Serial.println(luz);
}
 
void sensarMovimiento(){
	mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	if(DEBUGG_MODE){
		Serial.print(ax);
		Serial.print(" ");
		Serial.print(ay);
		Serial.print(" ");
		Serial.print(az);
		Serial.print(" ");

		Serial.print(gx);
		Serial.print(" ");
		Serial.print(gy);
		Serial.print(" ");
		Serial.print(gz);
		Serial.println();
	}
	
	//calculo la normal de las aceleraciones
	//aca utilizo un contador para poder guardar el valor del sensado en 2 variables diferentes y asi poder hacer las comparaciones
	if(contadorDeTurno%2==0){
		normal=sqrt(pow(ax,2)+pow(ay,2)+pow(az,2));
	}else{
		normal2=sqrt(pow(ax,2)+pow(ay,2)+pow(az,2));  
	}
	contadorDeTurno++;
	//reseteo el contador para que no se vaya a la mierda
	if(contadorDeTurno==99){
		contadorDeTurno=0;
    }
	
	//para mi hay que usar una logica de contadores(en tiempo) como dijo el profe
	if(hayMovimiento(abs(normal-normal2))){
	  SdPlay.setFile("swing.wav");
    SdPlay.play();
    while(!SdPlay.isStopped()){
      analogWrite(PIN_VIBRADOR_DIGITAL,255);
      encendido = reedEncendido();
      sensarLuz();
    }
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
    Serial.println("hay movimiento");
    SdPlay.setFile("quieto.wav");
    SdPlay.play();
	}else{
	  Serial.println("*******************************************************neutro");
	}
} 

/*
   método que indicará si el sable de luz produjo un movimiento.
   @author: Pablo
*/
//detecta movimiento segun el umbral
bool hayMovimiento(double movimiento) {
  bool flagMovimiento = 0;
  if(movimiento>UMBRAL_ACELEROMETRO){
    flagMovimiento = 1;
   // normal=0;
  }
  return flagMovimiento;
}

/*
   método que hara sonar el sable cuando se mueva.
   @author: Pablo
*/
void sonarSable() {
	SdPlay.play();
	while(!SdPlay.isStopped())
	{ 
     // sensarLuz();
	}

 /*
	if(attack==1){
		//reproducir sonido 1
	}else if(attack==2){
		//reproducir sonido 2
	}else if(attack==3){
		//reproducir combo
		//ATTACK=0;
	}
	if(nonAttack==1){
		//reproducir sonido neutro
	}
	//por ahora asi porque no se implemento nada
	attack=0;
	nonAttack=0;
 */
}

/*
 * método que setea el color del LED RGB
 * @author: Martin
 */

 /*
void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif

 
}
*/

void activarColor(){
  delay(2000);
  analogWrite(PIN_COLOR_ROJO,255);
  delay(2000);
  Serial.println("mitad de color");
  analogWrite(PIN_COLOR_ROJO,128);
  delay(5000);
  Serial.println("Apagate");
  analogWrite(PIN_COLOR_ROJO,0);
  delay(2000);
  analogWrite(PIN_COLOR_ROJO,255);
  }

  void activarColorYVibrador(){
  analogWrite(PIN_COLOR_ROJO,50);
  analogWrite(PIN_VIBRADOR_DIGITAL,0);
  delay(5000);
  analogWrite(PIN_COLOR_ROJO,0);
  analogWrite(PIN_COLOR_AZUL,100);
  analogWrite(PIN_VIBRADOR_DIGITAL,255);
  delay(2000);
  analogWrite(PIN_COLOR_AZUL,100);
  analogWrite(PIN_VIBRADOR_DIGITAL,0);
  analogWrite(PIN_COLOR_ROJO,100);
  delay(2000);
  analogWrite(PIN_COLOR_AZUL,0);
  analogWrite(PIN_COLOR_ROJO,0);
  }

  
