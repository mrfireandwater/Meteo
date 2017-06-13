/*
 * NAME           : etherMCU
 * PURPOSE        : combine LEDs, servo and weather data to modify the shape of the ether lamp.
 *                  Turn on/off by clapping hands
 * PROJECT        : "Ether", a lamp which shape and color changes upon the weather. Turn on/off by clapping hands
 * AUTHOR         : Louis Udry, Loïc Rochat
 * DATE           : 10 June 2017
 * CONTACT        : louis.udry@he-arc.ch loic.rochat@he-arc.ch
 * 
 * ACKNOWLEDGMENT : Gerald Huguenin and Florien Sauser, our semester teacher for providing their knowledge
 *                  during the project.
 * 
 */

#include <Adafruit_NeoPixel.h> //LED control library
#include <Servo.h> // Servo control library
/*
 * Weather conditions intervals : id received from the MPU (microprocessor unit) running the python script
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
#define pinLEDs 10 // pin attached to the data of the led strip
#define numLEDs 33 // number of led per galette

/*
 * Servo parameters
 */
#define pinServo 11 // pin the servo is attached to
#define closePos 180 // pos where galettes look like the sun
#define openPos 10 // pos where galettes look like clouds
Servo myservo; // declare the servo we are gonna use

/*
 * Micro parameters
 */
int t = 0; // to be use to store the time when hands'clapping
int flag = 0; // to know if this is the first or second clap. flag = 0 -> wait for first clap. flag = 1 -> wait for second clap
#define borneInf 200 // minimum milliseconds between to hands' clap
#define borneSup 400 // maximum milliseconds between to hands' clap
const byte interruptPin = 3; // output electronic microphone circut linked to linkit D3
int light = 0; // 1 = on. 0 = off

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLEDs*3, pinLEDs, NEO_GRB + NEO_KHZ800); // declare the led strip

/*
 * Variables used to store the data from the python script
 */
int data = 0; //read on the serial 1
int id = 0; // actual id of the weather 
int tab[2]; // purpose of buffer for "data"
int i = 0;


void setup() {
  Serial.begin(9600);  // open serial connection to USB Serial port (connected to your computer)
  Serial1.begin(57600);  // open internal serial connection to MT7688AN
                         // in MT7688AN, this maps to device
                         /*
                          * the timer of the Serial1 uses the same as interrupt 1 so they couldn't
                          * work together
                          */
  strip.begin();
  strip.setBrightness(150);

  myservo.attach(pinServo);

  pinMode(interruptPin,INPUT);
  pinMode(pinLEDs, OUTPUT);
  pinMode(pinServo, OUTPUT);
  //attachInterrupt(digitalPinToInterrupt(interruptPin),blink,CHANGE);
  
  demo(); //launch the demo
  }
 
void loop() {
  data = Serial1.read();      // read from MT7688AN | data = -1 if nothing received
  checkData(data);
}

/*
 * FUNCTION : demo of the differents weather states and color
 * INPUT :    nothing
 * OUTPUT :   nothing
 * OTHER : called once on startup
 */
void demo(){
  light = 1; // tell the light is on.
  //sunup with clouds
  Serial.println("sunup with clouds");
  ServoPos(openPos);
  SetColor(400, 255, 150, 0, 0); // clear yellow (morning)
  SetColor(400, 255, 255, 255, 1); // white
  SetColor(400, 255, 255, 255, 2); // white
  delay(2000);

  //sun
  Serial.println("sun");
  ServoPos(closePos);
  SetColor(400, 255, 150, 0, 0); // clear yellow (morning)
  SetColor(400, 255, 150, 0, 1); // clear yellow (morning)
  SetColor(400, 255, 150, 0, 2); // clear yellow (morning)
  delay(2000);

  // sun + clouds
  Serial.println("sun + clouds");
  ServoPos(openPos);
  SetColor(400, 255, 100, 0, 0); // yellow
  SetColor(400, 255, 255, 150, 1); // blue white
  SetColor(400, 255, 255, 150, 2); // blue white
  delay(2000);

  // brocken clouds
  Serial.println("brocken clouds");
  ServoPos(openPos);
  SetColor(400, 255, 255, 255, 0); // bluewhite
  SetColor(400, 255, 255, 150, 1); // blue white
  SetColor(400, 255, 255, 150, 2); // blue white
  delay(2000);

  //sunset
  Serial.println("sunset");
  ServoPos(openPos); //clouds
  SetColor(400, 255, 40, 90, 0); // pink
  SetColor(400, 255, 194, 240, 1); // white-pink
  SetColor(400, 255, 194, 240, 2); // white-pink
  delay(2000);
  
}

