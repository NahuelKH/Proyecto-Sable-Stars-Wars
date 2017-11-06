#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <SimpleSDAudio.h>

#define DEBUGG_MODE 0

MPU6050 mpu;

//TMRpcm tmrpcm;
SoftwareSerial BTserial(3,7); // RX | TX

int16_t ax, ay, az;
double normal;
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

#define DEFAULT_COLOR 0
#define SD_ChipSelectPin 4
//umbrales
#define UMBRAL_ACELEROMETRO 25000
#define UMBRAL_LUZ_AMBIENTAL 100
#define UMBRAL_LUZ_BRILLO 120
#define UMBRAL_LUZ_OSCURIDAD 90
//contadores de sonido
int luz=0;
boolean encendido=false;
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
    if (SdPlay.init(SSDA_MODE_FULLRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER)){
    Serial.println("todo okkkkkkkkkk");
  }else{
     Serial.println("SD fail");  
  }

   
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
//delay(5000);

  Serial.print("ready");
}

/**
  Método en el que se programa la funcionalidad
*/
void loop() {
   analogWrite(PIN_VIBRADOR_DIGITAL,0);
   encendido = reedEncendido();
    sensarMovimiento();
      //SdPlay.setFile("on.wav"); 
  //SdPlay.play();
 while(!SdPlay.isStopped()) {Serial.println("jojojo");}
  if(encendido){
    azul=0;
    rojo=255;
    setearColor(azul,rojo); 
   /* tmrpcm.play("on.wav");
    while(tmrpcm.isPlaying()){ 
      analogWrite(PIN_VIBRADOR_DIGITAL,255);
    }*/
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
    while(encendido){
          sensarMovimiento();
          sensar();
     }
   /* tmrpcm.play("off.wav");
    
    while(tmrpcm.isPlaying()){
        analogWrite(PIN_VIBRADOR_DIGITAL,255);
    }*/
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
    if(luz>220){
        if(azul>0){
          azul=255;
        }
        if(rojo>0){
          rojo=255;
        }
    }else{
        if(azul>0){
          azul=50;
        }
        if(rojo>0){
          rojo=50;
        }
    }
     setearColor(azul,rojo);
}
 /*
 * obtiene el valor sensado por el acelerometro
 */
void sensarMovimiento(){
  //mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  mpu.getAcceleration(&ax, &ay, &az);

  normal=sqrt(pow(ax,2)+pow(ay,2)+pow(az,2));
  if(normal>UMBRAL_ACELEROMETRO){
    Serial.println(normal);
    //tmrpcm.play("on.wav");  
if(SdPlay.setFile("on.wav")){
    Serial.println("encontrado");
  }else{
    Serial.println("fail");
    }
  SdPlay.play();
 while(!SdPlay.isStopped()) {
   Serial.println("que hace");
 }  
  }
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
        if(color=='A'){
          azul=20;
          rojo=0;
        }else if(color=='R'){
          azul=0;
          rojo=20;
        }else{
          azul=20;
          rojo=20;
        }
        setearColor(azul,rojo);
      break;
      case '1':
        Serial.println("FUERTE");
       if(color=='A'){
          azul=255;
          rojo=0;
        }else if(color=='R'){
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
        /* tmrpcm.play("Saberblk.wav");
         while(tmrpcm.isPlaying()){
      sensar();
         }*/
         break;
       case '3':
         Serial.println("ACELERACION");
        /* tmrpcm.play("swing.wav");
         while(tmrpcm.isPlaying()){
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
    //leerBluetooth();      
    sensarLuz();
    encendido=reedEncendido();
}

