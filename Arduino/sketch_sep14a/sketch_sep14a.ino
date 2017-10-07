#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <SimpleSDAudio.h>
#include <SoftwareSerial.h>

#define DEBUGG_MODE 1

MPU6050 mpu;
SoftwareSerial BTserial(3,7); // RX | TX
int16_t ax, ay, az;
int16_t gx, gy, gz;
double normal;
double normal2;
int contadorDeTurno=0;
int rojo=0;
int azul=0; 
char estado='x';
char color='A';


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
   //sonarSable();

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

   BTserial.begin(38400); 

  Serial.print("ready");
}

/**
  Método en el que se programa la funcionalidad
*/
void loop() {
    analogWrite(PIN_COLOR_AZUL,azul);
    analogWrite(PIN_COLOR_ROJO,rojo);
    
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
   encendido = reedEncendido();
  if(encendido){
    azul=255;
    rojo=0;
    setearColor(azul,rojo);
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
          sensar();
          if(SdPlay.isStopped()){
            SdPlay.play();
          }    
     }
    SdPlay.setFile("off.wav");
    SdPlay.play();
    analogWrite(PIN_VIBRADOR_DIGITAL,255);
    while(!SdPlay.isStopped()){}
    azul=0;
    rojo=0;
    setearColor(azul,rojo);
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
  }
}

 

/*
 * devuelve verdadero si el reed detecta el magnetismo
 */
boolean reedEncendido(){
  if(digitalRead(PIN_REED_DIGITAL) == HIGH){
      return true;
  }
  else{
    return false;
  }
}

/*
 * obtiene el valor sensado por el ldr
 * @author pablo
 */
void sensarLuz(){
    luz=analogRead(PIN_SENSOR_LDR_ANALOGICO);
    Serial.print("Luz: ");
    Serial.println(luz);
    if(luz>220){
        azul=255;
        rojo=255;
        setearColor(azul,rojo);
    }else{
        azul=azul-215<0?0:azul-215;
        rojo=rojo-215<0?0:rojo-215;
        setearColor(azul,rojo);
      
    }
}
 /*
 * obtiene el valor sensado por el acelerometro
 */
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
      sensar();
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
 * obtiene el valor enviado por bluetooth
 */
void leerBluetooth(){
  Serial.println("leyendo BT");
  if(BTserial.available()>0){
    estado=(char)BTserial.read();
    Serial.println(estado);
    switch(estado){
      case '0':
        Serial.println("DEBIL");
        if(color='A'){
          azul=50;
          rojo=0;
        }else if(color='R'){
          azul=0;
          rojo=50;
        }else{
          azul=50;
          rojo=50;
        }
        setearColor(azul,rojo);
      break;
      case '1':
        Serial.println("FUERTE");
       if(color='A'){
          azul=255;
          rojo=0;
        }else if(color='R'){
          azul=0;
          rojo=255;
        }else{
          azul=255;
          rojo=255;
        }
        setearColor(azul,rojo);
      break;
      case '2':
         Serial.println("CHOQUE");
         SdPlay.setFile("swing.wav");
         SdPlay.play();
         while(!SdPlay.isStopped()){
          sensar();
         }
         break;
       case '3':
         Serial.println("ACELERACION");
         SdPlay.setFile("on.wav");
         SdPlay.play();
         while(!SdPlay.isStopped()){
          sensar();
         }
         break;
        case 'A':
         azul=255;
         rojo=0;
         color='A';
         setearColor(azul,rojo);
        break;
        case 'R':
         azul=0;
         rojo=255;
         color='R';
         setearColor(azul,rojo);
        break;
        case 'V':
         azul=255;
         rojo=255;
         color='V';
         setearColor(azul,rojo);
        break;

    }
  }
}

/*
 * setea el color del sable
 * @author pablo
 */
void setearColor(int azul, int rojo){
   analogWrite(PIN_COLOR_AZUL,azul);
   analogWrite(PIN_COLOR_ROJO,rojo);
}


/*
 * Llama a los metodos que leen los sensores y el BT.
 */
void sensar(){
    leerBluetooth();      
    sensarLuz();
    encendido=reedEncendido();
}

