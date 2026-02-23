#include <WiFi.h> // Cette bibliothèque permet à l'ESP32 de se connecter au réseau WiFi.
#include <PubSubClient.h> // Cette bibliothèque vous permet d'envoyer et de recevoir des messages MQTT.
#include <Stepper.h>

// Définition des broches
#define PHOTORES_1 35 // Photorésistance en haut à gauche
#define PHOTORES_2 36 // Photorésistance en haut à droite
#define PHOTORES_3 34 // Photorésistance en bas à gauche
#define PHOTORES_4 39 // Photorésistance en bas à droite
#define MOTOR1_IN1 13  // Broche IN1 du moteur pas à pas
#define MOTOR1_IN2 14  // Broche IN2 du moteur pas à pas
#define MOTOR1_IN3 27  // Broche IN3 du moteur pas à pas
#define MOTOR1_IN4 26  // Broche IN4 du moteur pas à pas
#define MOTOR2_IN1 25  // Broche IN1 du moteur pas à pas
#define MOTOR2_IN2 33  // Broche IN2 du moteur pas à pas
#define MOTOR2_IN3 19  // Broche IN3 du moteur pas à pas
#define MOTOR2_IN4 21  // Broche IN4 du moteur pas à pas
#define buzzer 32 //Pin buzzer

// Constantes

const int stepsPerRevolution = 2048; // Nombre de pas par révolution pour le moteur 28BYJ-48
const int angleStep = 30;       // Pas de rotation (30 degrés)

// Initialisation du moteur pas à pas
Stepper motor1(stepsPerRevolution, MOTOR1_IN1, MOTOR1_IN3, MOTOR1_IN2, MOTOR1_IN4);
Stepper motor2(stepsPerRevolution, MOTOR2_IN1, MOTOR2_IN3, MOTOR2_IN2, MOTOR2_IN4);


// Variables
int seuilDifference = 10000; // Différence minimum pour activer le moteur
int valeurPhotores[4]; // Pour stocker les valeurs des photorésistances
int motor1_currentPosition = 0; // Position actuelle du panneau (en degrés)
int motor2_currentPosition = 0; // Position actuelle du panneau (en degrés)
bool systeme_state=false; //Etat du systeme ON/OFF
bool manuel=false;             //mode manuel
bool firstloop=true;         //premier loop pour le buzzer

// Réseau WiFi auquel vous voulez vous connecter. Vous pouvez se connecter aux différents réseaux
const char* ssid = "Galaxy A23 5G 7C5A";
const char* password = "jgfFhv46*u";

// Ce champ contient l'adresse IP ou le nom d'hôte du serveur MQTT.
const char* mqtt_server = "mqtt.ci-ciad.utbm.fr";


WiFiClient espClientGiraux2; // Initialiser la bibliothèque client wifi (connexion WiFi)
PubSubClient client(espClientGiraux2); // Créer un objet client MQTT (connexion MQTT) 
// Maintenant, notre client MQTT s'appelle "client". Nous devons maintenant créer une connexion à un broker.
long lastMsg = 0;


