// Arduino Gassensor mit LCD, über WiFi ansteuerbar.
 //Mailo Mathis
 //
 
// include the library code
#include <LiquidCrystal.h>
#include <WiFiNINA.h>
#include <time.h>
#include <Servo.h>

//wifi client stuff
char ssid[] = "MathisModul242LB2";        // Ihr Netzwerk SSID (name)
char pass[] = "Password1";    // Ihr <Netzwerk Passwort (benützen Sie WPA, oder benutzen Sie WEP als Schlüssel)

int keyIndex = 0;                 // Ihr Netzwerk Schlüssel Index (Nur für wenn Sie WEP benutzen)
int status = WL_IDLE_STATUS;      // Verbindungsstatus

const int array_len = 100; //Anzalhl Zeitpunkte die gespeichert werden sollen
float times[array_len]; //Anzahl Messungen
float ppms[array_len]; // Messungen
int count = 0; // Nächster Messungsindex
float old_millis = 0; // Zeit der letzten Messung
float equilibrium_time = 10000; // Zeit zu Volt equilibrium. Der Sauerstoffsensor benutzt Rücksprecheschleifen bis die benötigte Spannung erreicht ist,
//sodass es Messungen optimieren kann wenn man die Spannung ein Equilibrium erreichen lässt.
float time_step = 60000 + equilibrium_time; // Zeit zwischen den Messungen
int measurement_cycle = 0; // Schaltet zwischen 0 und 1 hin und her, je nach dem ob ein Zeitabstand erreicht wurde.

WiFiServer server(80);            //Serversocket (Port)
IPAddress ip;
WiFiClient client = server.available();

/////////////////////
// LCD ist mit Arduino verbunden als: LiquidCrystal lcd(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(11, 12, 6, 7, 8, 9);

//Pulse Width Modulation pins um den Operationsverstärker opamp auf Basisspannung zu schalten


const int ic2_voltage = 3 ; //relativer Grundspannungs-Pin, etwa 2.9 V
int vground = 151;// Durchschnittliche Pulse Width Modulation PWM ist 2.9 V

const int ic1_voltage = 2 ; //Spannungs-Pin, etwa 3.5 V
int v_set_voltage = vground+31; //31 = 600 mv differenz
    
//Sensor-Pins für Operationsverstärker opamp Ausgänge
const int vout = A1 ;//Messungs-Pin

////////////////////

float r_gain = 10000;
float scaling_ppm = 4 * 1e-7 * r_gain;


