# Projekt Sauerstoffsensor mit LC-Display und Servomotor

## Use-Case

- Inhaltsverzeichnis
- Definition	
- Beschreibung	
- Teile	
- In-Funktions-setzung	
- Use-Cases	
- Blockschaltbild	
- Dokumentation	
- Einführung und Vorbereitung	
- Implementierung	
- Gassensor mit LCD	
- BME 680	
- HD-1440A	
- Testergebnisse	
- Ping-Test Ergebnisse	
- BME 680 Sauerstoffsensor Testergebnisse	
- HD-1440A Servomotor Testergebnisse	
- Fazit	
	

## Definition

### Beschreibung

Das Projekt beinhaltet einen Sauerstoff Sensor, der den momentanen Sauerstoffgehalt der Luft misst und auf ein LC-Display wiedergibt. Der Sauerstoffsensor wird über ein LCD auf dem MKR 1010 Arduino-Chip angesteuert, respektive deren Messung angezeigt. Bei Überschreitung eines vordefinierten Wertes wird ein Fensterhebel betätigt.

### Teile

	•	1 MKR 1010 Arduino-Chip
	•	1 LC-Display Grove-LCD RGB Backlight
	•	1 BME680 Sauerstoffsensor
	•	1 Servomotor: HD-1440A)
	
In der Arduino IDE wird das Modul MKR 1010 ausgewählt
Der O2-Sensor ist für normale Luft nicht genau. Die Empfindlichkeit wird durch den RGain-Widerstand bestimmt. Dieser Sensor kann jedoch erkennen, wenn er sich in einem mit Sauerstoff gefüllten Raum befindet.

### In-Funktions-setzung

Sobald der Chip eingesteckt ist und das Programm auf den Arduino-Chip hochgeladen wurde, wird eine IP-Adresse angezeigt. Das Programm benötigt Verbindungen zu Gas-Sensor, LC-Display und Servomotor. 

### Use-Cases

	Name:	                      - Display
	Akteur:	                      - Prüfer
	Trigger:	              - Display
	Kurzbeschreibung:	      - Sauerstoff Abfrage
	Vorbedingungen:	              - Display muss eingeschaltet sein und den Chip über ein Modul ansteuerbar.

	
	Komponenten:	              - LCD-Anzeige, Schaltplatine und Sauerstoffsensor. 
	Essenzielle Schritte:	      - 1. Intention der Systemumgebung	Reaktion des Systems
		                      - 2. LCD-Anzeige einschalten.	Aufleuchten des Bildschirms
	        	              - 3. Anwählen des Moduls	Auswahl Modul
	                	      - 4. Anzeigen des Sauerstoffgehalts	Sauerstoffgehalt wird abgefragt.
                              
	Ausnahmefälle:	      	      - Fehler Module.	
	Nachbedingung:	              - Keine	
	Zeitverhalten:	              - Instantan	
	Verfügbarkeit:	              - Immer (Bei Stromanschluss)	
	Fragen, Kommentare:           - Keine	

------------------------------------------------------------------------------------------------------------------------------



	Name	                      	- Fensteröffnung
	Akteur	                      	- System
	Trigger	                      	- Sauerstoffgehalt übersteigt vordefinierten Wert
	Kurzbeschreibung	      	- Fensterhebel Öffnung
	Vorbedingungen	              	- Display muss eingeschaltet sein und Fensterhebel über Chipmodul ansteuerbar.

	
	Komponenten	              	- LCD-Anzeige, Schaltplatine, Fensterhebel und Sauerstoffsensor. 
	Essenzielle Schritte	      	- 1. Intention der Systemumgebung	Reaktion des Systems
	                            	- 2. LCD-Anzeige einschalten.	Aufleuchten des Bildschirms
                              		- 3. Anzeigen des Sauerstoffgehalts	Sauerstoffgehalt wird abgefragt.
	                            	- 4. Sauerstoffgehalt übersteigt vordefinierten Wert 	Fensterhebel wird betätigt.
                              
	Ausnahmefälle	                - Fehler oder Blockade des Hebels.	
	Nachbedingung	                - Keine	
	Zeitverhalten	                - Instantan	
	Verfügbarkeit	                - Beim Systemzustand über vorgegebenen Wert (Bei Stromanschluss)	
	Fragen, Kommentare	       	- Keine	

 
