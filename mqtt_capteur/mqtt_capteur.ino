#include <WiFi.h> // Cette bibliothèque permet à l'ESP32 de se connecter au réseau WiFi.
#include <PubSubClient.h> // Cette bibliothèque vous permet d'envoyer et de recevoir des messages MQTT.
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>


#define BUTTON_PIN 4       // Broche du bouton
#define YELLOW_LED_PIN 32  // Broche de la LED jaune
#define RED_LED_PIN 33     // Broche de la LED rouge
#define DHT_PIN 13         // Broche du capteur DHT11
#define FAN_PIN 18         // Broche du ventilateur
#define I2C_SDA 21  // Broche SDA par défaut
#define I2C_SCL 22  // Broche SCL par défaut

// Create a DHT object
DHT dht(DHT_PIN, 11);

//variable

bool systemeActive=false; //Etat du systeme 
bool manuel=false;  //mode manuel
bool buttonPressed=false;
int limite_temperature=30;
float temperature=25;

// BH1750 sensor object
BH1750 lightMeter;

// Réseau WiFi auquel vous voulez vous connecter. Vous pouvez se connecter aux différents réseaux
const char* ssid = "Galaxy A23 5G 7C5A";
const char* password = "jgfFhv46*u";

// Ce champ contient l'adresse IP ou le nom d'hôte du serveur MQTT.
const char* mqtt_server = "mqtt.ci-ciad.utbm.fr";


WiFiClient espClientGiraux1; // Initialiser la bibliothèque client wifi (connexion WiFi)
PubSubClient client(espClientGiraux1); // Créer un objet client MQTT (connexion MQTT) 
// Maintenant, notre client MQTT s'appelle "client". Nous devons maintenant créer une connexion à un broker.
long lastMsg = 0;


//---Cette fonction initialise les paramètres pour la connexion WiFi et MQTT, configure le mode d'entrée
//et de sortie pour les broches de bouton et de LED, et démarre la communication série.
void setup() {
  Serial.begin(115200);
  
  // Configuration des broches
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  // Initialisation des capteurs
  dht.begin();

  // Initialisation de I2C avec les broches spécifiques
  //Wire.begin(I2C_SDA, I2C_SCL);

  // Initialise le BH1750 sensor (capteur lumière)
  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 initialized.");
  } else {
    Serial.println("Error initializing BH1750!");
    while (1);
  }

  // Éteindre tous les composants au démarrage
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);

  delay(2000); // Donnez-moi le temps de mettre en place le moniteur série
  Serial.println("ESP32 mqtt interface");

  setup_wifi(); // Cette ligne appelle la fonction setup_wifi() pour configurer la connexion Wi-Fi sur l'ESP32.
  client.setServer(mqtt_server, 1883); //définit le serveur (broker) MQTT à utiliser pour la communication et
                                           //  le port de communication à 1883, qui est le port standard pour MQTT.
  client.setCallback(callback); // définit la fonction callback comme la fonction à appeler lorsqu'un message MQTT est reçu.
}

//------Cette fonction se connecte au réseau WiFi en utilisant les paramètres de connexion fournis 
//dans les variables ssid et password.
void setup_wifi() {
  delay(10); // Cette instruction pause l'exécution du programme pendant 10 millisecondes.
  // We start by connecting to a WiFi network
  Serial.println(); // Imprime une ligne vide/saut de ligne dans la console série.
  Serial.print("Connecting to ");
  Serial.println(ssid); 

  WiFi.begin(ssid, password); // //démarre la connexion Wi-Fi avec les informations de connexion (SSID et mot de passe) fournies.

//Cette boucle effectue une pause de 500 millisecondes jusqu'à ce que l'ESP32 soit 
//connecté au réseau Wi-Fi. Le statut de la connexion est obtenu en appelant "WiFi.status()".
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//-----La fonction callback est utilisée pour traiter les messages MQTT reçus par l'ESP32 et de prévoir une action.
//Elle est appelée chaque fois qu'un nouveau message est reçu sur un Topic auquel l'ESP32 est abonné.
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic); // imprime le nom du Topic sur lequel le message a été publié.
  Serial.print(". Message: ");
  
  // Le message reçu est transmis à la fonction en tant que tableau de bytes "message" avec une longueur "length". 
  //Pour pouvoir travailler avec ce message dans le reste du code, nous devons d'abord le convertir
  //en chaîne de caractères.
  String messageTemp=""; // déclare une variable de chaîne temporaire pour stocker le message reçu.
  
  // boucle sur chaque élement dans le tableau de bytes "message" et les ajoute à la chaîne "messageTemp".
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  //Pour chaque itération, l'élément actuel du tableau de bytes "message" est converti en char avec
  //"(char)message[i]" et affiché sur la console série avec "Serial.print((char)message[i])".
  //Ensuite, ce même élément est ajouté à la fin de la chaîne "messageTemp" avec
  //"messageTemp += (char)message[i];
  }
  Serial.println();

