# Semester Project Ether
_a lamp the reshape itself according to the current weather_

Blog :

installer requests sur windows :
	dans le terminal windows : python -m pip install requests

coo de neuchatel :
	{
    "id": 2659496,
    "name": "Neuchatel",
    "country": "CH",
    "coord": {
      "lon": 6.931,
      "lat": 46.991791
    }

openweathermap api key : aacbb81c3ddd5bd5176b4bc64424022d

requete pour neuch, temps actuel: 
	r = requests.get('http://api.openweathermap.org/data/2.5/weather?id=2659496&APPID=aacbb81c3ddd5bd5176b4bc64424022d')
 " 	...	", prévison :

r = requests.get('http://api.openweathermap.org/data/2.5/forecast?id=2659496&APPID=aacbb81c3ddd5bd5176b4bc64424022d')

extraire une partie (ex weather)
	import json
	t = json.loads(r.text)
	print(t['weather'])
	sortie : >>> [{'id': 741, 'main': 'Fog', 'description': 'fog', 'icon': '50d'}


extraire le 'Fog' :
	print(t["weather"][0]["main"])

INSTALLER DES DEPENDANCIES : <3 (pour requests notamment)

	https://github.com/relayr/python-sdk/blob/master/docs/manual/openwrt.txt

voir l'espace libre : commande : df

voir l'espace libre en ram : commande : free

sur openwrt, au début tapper cd .. pour rentrer dans le repository de base

unicode to string
	a.encode('ascii','ignore') voir : http://stackoverflow.com/questions/1207457/convert-a-unicode-string-to-a-string-in-python-containing-extra-symbols

concaténer dans un int : int(meteo)

Liste type de temps :
	http://openweathermap.org/weather-conditions

doc comm MPU et MCU mediatek
	https://docs.labs.mediatek.com/resource/linkit-smart-7688/en/tutorials/linkit-smart-7688-duo/mpu-mcu-uart-connection

Exécuter un script au boot :
	Aller dans /etc/inid.d, créer un fichier avec vim _nomFichier_ puis insérer ce contenu dedans :
		
		#!/bin/sh /etc/rc.common
		# Example script
		# Copyright (C) 2007 OpenWrt.org
		START=100
		STOP=15
		start() {        
			echo start
			cd ..
			python ether/Meteo.py
			# commands to launch application
		}                
		stop() {          
			echo stop
			# commands to kill application 
		}


Ou le START 100 est la priorité d exécution au démarrage. Il faut le mettre à 100 pour laisser le temps au wifi de démarrer.
Ensuite autoriser le fichier à s exécuter : chmod +x _nomFichier_ ensuite l ajouter à la liste des script à exécuter au boot : 		/etc/init.d/_nomFichier_ enable
Finalement alle dans le répertoire du fichier que va lancer le script et aussi faire : Meteo.py enable

Tutoriel complet : https://wiki.openwrt.org/doc/techref/initscripts

Interrupt sur l'atmega32u4 sur le linkit 7688 duo. Ce qui marche : déclarer le pin interrupt en 1 dans le code et le connecter l'électronique sur D1. Cela utilise le même pin que le Serial1. Les deux ne marchent pas en même temps. Déclarer le pin interrupt 3 dans le code et le connecter l'électronique sur D3 sur la linkit. Cela utilise le même interrupt ou timer que la librairie Servo.h de l'arduino. Des oscilliations sur ce pin entraîne des sacades sur le servo.