![grafik](https://user-images.githubusercontent.com/56382532/174975333-3866270e-68a5-4081-972b-1ad78c062ffe.png)

















### Blockschaltbild

-	Sauerstoffsensor	BME 680
-	Servomotor 	      	HD-1440A
-	LC-Display	      	Grove-LCD RGB Backlight
-	WiFi	            	Arduino MKR1010 WiFi Modul 

![grafik](https://user-images.githubusercontent.com/56382532/174975446-ed788f3e-5546-4ec7-97d9-2371f0c9a3f3.png)


 

Ich habe folgende Schaltungsvorlage verwendet, um mich zu orientieren: 
https://www.sgxsensortech.com/content/uploads/2014/08/A1A-EC_SENSORS_AN2-Design-of-Electronics-for-EC-Sensors-V4.pdf



## Dokumentation

### Einführung und Vorbereitung
Die Applikation beinhaltet verschiedenste Schnittstellen, von Gassensor, LC-Display und ein Servomotor die miteinander synchronisiert werden müssen. Der Servomotor sollte angesteuert werden, wenn der Gassensor einen kritischen Wert verzeichnet, welcher ein Fensteröffnen imitieren soll. 

### Implementierung
Bei der Implementierung habe ich folgende Ressource verwendet, um die Architektur zu verstehen:
https://www.sgxsensortech.com/content/uploads/2014/08/A1A-EC_SENSORS_AN2-Design-of-Electronics-for-EC-Sensors-V4.pdf

#### Gassensor mit LCD
   
![grafik](https://user-images.githubusercontent.com/56382532/174975748-4a1d2dd5-eaaf-4a6a-a4ac-153c8b45ef38.png)
![grafik](https://user-images.githubusercontent.com/56382532/174975780-9f29db19-23e1-479e-a553-492642299d6e.png)
![grafik](https://user-images.githubusercontent.com/56382532/174975827-303d2ddf-b65b-4749-ae37-01f9845a4ee5.png)


#### BME 680
Der Port werden mittels der I2 C Schnittstelle angesteuert, um Signalkonflikte zu verhindern.

#### HD-1440A

### Servomotor Programmcode

	Servo myservo;  							// Servo-Objekt erstellen, um ein Servomotor zu steuern

	int pos = 0;    							// Variable zur Speicherung der Servoposition

	void setup_servo() {
 	 myservo.attach(3);  							// Befestigt das Servo an Pin 3 am Servoobjekt
	}

	void loop_servo (){
 	 if (ppms[array_len] > 500) {
 	 for (pos = 0; pos <= 180; pos += 1) { 					//geht von 0 Grad auf 180 Grad (in 1 Grad Schritte)
   	 myservo.write(pos);              					// Servomotor in der variablen 'POS' positioniert
    	delay(15);                       					// wartet 15 ms, bis das Servo die Position erreicht hat
 	 }
  	for (pos = 180; pos >= 0; pos -= 1) { geht von 180 Grad auf 0 Grad
   	 myservo.write(pos);              					// Servomotor in der variablen 'POS' positioniert
 	   delay(15);                       					// wartet 15 ms, bis das Servo die Position erreicht hat
 	 }
	}
	  }
	}


### Testergebnisse

#### Ping-Test Ergebnisse

Android Phone:
15:24:12.766 -> BSSID: 90:17:C8:10:28:A6
15:24:12.766 -> signal strength (RSSI): -39
15:24:12.766 -> Encryption Type: 4
15:24:12.766 -> IP address: 192.168.43.250
15:24:12.766 -> Subnet mask: 255.255.255.0
15:24:12.766 -> Gateway IP: 192.168.43.1
15:24:12.766 -> MAC address: 84:0D:8E:34:C1:20

#### BME 680 Sauerstoffsensor Testergebnisse

15:31:17.306 -> Temperature = 26.94 *C
15:31:17.306 -> Pressure = 967.73 hPa
15:31:17.306 -> Humidity = 25.98 %
15:31:17.306 -> Gas = 114.96 KOhms
15:31:17.306 -> Approx. Altitude = 385.98 m

#### HD-1440A Servomotor Testergebnisse

Motor defekt, das Ansteuern macht sich durch einen Summton bemerkbar.

### Fazit

Die Kombination von LC-Display, Servomotor und Sauerstoffsensor hat sich gut ergänzt. Die ersten Tests waren vielversprechend und dementsprechend habe ich das Projekt weiterverfolgt.
