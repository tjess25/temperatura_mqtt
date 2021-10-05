

#include "DHT.h"
//Bibliotecas
#include <WiFi.h>  // Biblioteca para el control de WiFi
#include <PubSubClient.h> //Biblioteca para conexion MQTT

#define DHTPIN 12   

#define DHTTYPE DHT11   

//Datos de WiFi
const char* ssid = "pinchos";  // Aquí debes poner el nombre de tu red
const char* password = "01Sricrse00SraD";  // Aquí debes poner la contraseña de tu red
const char* pubTopic = "t/oficina";

 
DHT dht(DHTPIN, DHTTYPE);

//Datos del broker MQTT
const char* mqtt_server = "192.168.0.100"; // Si estas en una red local, coloca la IP asignada, en caso contrario, coloca la IP publica
IPAddress server(192,168,0,100);

// Objetos
WiFiClient espClient; // Este objeto maneja los datos de conexion WiFi
PubSubClient client(espClient); // Este objeto maneja los datos de conexion al broker

// Variables
unsigned long timeLast; // Variables de control de tiempo no bloqueante
int wait = 5000;  // Indica la espera cada 5 segundos para envío de mensajes MQTT

void setup() {
  Serial.begin(115200);
  Serial.println(F("Prueba de coenxion"));

  dht.begin();

   Serial.println();
  Serial.println();
  Serial.print("Conectar a ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password); // Esta es la función que realiz la conexión a WiFi
 
  while (WiFi.status() != WL_CONNECTED) { // Este bucle espera a que se realice la conexión    digitalWrite (statusLedPin, HIGH);
    delay(500); //dado que es de suma importancia esperar a la conexión, debe usarse espera bloqueante
    Serial.print(".");  // Indicador de progreso
    delay (5);
  }
  
  // Cuando se haya logrado la conexión, el programa avanzará, por lo tanto, puede informarse lo siguiente
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
  
  delay (1000); // Esta espera es solo una formalidad antes de iniciar la comunicación con el broker

  // Conexión con el broker MQTT
  client.setServer(server, 1883); // Conectarse a la IP del broker en el puerto indicado
  //client.setCallback(callback); // Activar función de CallBack, permite recibir mensajes MQTT y ejecutar funciones a partir de ellos
  delay(1500);  // Esta espera es preventiva, espera a la conexión para no perder información

  timeLast = millis (); // Inicia el control de tiempo
}

void loop() {
  
  //delay(2000);

  //Verificar siempre que haya conexión al broker
  if (!client.connected()) {
    reconnect();  // En caso de que no haya conexión, ejecutar la función de reconexión, definida despues del void setup ()
  }// fin del if (!client.connected())
  client.loop(); // Esta función es muy importante, ejecuta de manera no bloqueante las funciones necesarias para la comunicación con el broker

  if (millis() - timeLast > wait) {
      float t = dht.readTemperature();
      float f = dht.readTemperature(true);
      
      if ( isnan(t) || isnan(f)) {
        Serial.println(F("No hay conexion"));
        return;
      } else {
        Serial.print(F("Temperatura en °C: "));
        Serial.println(t);
        Serial.print(F("Temperatura en °F: "));
        Serial.println(f);


        char mqtt_payload[30] = "";
        snprintf (mqtt_payload, 30, "ºC = %lf", t);
        Serial.print("Publish message: ");
        Serial.println(mqtt_payload);
        client.publish(pubTopic, mqtt_payload);
        Serial.println("> MQTT data published");
        Serial.print("********** End ");
        Serial.println("*****************************************************");
        //delay(2000);
        timeLast = millis(); // Actualización de seguimiento de tiempo
        } 
    }
}

// Función para reconectarse
void reconnect() {
  //Creamos un loop en donde intentamos hacer la conexión
    while(!client.connected()) {
        Serial.print("Conectando a: ");
        Serial.println(mqtt_server);
        //Creamos una nueva cadena de conexión para el servidor
        //e intentamos realizar la conexión nueva
        //si requiere usuario y contraseña la enviamos connect(clientId, username, password)
        String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
        if(client.connect(clientId.c_str())) {
            Serial.println("[DONE]");
        } else {
            Serial.print( "[FAILED] [ rc = " );
            Serial.print( client.state() );
            Serial.println( " : retrying in 5 seconds]" );
            delay( 5000 );
        }
    }
}// fin de void reconnect(
