//Constantes para pines de sensores
#define PIN_SENSOR_LDR_ANALOGICO 3
#define PIN_SCL_6050_ANALOGICO 5
#define PIN_SDA_6050_ANALOGICO 4
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
/**
  Método que inicializa/configura el arduino.
*/
void setup() {
  // Seteo de pines ANALOGICOS
  //pinMode(PIN_SENSOR_LDR_ANALOGICO, INPUT);

  Serial.begin(9600);
  
  pinMode(PIN_SCL_6050_ANALOGICO, INPUT);
  pinMode(PIN_SDA_6050_ANALOGICO, INPUT);
  /*
  // Seteo de pines DIGITALES
  pinMode(PIN_REED_DIGITAL, INPUT);
  
  pinMode(PIN_SCK_TARJETA_SD_DIGITAL, OUTPUT);
  pinMode(PIN_MISO_TARJETA_SD_DIGITAL, OUTPUT);
  pinMode(PIN_MOSI_TARJETA_SD_DIGITAL, OUTPUT);
  pinMode(PIN_CS_TARJETA_SD_DIGITAL, OUTPUT);
  pinMode(PIN_PARLANTE_DIGITAL, OUTPUT);
  pinMode(PIN_VIBRADOR_DIGITAL, OUTPUT);

  setColor(DEFAULT_COLOR, DEFAULT_COLOR, DEFAULT_COLOR);
  */
}

/**
  Método en el que se programa la funcionalidad
*/
void loop() {
  Serial.print(analogRead(3));
  Serial.println();
  delay(2000);
 /* if(reed){
    // Luz
    intensidad = sensarLuzExterna();
    ajustarIntensidad();


    // 
    
  }
  
  if (hayMovimiento()) {
    sonarSableMovimiento();
  }

  setColor(0, 0, 255);  // Azul
*/
}

 
/*
   método que indicará si el sable de luz produjo un movimiento.
   @author: Pablo
*/
boolean hayMovimiento() {
  return true;
}

/*
   método que hara sonar el sable cuando se mueva.
   @author: Pablo
*/
void sonarSable() {

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
