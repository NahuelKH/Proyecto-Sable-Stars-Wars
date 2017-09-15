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

/**
* Método que inicializa/configura el arduino.
*
 */
void setup() {
  // put your setup code here, to run once:

}

/**
* Método en el que se programa la funcionalidad
*
 */
void loop() {

    if(hayMovimiento()){
      sonarSableMovimiento();
    }

}

/*
 * método que indicará si el sable de luz produjo un movimiento.
 */
boolean hayMovimiento(){
  return true;
}

/*
 * método que hara sonar el sable cuando se mueva.
 */
void sonarSable(){
  
}

