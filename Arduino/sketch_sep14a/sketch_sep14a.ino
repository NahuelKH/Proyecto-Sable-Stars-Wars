#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>

#define DEBUGG_MODE 0

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
#define PIN_REED_DIGITAL 5

//Constantes para pines de Actuadores
#define PIN_SCK_TARJETA_SD_DIGITAL 13
#define PIN_MISO_TARJETA_SD_DIGITAL 12
#define PIN_MOSI_TARJETA_SD_DIGITAL 11
#define PIN_CS_TARJETA_SD_DIGITAL 4
#define PIN_PARLANTE_DIGITAL 9
#define PIN_VIBRADOR_DIGITAL 2

// Supuestamente se necesita esto para haer andar el LED RGB Anodo
#define COMMON_ANODE

#define DEFAULT_COLOR 0

//umbrales
#define UMBRAL_ACELEROMETRO 3000
#define UMBRAL_LUZ_AMBIENTAL 100
#define UMBRAL_LUZ_BRILLO 120
#define UMBRAL_LUZ_OSCURIDAD 90
//contadores de sonido
int attack = 0;
int nonAttack = 0;
int semAttack = 0;
int luz=0;
/**
  Método que inicializa/configura el arduino.
*/
void setup() {
  // Seteo de pines ANALOGICOS
  //pinMode(PIN_SENSOR_LDR_ANALOGICO, INPUT);
  Wire.begin();
  Serial.begin(9600);
  mpu.initialize();
  pinMode(PIN_SCL_6050_ANALOGICO, INPUT);
  pinMode(PIN_SDA_6050_ANALOGICO, INPUT);
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
  // Seteo de pines DIGITALES
  pinMode(PIN_REED_DIGITAL, INPUT);
/*  
  pinMode(PIN_SCK_TARJETA_SD_DIGITAL, OUTPUT);
  pinMode(PIN_MISO_TARJETA_SD_DIGITAL, OUTPUT);
  pinMode(PIN_MOSI_TARJETA_SD_DIGITAL, OUTPUT);
  pinMode(PIN_CS_TARJETA_SD_DIGITAL, OUTPUT);
  pinMode(PIN_PARLANTE_DIGITAL, OUTPUT);
  pinMode(PIN_VIBRADOR_DIGITAL, OUTPUT);

  setColor(DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR);
  */
  
  //configuracion mpu
  mpu.setXAccelOffset(-1577);
  mpu.setYAccelOffset(61);
  mpu.setZAccelOffset(1171);
  mpu.setXGyroOffset(-70);
  mpu.setYGyroOffset(36);
  mpu.setZGyroOffset(-16);
  //fin

  Serial.print("ready");
}

/**
  Método en el que se programa la funcionalidad
*/
void loop() {
	Serial.println("leyendo");
	sensarMovimiento();
  sensarLuz();
	delay(2000);
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
		//reproducir sonido
   Serial.println("hay movimiento");
		attack++;
		sonarSable();
	}else{
		//preparar logica para el neutro
    Serial.println("*******************************************************neutro");
		nonAttack++;
		sonarSable();
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
    normal=0;
  }
  return flagMovimiento;
}

/*
   método que hara sonar el sable cuando se mueva.
   @author: Pablo
*/
void sonarSable() {
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
