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

//Al utilizar la biblioteca SoftwareSerial los pines RX y TX para la transmicion serie de Bluethoot se pueden cambiar mapear a otros pines.
//Sino se utiliza esta bibioteca esto no se puede realizar y se debera conectar al pin 0 y 1, conexion Serie no pudiendo imprmir por el monitor serie
//Al estar estos ocupados.
SoftwareSerial BTserial(3,7); // RX | TX

char c = ' ';
 
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
   //sI reciben datos del HC05 
    if (BTserial.available())
    { 
      Serial.println("entramos");
        //se los lee y se los muestra en el monitor serie
        c = BTserial.read();
        Serial.println("leimos");
        Serial.write(c);
        Serial.println("escribimos");
    }

    //Si se ingresa datos por teclado en el monitor serie 
    if (Serial.available())
    {
        //se los lee y se los envia al HC05
        c =  Serial.read();
        Serial.write(c);
        BTserial.write(c); 
    }
}
