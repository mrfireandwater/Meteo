#define thunderstorm_down 200
#define thunderstorm_up 232
#define drizzle_down 300
#define drizzle_up 321
#define clear_cst 800

void setup() {
    Serial.begin(115200);  // open serial connection to USB Serial port (connected to your computer)
    Serial1.begin(57600);  // open internal serial connection to MT7688AN
                           // in MT7688AN, this maps to device
    pinMode(13, OUTPUT);
    }
 
void loop() {
    int c = Serial1.read();      // read from MT7688AN
    Serial.println(c);
    delay(50);
    if (c != -1) {
        if(c >= thunderstorm_down && c <= thunderstorm_up){
        Serial.println("Thunderstorm");
        }
        else if(c >= drizzle_down && c <= drizzle_up){
        Serial.println("Drizzle");
        }
        else if(c == clear_cst){
        Serial.println("Clear :-)");
        }
        else{
          Serial.println("autre");
        }
    }else{}
}
