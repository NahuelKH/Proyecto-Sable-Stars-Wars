#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <TMRpcm.h>

#define DEBUGG_MODE 0

MPU6050 mpu;

TMRpcm tmrpcm;
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

#define SD_ChipSelectPin 4

// Supuestamente se necesita esto para haer andar el LED RGB Anodo
#define COMMON_ANODE

#define DEFAULT_COLOR 0
#define SD_ChipSelectPin 4
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
   tmrpcm.speakerPin = 9;
   if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
    return;   // don't do anything more if not
  }else{
    Serial.println("todo okkkkkkkkkk");
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
   Serial.println("****************************");
   Serial.println(encendido);
   Serial.println("****************************");
   Serial.println("****************************");
  if(encendido){
    azul=255;
    rojo=255;
    setearColor(azul,rojo); 
    tmrpcm.play("on.wav");
    while(tmrpcm.isPlaying()){ 
      analogWrite(PIN_VIBRADOR_DIGITAL,255);
    }
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
    while(encendido){
          sensarMovimiento();
          sensar();
      Serial.println("****************************");
      Serial.println(encendido);
      Serial.println("****************************");
          if(!tmrpcm.isPlaying()){
             tmrpcm.play("quieto.wav");
          }    
     }
    tmrpcm.play("off.wav");
    
    while(tmrpcm.isPlaying()){
        analogWrite(PIN_VIBRADOR_DIGITAL,255);
    }
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
    
    tmrpcm.play("swing.wav");
    while(!tmrpcm.isPlaying()){
      analogWrite(PIN_VIBRADOR_DIGITAL,255);
      sensar();
    }
    analogWrite(PIN_VIBRADOR_DIGITAL,0);
    Serial.println("hay movimiento");
    
    tmrpcm.play("quieto.wav");
  }else{
    //Serial.println("*******************************************************neutro");
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
          azul=20;
          rojo=20;
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
         tmrpcm.play("swing.wav");
         while(tmrpcm.isPlaying()){
          sensar();
         }
         break;
       case '3':
         Serial.println("ACELERACION");
         tmrpcm.play("on.wav");
         while(tmrpcm.isPlaying()){
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

