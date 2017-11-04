<<<<<<< Updated upstream
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
//#include <SimpleSDAudio.h>
#include <SoftwareSerial.h>
//#include <SD.h>
//#include <TMRpcm.h>

#define DEBUGG_MODE 1

MPU6050 mpu;

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

#define SD_ChipSelectPin 4

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
TMRpcm tmrpcm;
SoftwareSerial BTserial(3,7); // RX | TX
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
  //configuracion sonido asdasd
  tmrpcm.speakerPin = 9;
   if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
    return;   // don't do anything more if not
  }else{
    Serial.println("todo okkkkkkkkkk");
     tmrpcm.play("on.wav");
    }
 /* SdPlay.setSDCSPin(PIN_CS_TARJETA_SD_DIGITAL);
  if(!SdPlay.init(SSDA_MODE_FULLRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER)){
	  Serial.println("Error en el modulo SD");
  }else{
    Serial.println("Modulo ok");
    }
  if(!SdPlay.setFile("quieto.wav")){
  	Serial.println("No se encontro el archivo de audio");
  }else{
    Serial.println("sonido ok");
    }*/
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

   BTserial.begin(9600); 
delay(5000);
  Serial.print("ready");
}

/**
  Método en el que se programa la funcionalidad
*/
void loop() {
  Serial.println("Leyendo bluetooth");
   delay(2000);
  leerBluetooth();
  delay(2000);
  tmrpcm.play("on.wav");
  delay(3000);
    analogWrite(PIN_COLOR_AZUL,azul);
    analogWrite(PIN_COLOR_ROJO,rojo);
    
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
   encendido = reedEncendido();
  if(encendido){
    azul=255;
    rojo=0;
    setearColor(azul,rojo);
    //SdPlay.setFile("on.wav");
    //SdPlay.play();
   /* while(!SdPlay.isStopped()){ 
      analogWrite(PIN_VIBRADOR_DIGITAL,255);
    }*/
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
  //  SdPlay.setFile("quieto.wav");
    //SdPlay.play();
    while(encendido){
          sensarMovimiento();
          sensar();
        /*  if(SdPlay.isStopped()){
            SdPlay.play();
          }*/    
     }
  //  SdPlay.setFile("off.wav");
    //SdPlay.play();
    analogWrite(PIN_VIBRADOR_DIGITAL,255);
    //while(!SdPlay.isStopped()){}
    azul=0;
    rojo=0;
    setearColor(azul,rojo);
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
  }
}
=======
/*Nota: Pasos para configuración del modulo HC05 con Boton
 * 1) Realizar la conexion
 *    VCC: 5V
 *    GND: GND
 *    RX: PIN 10(Solo en MODO CONFIGURACION)
 *    TX: PIN 11(Solo en MODO CONFIGURACION)
 * 2) Entrar en MODO CONFIGURACION, para eso:   
 *    2.1) Desconectar VCC y el cable serial
 *    2.2) Mantener Apretado el boton del HC05 
 *    2.3) Volver a conectar VCC y el cable serial, siempre manteniendo apretado el boton del HC05 hasta que el led del HC05 no parpapadee discontinuamente por 1 seg
 *    2.4) Al parpadear asi se ingreso en MODO CONFIGURACION, entonces se puede cargar este sketch y configurarlo con los comandos AT
 * 3) Una vez realizada la configuracion con los comandos AT, se debe desconectar VCC y volver a conectarlo. Enttonces el led del HC05 parpaderara intermiteenmte
 * 4) Para entrar correctamente en MODO TRANSMICION y poder asi recibir datos desde Android, se deben intercambiar los pines RX y TX de la siguiente manera
 *    RX: PIN 10(Solo en MODO TRANSMICION)
 *    TX: PIN 11(Solo en MODO TRANSMICION)
*/

/*Comandos AT utilizados en la configuracion:
 * 
 * 1)AT+RESET
 *   AT+ORGL: borran la configuracion del HC05
 * 2)AT+NAME=ARDUINO_HC05
 * 3)AT+ROLE=0: Defino al HC05 como Esclavo
 * 4)AT+UART=9600,0,0: Defino la velocidad de tranferencia de datos entre el HC05 y Android a 9600 baudios
 */

#include <SoftwareSerial.h>
>>>>>>> Stashed changes

//Al utilizar la biblioteca SoftwareSerial los pines RX y TX para la transmicion serie de Bluethoot se pueden cambiar mapear a otros pines.
//Sino se utiliza esta bibioteca esto no se puede realizar y se debera conectar al pin 0 y 1, conexion Serie no pudiendo imprmir por el monitor serie
//Al estar estos ocupados.
SoftwareSerial BTserial(3,7); // RX | TX
char estado="X";
char c = ' ';
 
<<<<<<< Updated upstream

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
	//  SdPlay.setFile("swing.wav");
    //SdPlay.play();
    /*while(!SdPlay.isStopped()){
      analogWrite(PIN_VIBRADOR_DIGITAL,255);
      sensar();
    }*/
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
    Serial.println("hay movimiento");
    //SdPlay.setFile("quieto.wav");
    //SdPlay.play();
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
    estado=BTserial.read();
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
       /*  SdPlay.setFile("swing.wav");
         SdPlay.play();
         while(!SdPlay.isStopped()){
          sensar();
         }*/
         break;
       case '3':
         Serial.println("ACELERACION");
         /*SdPlay.setFile("on.wav");
         SdPlay.play();
         while(!SdPlay.isStopped()){
          sensar();
         }*/
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

=======
void setup()
{
    //Se configura la velocidad del puerto serie para poder imprimir en el puerto Serie
    Serial.begin(9600);
    Serial.println("Inicializando configuracion del HC-05...");

    //Se configura la velocidad que tendra el HC05 para recbir por Serie los comandos AT.
    //NOTA: Conviene siempre dejarlo por defecto a 38400
    BTserial.begin(9600); 
    
    //Se configura la velocidad de transferencia de datos entre el Bluethoot  HC05 y el de Android.
    //BTserial.begin(9600); 
    Serial.println("Esperando Comandos AT...");
}
 
void loop()
{
  //BTserial.println("Bluetooth Test");
  //Serial.println(BTserial.available());
  //Serial.println("--------");
   //sI reciben datos del HC05 
    if (BTserial.available())
    { 
        // Serial.println("primer if");
        //se los lee y se los muestra en el monitor serie
        c = BTserial.read();
        Serial.write(c);
    }

    //Si se ingresa datos por teclado en el monitor serie 
    if (Serial.available())
    {
        //Serial.println("segundo if");
        //se los lee y se los envia al HC05
        c =  Serial.read();
        Serial.write(c);
        BTserial.write(c); 
    }

   /* if(BTserial.available()){
          Serial.println("leyendoBLUETOOTH");
          Serial.println(BTserial.read());
          estado = (char)BTserial.read();
          Serial.println(estado);
          Serial.println("Esperando");
          delay(3000);
     }*/
     //delay(3000);
}
>>>>>>> Stashed changes
