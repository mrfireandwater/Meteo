import requests
import json
import time
import serial

s = None


def setup():
    # open serial COM port to /dev/ttyS0, which maps to UART0(D0/D1)
    # the baudrate is set to 57600 and should be the same as the one
    # specified in the Arduino sketch uploaded to ATMega32U4.
    global s
    s = serial.Serial("/dev/ttyS0", 57600)

def loop():
    # send meteo to the Arduino sketch on ATMega32U4.
    # the sketch will turn on the lamp according to the meteo data
    r = requests.get('http://api.openweathermap.org/data/2.5/weather?id=2659496&APPID=aacbb81c3ddd5bd5176b4bc64424022d') # results in json format
    t = json.loads(r.text) # the results are concateneted in a json format python understand
    print t['weather'] #debug
    meteo = t['weather'][0]['id'] # real data used
    meteoInt = int(meteo)

    # write can only handle int between 0 to 255. Let's split our id :
    centaine = meteoInt/256
    dizaine = meteoInt % 256

    s.write([255]) # tell arduino an id will be sent
    s.write([centaine]) #send to the arduino through an integer
    s.write([dizaine])

    print int(meteo) #debug
    time.sleep(1)

def checkLoop():
    if (time.time() % 5) == 0 :
        loop();
    else :
        time.sleep(1)

if __name__ == '__main__':
    setup()
    while True:
        checkLoop()
        loop()