//---Cette fonction initialise les paramètres pour la connexion WiFi et MQTT, configure le mode d'entrée
//et de sortie pour les broches de bouton et de LED, et démarre la communication série.
void setup() {
  Serial.begin(115200);

  //pinMode(PHOTORES_1, INPUT);
  //pinMode(PHOTORES_2, INPUT);
  //pinMode(PHOTORES_3, INPUT);
  //pinMode(PHOTORES_4, INPUT);

  pinMode(buzzer, OUTPUT);

  // Configurer le moteur pas à pas
  motor1.setSpeed(10); // Vitesse du moteur (en tr/min)
  motor2.setSpeed(10); // Vitesse du moteur (en tr/min)

  delay(2000); // Donnez-moi le temps de mettre en place le moniteur série

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
  String messageTemp; // déclare une variable de chaîne temporaire pour stocker le message reçu.
  
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

  //Si un message MQTT (variable messageTemp) est reçu sur le Topic "esp32/Giraux", 
  //vous vérifiez si le message est soit "on" ou "off".
  //Ensuite, vous modifiez l'état de sortie de la LED physique en fonction du message "messageTemp" 
  //reçu en utilisant la fonction digitalWrite.

  //message ON/OFF manuel
    if(messageTemp=="systeme_ON"){
      firstloop=true;
      systeme_state=true;
    }
    else if(messageTemp=="systeme_OFF"){
      systeme_state=false;
    }
  

  if(systeme_state==true){

    //message ON/OFF manuel
    if(messageTemp=="manuel_on"){
      manuel=true;
      Serial.printf("Le mode manuel est activée\n");
    }
    else if(messageTemp=="manuel_off"){
      manuel=false;
      Serial.printf("Le mode manuel est desactivée\n");
    }

    if (messageTemp.startsWith("panneau")) {
    char* valuePart1 = (char*)message + 7; // Décaler de 7 pour ignorer "panneau"
    
    // Nettoyer les espaces ou caractères non désirés à la fin
    int i = 0;
    while (valuePart1[i] >= '0' && valuePart1[i] <= '9') {
        i++; // Avancer tant que c'est un chiffre
    }
    valuePart1[i] = '\0'; // Terminer correctement la chaîne à la fin du nombre

    seuilDifference = atoi(valuePart1); // Convertir en entier
    Serial.printf("seuilDifference Mise a jour %d\n", seuilDifference);
    }

  /*
    if(messageTemp.startsWith("panneau")){
    String valuePart1 = String((char*)message).substring(7);  // Extraire la partie numérique après "panneau"
    valuePart1.trim();
    seuilDifference = valuePart1.toInt();         // Convertir en entier et mettre à jour la variable
    Serial.printf("seuilDifference Mise a jour %d\n",seuilDifference);
    }
  */

    //Si mode manuel ON

    if(manuel){
      if(messageTemp=="motor_right"){
        activerMoteur('r');
      }
      else if(messageTemp=="motor_left"){
        activerMoteur('l');
      }
      else if(messageTemp=="motor_up"){
        activerMoteur('u');
      }
      else if(messageTemp=="motor_down"){
        activerMoteur('d');
      }
      else if(messageTemp=="motor_ini"){
        activerMoteur('x');
      }
      if(messageTemp == "buzzer_on"){
        tone(buzzer,1535,500);
        delay(200);
      }
    }
    else{
      noTone(buzzer);
    }
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
  //Si la connexion n'est pas établie, le code essaie de se connecter à l'aide de "client.connect("espClient")".
   //Si la connexion est réussie, le code imprime "connected" sur la console série et 
   // s'abonne au topic "esp32/Giraux" avec "client.subscribe("esp32/Giraux")".
    if (client.connect("espClientGiraux2")) {
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
  if (now - lastMsg > 500 && systeme_state == true) { // Vérifie si le temps écoulé depuis le dernier message publié est supérieur à 2000 millisecondes.
    lastMsg = now; // Si oui, met à jour la variable "lastMsg" avec la valeur actuelle de "now".
    if(firstloop){
      tone(buzzer,1535,500);
      delay(200);
      firstloop=false;
    }
    else if(manuel==false){
      noTone(buzzer);
    }
      // Lire les valeurs des photorésistances
    valeurPhotores[0] = analogRead(PHOTORES_1);
    valeurPhotores[1] = analogRead(PHOTORES_2);
    valeurPhotores[2] = analogRead(PHOTORES_3);
    valeurPhotores[3] = analogRead(PHOTORES_4);

    // Afficher les valeurs pour debug
    Serial.print("Valeurs: ");
    Serial.printf("%d ;",analogRead(PHOTORES_1));
    Serial.printf("%d ;",valeurPhotores[1]);
    Serial.printf("%d ;",valeurPhotores[2]);
    Serial.printf("%d \n",valeurPhotores[3]);


    //Desactiver si mode manuel est true
    if(manuel==false){

      // Identifier la direction de la lumière
      int maxIndex = 0;
      for (int i = 1; i < 4; i++) {
        if (valeurPhotores[i] > valeurPhotores[maxIndex]) {
          maxIndex = i;
        }
      }

      // Comparer la photorésistance la plus éclairée avec les autres
      bool besoinRotation = false;
      for (int i = 0; i < 4; i++) {
        if (i != maxIndex && valeurPhotores[maxIndex] - valeurPhotores[i] > seuilDifference) {
          besoinRotation = true;
          break;
        }
      }

      if (besoinRotation) {
        // Tourner le moteur vers la direction de la lumière
        Serial.print("Tourner vers la direction: ");
        Serial.println(maxIndex);

        if(maxIndex==0){
          activerMoteur('r');
          activerMoteur('d');
        }
        else if(maxIndex==1){
          activerMoteur('r');
          activerMoteur('d');
        }
        else if(maxIndex==2){
          activerMoteur('u');
          activerMoteur('l');
        }
        else if(maxIndex==3){
          activerMoteur('u');
          activerMoteur('l');
        }

        /*
        if(maxIndex==2 ||maxIndex == 3){
          activerMoteur('r');
        }
        else{
          activerMoteur('l');
        }
        if(maxIndex==0 || maxIndex== 3){
          activerMoteur('u');
        }
        else{
          activerMoteur('d');
        }
        */
        besoinRotation = false;
      }
      /*
      if(valeurPhotores[maxIndex] < 500) {
        // Revenir à la position initiale si les valeurs sont égales
        Serial.println("Retour à la position initiale");
        activerMoteur('x'); // 'x' pour indiquer le retour initial
      }
      */
      delay(1000); // Pause entre chaque mesure



    }
  }
}

void activerMoteur(char direction) { 
  // x: return to zero, r: right, l: left, u: up, d: down
  Serial.printf("activerMoteur : %c\n", direction);
  if (direction == 'x') {
    // Revenir à la position initiale
    int stepsToMove = (0 - motor1_currentPosition) * stepsPerRevolution / 360;
    motor1.step(stepsToMove);
    motor1_currentPosition = 0;

    stepsToMove = (0 - motor2_currentPosition) * stepsPerRevolution / 360;
    motor2.step(stepsToMove);
    motor2_currentPosition = 0;
  } else {
    // Tourner de 30 degrés dans la direction mouvement x, y
    int stepsToMove = angleStep * stepsPerRevolution / 360;

    if (direction == 'r') {
      // Mouvement sens d'une horloge
      motor1.step(stepsToMove);
      motor1_currentPosition += angleStep;
    } 
    else if (direction == 'l') {
      // Mouvement contre sens horloge
      stepsToMove = -stepsToMove;
      motor1.step(stepsToMove);
      motor1_currentPosition -= angleStep;
    } 
    else if (direction == 'u') {
      // Mouvement vers le haut (z)
      motor2.step(stepsToMove);
      motor2_currentPosition += angleStep;
    } 
    else if (direction == 'd') {
      // Mouvement vers le bas (contre sens horloge)
      stepsToMove = -stepsToMove;
      motor2.step(stepsToMove);
      motor2_currentPosition -= angleStep;
    }
  }
}
