/*
 * NAME           : etherMCU
 * PURPOSE        : combine LEDs, servo and weather data to modify the shape of the ether lamp.
 *                  Turn on/off by clapping hands
 * PROJECT        : "Ether", a lamp which shape and color changes upon the weather. Turn on/off by clapping hands
 * AUTHOR         : Louis Udry, Loïc Rochat
 * DATE           : 10 June 2017
 * CONTACT        : louis.udry@he-arc.ch loic.rochat@he-arc.ch
 * 
 * ACKNOWLEDGMENT : Gerald Hugenin and Florien Sauser, our semester teacher for providing their knowledge
 *                  during the project.
 * 
 */

#include <Adafruit_NeoPixel.h>
#include <Servo.h>
/*
 * Weather conditions :
 */
#define thunderstorm_down 200
#define thunderstorm_up 232
#define drizzle_down 300
#define drizzle_up 321
#define rain_down 500
#define rain_up 531
#define snow_down 600
#define snow_up 622
#define atmosphere_down 700
#define atmosphere_up 781
#define clear_cst 800
#define fewClouds_down 801
#define fewClouds_up 802
#define brockenClouds_down 803
#define brockenClouds_up 804
#define extreme_down 900
#define extreme_up 906
#define additionnal_down 951
#define additionnal_up 962

/*
 * LEDs parameters :
 */
#define pinLEDs 10 // vérifier si c'est pwm
#define numLEDs 33 // Nombre de LEDs par galette. Remarque : compter le nombre correcte de LEDs

/*
 * Servo parameters
 */
#define pinServo 11
#define closePos 180
#define openPos 60
Servo myservo;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLEDs*3, pinLEDs, NEO_GRB + NEO_KHZ800);
int data = 0;
int id = 0;
int tab[2];
int i = 0;

void setup() {
    Serial.begin(115200);  // open serial connection to USB Serial port (connected to your computer)
    Serial1.begin(57600);  // open internal serial connection to MT7688AN
                           // in MT7688AN, this maps to device
    strip.begin();
    strip.setBrightness(50);

    myservo.attach(11);
    }
 
void loop() {
    data = Serial1.read();      // read from MT7688AN | data = -1 if nothing received
    checkData(data);
}

/*
 * FUNCTION : Manage the position of the Servo.
 * INPUT :    the servo position we want to go to.
 * OUTPUT :   nothing
 */
void ServoPos(int nextPos){
  int currentPos = myservo.read();
  if(nextPos == currentPos){
    // do nothing
  }
  else if (currentPos < nextPos){
    for (currentPos; currentPos <= nextPos; currentPos++) { 
      myservo.write(currentPos);
      delay(40);  
    }
  }
  else if (currentPos > nextPos){
    for (currentPos; currentPos >= nextPos; currentPos--) { 
      myservo.write(currentPos);
      delay(40);
    }
  }
}

/*
 * FUNCTION : Put all LEDs to a specific color 
 * INPUT :    timing for the time all the led lights up (one after another),
 *            r for red, g for green, b for blue composite, numGalette correspond to the galette 
 *            to power on. 0 means galette 1, 1 means galette 2, 2 means galette 3.
 * OUTPUT :   nothing
 */
void SetColor(int timing, int r, int g, int b, int numGalette)
{
  for(i = (numGalette*numLEDs); i <= ((numGalette + 1) * numLEDs) ; i++)
    {
      strip.setPixelColor(i, r, g, b);
      strip.show();
      delay(timing/numLEDs);
    }
  Serial.println(numGalette);
  return;
}

/*
 * FUNCTION : Check for weather data on serial. store the data in an integer "meteo". Call the translate methode
 * INPUT :    raw data from serial
 * OUTPUT :   nothing
 */
void checkData(int datalocal){
  delay(50);
  if(datalocal == 255){
      for(int i = 0 ; i<2 ; i++){
        tab[i] = Serial1.read(); 
        //delay(50);
        Serial.println(tab[i]);
      }
      int meteo = 256*tab[0] + tab[1];
      translator(meteo);
    }else{}
}