void printWifiStatus() {
  // Gibt den SSID (Name) von dem Natzwerk aus, mit welchem Sie verbunden sind:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Gibt die IP Adresse deiner Platine aus:
  ip = WiFi.localIP();
  lcd.setCursor(0,0);
  lcd.print(ip);

  // Gibt die empfangene Signalstärke aus:
  long rssi = WiFi.RSSI();
  Serial.print("Signalstärke (RSSI-Received Signal Strength Indicator):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("Um diese Seite zu besuchen, können Sie ein Browserfenster öffnen (http://)");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Die Kommunikation mit dem WiFi Modul ist felgeschlagen!");
    // Nich fortfahren
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Bitte Firmware aktualisieren");
  }
}


void connect_WiFi() {
  // Versuch sich mit dem WiFi Netzwerk zu verbinden:
  int StatusWiFi=WiFi.status();
  Serial.println(StatusWiFi);
  if(StatusWiFi!=3||(WiFi.localIP()[0]==0))  // Wenn die Verbindung verworfen, gestört oder unterbrochen wird, versuchen Sie es erneut
  {
      WiFi.disconnect();
      lcd.setCursor(0,0);
      lcd.print("Suche nach WiFi");
      delay(1000);
      //WiFiServer server(80);
      Serial.print("Versuch sich mit dem Netzwerk (SSID) zu verbinden: ");
      Serial.println(ssid);
      status = WiFi.begin(ssid, pass);
      
      Serial.println(status);
  }
  Serial.println("Verbunden");

}

void printWEB() {

  if (client) {                             // wenn Sie ein Client bekommen,
    Serial.println("new client");           // geben Sie eine Nachricht zum Serial Port aus,
    while (client.connected()) {            // lassen Sie die Schleife laufen wärend der Client verbunden ist
      if (client.available()) {             // und wenn es Bytes vom Client zum lesen gibt, 
        char c = client.read();             // lesen Sie das Byte, dann
        Serial.write(c);                    // geben Sie sie auf den Monitor aus
        if (c == '\n') {                    // wenn das Byte ein "newline character" ist.

          // Wenn die momentane linie leer ist, haben Sie ein "newline characters" in einer Linie.
          // Das ist das Ende von dem Benutzer HTTP Anfarge, sende bitte ein Antwort:
          
            // HTTP headers fangen immer mit einem Antwortcode an (z.B. HTTP/1.1 200 OK)
            // und einem content-type, sodass der Nutzer weiss was kommt, dann kommt eine leere Linie:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // Kreiere Tabelle
            client.print("<html><body>");
            client.print("<table class=\"dataTable text-light dataTables-sidebar overflow-x-auto\" cellspacing=\"30\" cellpadding=\"0\"><thead></thead><tbody>");
            
            client.print("<tr><td>Zeit seit Beginn der Messung (in Sekunden)</td><td>Sauerstoff(ppm)</td></tr>");
            for(int ii=0;ii<array_len;ii++){
              int curr_index = (ii+count)%array_len;
                if(times[curr_index] != 0){
                  client.print("<tr><td>");
                  client.print(times[curr_index]);
                  client.print("</td><td>");
                  client.print(ppms[curr_index]);
                  client.print("</td></tr>");
              }
            
            }
            client.print("</table></tbody></body></html>");
            
            
            // Aus der While-Schleife ausbrechen:
            break;
          }
      }
    }
    // Schliesse die Verbindung:
    client.stop();
  }
}

float get_oxygen_voltage(){
    //Nehmen Sie die durchschnittliche Spannung von dem Sauerstoffsensor
    int num_iterations = 10;
    float counter_val = 0;
    float out_val = 0;
    
    for(int ii=0;ii<num_iterations;ii++){
      out_val += float(analogRead(vout)) * 5. / 1023.;
      //counter_val += float(analogRead(vcount));// * 5. / 1023.;
      delay(200);
      }
    out_val /= num_iterations;  
    ////////////////////////////////////////
    //Lassen Sie den Sauerstoffsensor ruhen
    ///////////////////////////////////////
    analogWrite(ic1_voltage,0) ;
    analogWrite(ic2_voltage,0) ;  
    return out_val;
}

void setup() {
  // Legen Sie die Nummer der LCD Zeielen und Spalten fest:
  lcd.begin(16, 2);
  // Setzen Sie den cursor auf Spalte 6, Linie 1
  lcd.setCursor(6, 1);
  // Geben Sie eine Nachricht an den LCD-Bildschirm aus.
  lcd.print("O2!");
  
  lcd.setCursor(2, 1);
  lcd.print("Ja, Bitte!");
  
  pinMode(ic1_voltage,OUTPUT) ;
  pinMode(ic2_voltage,OUTPUT) ;  

  Serial.begin(9600);
  WiFi.disconnect();
  enable_WiFi();
  connect_WiFi();
  server.begin();
  printWifiStatus();
}

void loop() {   
    if(millis() - old_millis > time_step){// Wenn gnügend Zeit seit dem letzten Messzyklus vergangen ist, führe eine neu Messung aus
      old_millis = millis(); // Setzt den old_millis Zeitstempel zurück
      analogWrite(ic1_voltage,v_set_voltage); //Schaltet den Sauerstoffsensor an
      analogWrite(ic2_voltage,vground);
      measurement_cycle = 1; // Messzyklus hat begonnen
    }
    if(measurement_cycle == 1)
    {
      if(millis() - old_millis > equilibrium_time)//Wenn die Spannung ausgeglichen ist, Messung starten.
      {
        float delta_v = get_oxygen_voltage();//Liesst die Messungsspannung aus
        analogWrite(ic1_voltage,0);//Schaltet den Sauerstoffsensor aus.
        analogWrite(ic2_voltage,0);
        measurement_cycle = 0;//Wechselt den Zustand auf Nicht-Messen
        ////////////////////////////////////////
        //Spannungsausgabe
        ///////////////////////////////////////
        lcd.clear();//Aktualisiere lcd
        lcd.setCursor(0,0);
        lcd.print(ip);
        lcd.setCursor(0,1);
        lcd.print("~");
        lcd.setCursor(1,1);
        lcd.print(delta_v/scaling_ppm);
        lcd.setCursor(12, 1);
        lcd.print("ppm");
        
        // Daten aufnehmen
        times[count%array_len] = millis()/1000.;
        ppms[count%array_len] = delta_v/scaling_ppm;
        count += 1;
        count %= array_len;
        
      }
    }

        
    connect_WiFi();
    printWifiStatus();

    client = server.available();// Kontrolliert ob jemand die Arduinoseite durchsucht
    if (client) {
      lcd.setCursor(0,0);
      lcd.print("Verbinde...    ");
      printWEB(); //Sendet eine Tabelle der Messungen wenn sich jemand verbindet.
      delay(5000);
      lcd.setCursor(0,0);
      lcd.print(ip);
    }
  delay(1000);  
}


  
