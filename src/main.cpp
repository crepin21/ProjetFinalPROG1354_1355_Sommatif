/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include "RTClib.h" 
#include "Timer.h"


// Definition du mot de passe Wifi
const char* ssid = "BELL470";
const char* password = "CMR062021";
//const char* ssid     = "IDO-OBJECTS";
//const char* password = "42Bidules!";


#define SEALEVELPRESSURE_HPA (1013.25)

// Creation d'objets
RTC_DS3231 rtc;
Adafruit_BME280 bme; // I2C
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//Declaration de variables
Timer Temps;
long int delayMS = 1000;
long int PresentMillis;
unsigned long delayTime;



/**
 * @brief Fonction retournant la temperature du capteur
 * 
 * @return String 
 */
String readBmeTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = bme.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {    
    Serial.println("Failed to read from BME sensor!");
    return "--";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

/**
 * @brief Fonction retournant l'humidite du capteur
 * 
 * @return String 
 */
String readBmeHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = bme.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from bme sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

/**
 * @brief Fonction retournant l'heure et la minute grace au RTC
 * 
 * @return String 
 */
String readRtcTime() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  DateTime now = rtc.now();
  float h = (now.hour(), DEC);
  String Heure = String(h) + String((now.minute(), DEC));
  if (isnan(h)) {
    Serial.println("Failed to read from rtc sensor!");
    return "--";
  }
  else {
    Serial.println(h);
    return Heure;
  }
}

//Creation de la page web contenue dans la variable index_html[]
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1"> 
  <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" >
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.8.1/font/bootstrap-icons.css">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem;
        text-align: center;
       }
    p { font-size: 3.0rem;
        text-align: center;
      }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>CHAMBRE FROIDE</h2>
  <p>
    <i class="bi bi-thermometer-sun" style="color:#059e8a;"></i>
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="bi bi-droplet-fill" style="color:#00add6;"></i>
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span> <!--%HUMIDITY% pour y placer l'humidide du capteur-->
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <i class="bi bi-watch" style="color:#059e8a;"></i>
    <span class="dht-labels">Time</span> 
    <span id="time">%TIME%</span>
    <sup class="units">&deg;H</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      <!--Mise a jour de l element d'id temperature-->
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 5000 ) ;          <!--Delai pour rafraichir la page -->

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 5000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("time").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/time", true);
  xhttp.send();
}, 5000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if (var == "TEMPERATURE")
  {
    return readBmeTemperature();  //Renvoie la T* au niveau de l'espace reserve appele TEMPERATURE 
  }
  else if (var == "HUMIDITY")
  {
    return readBmeHumidity();     //Renvoie H* au niveau de l'espace reserve appele HUMIDITY
  }
  else
  {
    if (var == "TIME")
    {
      return readRtcTime();
    }  
  }
 /* if(var == "TEMPERATURE"){
    return readBmeTemperature();  //Renvoie la T* au niveau de l'espace reserve appele TEMPERATURE 
  }
  else if(var == "HUMIDITY"){
    return readBmeHumidity();     //Renvoie H* au niveau de l'espace reserve appele HUMIDITY
  }  */
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
  
                                   //Test fonctionnement du BME
  Serial.println(F("BME280 test"));
  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println("-- Default Test --");
  delayTime = 1000;
  Serial.println();
                         //Test fonctionnement du RTC
 if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
	Temps.startTimer(delayMS);
     if (Temps.isTimerReady())   //delayMS c est ecoule
	   {	
        Serial.println("Connecting to WiFi..");
     }
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

                                         // Route for root / web page

  //Evoie du code html stocker dans la variable
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  //Remplacement de la temperature, humidite et heure
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBmeTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBmeHumidity().c_str());
  });
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readRtcTime().c_str());
  });

  // Start server
  server.begin();
}

void loop(){
  
}