/*
 * FUNCTION : translate the meteo id into action like turning on/off LEDs according to the weather
 * INPUT :    meteo (int)
 * OUTPUT :   nothing
 */
int translator(int id){

    if(id >= thunderstorm_down && id <= thunderstorm_up){
      Serial.println("Thunderstorm");
      ServoPos(openPos);
      SetColor(400, 255, 0, 0, 0); // red = not implemented
      SetColor(400, 255, 0, 0, 1); // red = not implemented
      SetColor(400, 255, 0, 0, 2); // red = not implemented
    }
    else if(id >= drizzle_down && id <= drizzle_up){
      Serial.println("Drizzle");
      ServoPos(openPos);
      SetColor(400, 255, 0, 0, 0); // red = not implemented
      SetColor(400, 255, 0, 0, 1); // red = not implemented
      SetColor(400, 255, 0, 0, 2); // red = not implemented
    }
    else if(id >= rain_down && id <= rain_up){
      Serial.println("Rain");
      ServoPos(openPos);
      SetColor(400, 255, 0, 0, 0); // red = not implemented
      SetColor(400, 255, 0, 0, 1); // red = not implemented
      SetColor(400, 255, 0, 0, 2); // red = not implemented
    }
    else if(id >= snow_down && id <= snow_up){
      Serial.println("Snow");
      ServoPos(openPos);
      SetColor(400, 255, 0, 0, 0); // red = not implemented
      SetColor(400, 255, 0, 0, 1); // red = not implemented
      SetColor(400, 255, 0, 0, 2); // red = not implemented
    }
    else if(id >= atmosphere_down && id <= atmosphere_up){
      Serial.println("Fog probably");
      ServoPos(closePos);
      SetColor(400, 255, 100, 0, 0); // blue white
      SetColor(400, 255, 255, 150, 1); // blue white
      SetColor(400, 255, 255, 150, 2); // blue white
    }
    else if(id == clear_cst){
      Serial.println("Clear :-)");
      ServoPos(closePos);
      SetColor(400, 255, 100, 0, 0); // yellow
      SetColor(400, 255, 100, 0, 1); // yellow
      SetColor(400, 255, 100, 0, 2); // yellow
    }
    else if(id >= fewClouds_down && id <= fewClouds_up){
      Serial.println("Few clouds");
      ServoPos(openPos);
      SetColor(400, 255, 100, 0, 0); // blue white
      SetColor(400, 255, 255, 150, 1); // blue white
      SetColor(400, 255, 255, 150, 2); // blue white
    }
    else if(id >= brockenClouds_down && id <= brockenClouds_up){
      Serial.println("Brocken clouds");
      ServoPos(openPos);
      SetColor(400, 255, 255, 255, 0); // bluewhite
      SetColor(400, 255, 255, 150, 1); // blue white
      SetColor(400, 255, 255, 150, 2); // blue white
    }
    else if(id >= extreme_down && id <= extreme_up){
      Serial.println("Brocken clouds");
      ServoPos(openPos);
      SetColor(400, 255, 255, 255, 0); // bluewhite
      SetColor(400, 255, 255, 150, 1); // blue white
      SetColor(400, 255, 255, 150, 2); // blue white
    }
    else if(id >= additionnal_down && id <= additionnal_up){
      Serial.println("Brocken clouds");
      ServoPos(openPos);
      SetColor(400, 255, 255, 255, 0); // bluewhite
      SetColor(400, 255, 255, 150, 1); // blue white
      SetColor(400, 255, 255, 150, 2); // blue white
    }
    else{
      Serial.print("autre, id = : ");
      Serial.println(id);
      ServoPos(openPos);
      SetColor(400, 0, 255, 0, 0); // green = other
      SetColor(400, 0, 255, 0, 1); // green = other
      SetColor(400, 0, 255, 0, 2); // green = other
    }
}

