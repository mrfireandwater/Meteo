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
    s.write('int(meteo)') #send to the arduino through an integer
    print int(meteo) #debug
    time.sleep(1)

def checkLoop():
    if (time.time() % 5) == 0 :
        loop();
    else :
        time.sleep(1)

if __name__ == '__main__':
    setup()
    r = requests.get('http://api.openweathermap.org/data/2.5/weather?id=2659496&APPID=aacbb81c3ddd5bd5176b4bc64424022d') # results in json format
    t = json.loads(r.text) # the results are concateneted in a json format python understand
    meteo = t['weather'][0]['id'] # real data used. meteo is a unicode type
    s.write('int(meteo)') #send to the arduino through an integer
    print int(meteo) #debug where id correspond to clouds, clear sky, fog... see http://openweathermap.org/weather-conditions 
    while True:
        checkLoop()
        loop()