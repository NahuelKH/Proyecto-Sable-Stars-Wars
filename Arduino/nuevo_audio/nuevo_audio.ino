#include <SD.h>                      // need to include the SD library
//#define SD_ChipSelectPin 53  //example uses hardware SS pin 53 on Mega2560
#define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328, can use other pins
#include <TMRpcm.h>           //  also need to include this library...

#include <SoftwareSerial.h>

SoftwareSerial BTserial(3,7); // RX | TX

TMRpcm tmrpcm;   // create an object for use in this sketch
char c = ' ';

void setup(){

  tmrpcm.speakerPin = 9; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc

  Serial.begin(9600);
   BTserial.begin(9600); 
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
    return;   // don't do anything more if not
  }else{
    Serial.println("todo ok");
    }
 // tmrpcm.play("on.wav"); //the sound file "music" will play each time the arduino powers up, or is reset
}



void loop(){  

  if(Serial.available()){    
    
    c =  Serial.read();
        Serial.write(c);
        BTserial.write(c);

        if(c == 'p'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("on.wav");
    }
  }

   if (BTserial.available())
    { 
        //se los lee y se los muestra en el monitor serie
        c = BTserial.read();
        Serial.write(c);
        if(c == 'p'){ //send the letter p over the serial monitor to start playback
      tmrpcm.play("on.wav");
    }
    }
//Serial.println("---------------------");
}