//Si un message MQTT (variable messageTemp) est reçu sur le Topic "esp32/led", 
//vous vérifiez si le message est soit "on" ou "off".
//Ensuite, vous modifiez l'état de sortie de la LED physique en fonction du message "messageTemp" 
//reçu en utilisant la fonction digitalWrite.

  //systeme ON/OFF
  if(messageTemp=="systeme_ON"){
    systemeActive=true;
  }
  else if(messageTemp=="systeme_OFF"){
    systemeActive=false;
  }
  else if(messageTemp=="manuel_on"){
    manuel=true;
  }
  else if(messageTemp=="manuel_off"){
    manuel=false;
  }
  if(manuel){
    if(messageTemp=="ventilateur_on"){
        digitalWrite(FAN_PIN, HIGH);
    }
    else if(messageTemp=="ventilateur_off"){
      digitalWrite(FAN_PIN, LOW);
    }
  }
  
  //si reçois un message du type "temperature32" alors update limite_temperature à 32
  if(messageTemp.startsWith("temperature")){
    String valuePart = String((char*)message).substring(11);  // Extraire la partie numérique après "temperature"
    limite_temperature = valuePart.toInt();         // Convertir en entier et mettre à jour la variable
    Serial.printf("Limite temperature Mise a jour %d\n",valuePart.toInt());
  }
}

//La fonction "reconnect()" est utilisée pour garantir la connexion MQTT entre l'ESP32 et le broker MQTT. 
//Elle est appelée dans la boucle principale et elle se répète jusqu'à ce que la connexion soit établie avec succès.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
  // Si la connexion est déjà établie, la fonction ne fait rien. 
  //Si la connexion n'est pas établie, le code essaie de se connecter à l'aide de "client.connect("espClientGiraux1")".
   //Si la connexion est réussie, le code imprime "connected" sur la console série et 
   // s'abonne au topic "esp32/led" avec "client.subscribe("esp32/led")".
    if (client.connect("espClientGiraux1")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/Giraux");
    } 
  // Si la connexion échoue, le code imprime "failed, rc=" suivi de l'état de la connexion avec "client.state()" sur la console série 
   // La fonction se répète jusqu'à ce que la connexion soit établie avec succès.
  else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // On attend 5 secondes avant de retenter la connexion
      delay(5000);
    }
  }
}

void loop() {
  // La première tâche de la fonction principale est de vérifier si le client MQTT est connecté. 
  //Si ce n'est pas le cas, la fonction reconnect() est appelée pour reconnecter le client.
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop(); // La méthode client.loop() est appelée pour traiter les messages MQTT entrants.
        // Maintient la connexion avec le serveur MQTT en vérifiant si de nouveaux messages sont arrivés et en envoyant les messages en attente.
  
  //La dernière partie vérifie le temps écoulé depuis le dernier message publié et n'envoie le prochain message que toutes les 2 secondes (2000 millisecondes).
  
  long now = millis(); // Crée une variable "now" pour stocker le nombre de millisecondes écoulées depuis le démarrage du programme.
  if (now - lastMsg > 500) { // Vérifie si le temps écoulé depuis le dernier message publié est supérieur à 2000 millisecondes.
      lastMsg = now; // Si oui, met à jour la variable "lastMsg" avec la valeur actuelle de "now".

      
      // Lecture de l'état du bouton
    int buttonState = digitalRead(BUTTON_PIN);

    // Détection de l'appui sur le bouton (logique inversée pour INPUT_PULLUP)
    if (buttonState == LOW && !buttonPressed) {
      // Inverser l'état du système
      systemeActive = !systemeActive;
      buttonPressed = true;

      if(systemeActive){
        String payload = "systeme_ON";
        client.publish("giraux/systeme", payload.c_str());
        Serial.println("Data published: " + payload);
      }
      else{
        String payload = "systeme_OFF";
        client.publish("giraux/systeme", payload.c_str());
        Serial.println("Data published: " + payload);
      }

      delay(200); // Anti-rebond
    }

    // Réinitialiser l'état du bouton
    if (buttonState == HIGH) {
      buttonPressed = false;
    }

    if (systemeActive) {
      // Allumer la LED jaune quand le système est actif
      digitalWrite(RED_LED_PIN, HIGH);

      //lumiere
      float lightLevel = lightMeter.readLightLevel();

      // Publie light level data comme JSON
      String lightPayload = "{\"capteur\":\"lumiere\",\"light_level\":" + String(lightLevel) + "}";
      client.publish("giraux/light_sensor", lightPayload.c_str());
      Serial.println("Data published: " + lightPayload);

      
      // Publie temperature data comme JSON
      temperature = dht.readTemperature();
      String temperaturePayload = "{\"capteur\":\"capteur_temperature\",\"value\":" + String(temperature) + "}";
      client.publish("giraux/temperature", temperaturePayload.c_str());
      Serial.println("Data published: " + temperaturePayload);
      
      // Publie humitite data comme JSON
      float humidite = dht.readHumidity();
      String humiditePayload = "{\"capteur\":\"capteur_humidite\",\"value\":" + String(humidite) + "}";
      client.publish("giraux/humidite", humiditePayload.c_str());
      Serial.println("Data published: " + humiditePayload);
      
      if(manuel==false){
        // Contrôle du ventilateur
        if (temperature > limite_temperature) {
          digitalWrite(FAN_PIN, HIGH);
        } else {
          digitalWrite(FAN_PIN, LOW);
        }
      }

      /*
      // Contrôle des LED en fonction de la luminosité
      if (lightLevel < 500) { // Seuil à ajuster selon votre capteur
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(YELLOW_LED_PIN, LOW);
      } else {
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, HIGH);
      }
    } else {
      // Quand le système est inactif
      digitalWrite(YELLOW_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(FAN_PIN, LOW);
    }
    */
  }
  else{
    digitalWrite(RED_LED_PIN, LOW);
  }
}
}