/*
 * FUNCTION : Used to detect hands clapping to turn on/off the lamp
 * INPUT :    nothing
 * OUTPUT :   nothing
 * OTHER : called by the interrupt
 */
void ClapClap(){
  Serial.println("clap");
  if (flag == 0){ // if waiting for the first clap
    //first clap
    t = millis(); //remember the time of the first clap
    flag = 1; //know we set that we will wait for the second clap (flag = 1)
    detachInterrupt(digitalPinToInterrupt(interruptPin)); // detach the interrupt so it isn't called by the noise generated by the first hands clapping
    //Serial.println("detach interrupt");
    delay(borneInf); //minimum time between two claps
    attachInterrupt(digitalPinToInterrupt(interruptPin),blink,CHANGE);
    //Serial.println("attach interrupt");     
  }
  else if (flag == 1 && ((millis()-t) <= borneSup) && ((millis()-t) > borneInf)){ //waiting for the second clap
    //second clap
    Serial.println(millis()-t);
    Serial.println("hands clapping");
    LightOnOff();
    flag = 0;
  }
  else if (flag == 1 && ((millis()-t) <= borneSup)){ //if longer than borneSup, there isn't a second clap
    flag = 1;
  }
  else{
    flag = 0;
  }
}

/*
 * FUNCTION : turn on or off the lamp after hand clapping
 * INPUT :    nothing
 * OUTPUT :   nothing
 * OTHER : called by ClapClap()
 */
void LightOnOff(){
  if(light == 1){
    SetColor(200, 0, 0, 0, 0); // red = not implemented
    SetColor(200, 0, 0, 0, 1); // red = not implemented
    SetColor(200, 0, 0, 0, 2); // red = not implemented
    light = 0;
  }
  else if(light == 0){
    SetColor(200, 255, 255, 255, 0); // red = not implemented
    SetColor(200, 255, 255, 255, 1); // red = not implemented
    SetColor(200, 255, 255, 255, 2); // red = not implemented
    light = 1;
  }
}

/*
 * FUNCTION : Interrupt connected to output of the electronic microphone circut
 * INPUT :    digital output of op amp.
 * OUTPUT :   nothing
 * OTHER : call the ClapClap function
 */
void blink(){
  ClapClap();
}

/*
 * FUNCTION : Manage the position of the Servo.
 * INPUT :    the servo position we want to go to.
 * OUTPUT :   nothing
 */
void ServoPos(int nextPos){
   myservo.attach(pinServo);
  int currentPos = myservo.read(); // get position of the servo
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
  myservo.detach();
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
      strip.setPixelColor(i, r, g, b); //change color composant of the LED number i
      strip.show(); //update the led strip to the color we just set up
      delay(timing/numLEDs);
    }
  //Serial.println(numGalette); //debug
  return;
}

/*
 * FUNCTION : Check for weather data on serial. store the data in an integer "meteo". Call the translate methode
 * INPUT :    raw data from serial
 * OUTPUT :   nothing
 */
void checkData(int datalocal){
  delay(50); // keep that please
  if(datalocal == 255){ //where 255 indicates the python script will send weather data
      for(int i = 0 ; i<2 ; i++){
        tab[i] = Serial1.read();  //read the serial and store
        //delay(50);
        //Serial.println(tab[i]);
      }
      int meteo = 256*tab[0] + tab[1]; // because is was % 256 by the python script. Serial 1 only accept int between 0 and 255
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

