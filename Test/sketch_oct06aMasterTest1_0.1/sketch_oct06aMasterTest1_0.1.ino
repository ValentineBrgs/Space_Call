#include <AsyncDelay.h>
#include <SoftWire.h> //bibliotheque permettant de piloter un second bus I2C en tant que maitre
#include <Wire.h>

///////////////////////Communication I2C avec Robinson///////////////////////
#define Arduino_adress 0x16 // Adresse de l'Arduino sur le bus I2C de Robinson

///////////////////////Communication I2C avec les ecoles///////////////////////
const int sdaPin = A2;
const int sclPin = A3;
SoftWire sw(sdaPin, sclPin);
char swTxBuffer[32]; // These buffers must be at least as large as the largest read or write you perform.
char swRxBuffer[32]; // These buffers must be at least as large as the largest read or write you perform.
const int I2CAdressSchoolToTest = 1; //I2C adress of the board we want to test.
const int I2CAdressSchool[] = { I2CAdressSchoolToTest, 22 }; // List of the I2C adress of the school board
const int School_number = 2; // Number of school 
int emitting_school = -1; // index of the school to speak during the round
int receiving_school = 0; // index of the school to listen during the round


AsyncDelay delayGetlog;

AsyncDelay delayStartnewRound; // delay between the end of the last round and the beginning of the new one
AsyncDelay delayEndRound; // delay between the beginning and the end of the round
unsigned long lastTime_EndRound = 0;
unsigned long lastTime_NewRound = 0;
unsigned long lastTime_newWord = 0;
const long T_delayEndRound = 170000; // delay between the beginning and the end of the round
const int T_delayStartNewRound = 1 * 15 * 1000; // delay between the end of the last round and the beginning of the new one
const int T_delayChangeWord = 1 * 15 * 1000; // delay between saying 2 words
bool StartRound1 = true; // dit si on a le droit de commencer le round  1 ou non
bool StartRound2 = false; // dit si on a le droit de commencer le round  2 ou non
bool Round1_in_progress = false; // dit si on a le droit de terminer le round ou pas
bool Round2_in_progress = false; // dit si on a le droit de terminer le round ou pas


void setup()
{
    Serial.begin(9600);

    // initSchoolI2C(); // Initialisation du bus I2C ecole
    sw.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
    sw.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
    sw.setDelay_us(5);
    sw.setTimeout(1000);
    sw.begin();
    initRobinsonI2C(); // Initialisation du bus I2C Robinson
    // Initialisation des délais
    delayGetlog.start(500, AsyncDelay::MILLIS);
    Serial.println("Master connected");
    Serial.println("enter 'start' to start the test");
    while (Serial.available() == 0) {
        delay(500);
    }

}

void loop()
{
    // On demande si il y a des logs a enregistrer à intervalle regulier
    if (delayGetlog.isExpired()) {
        for (int i = 0; i < School_number; i++) {
            getLog(I2CAdressSchool[i]);
        }
        delayGetlog.repeat();
    }
    
    // Round 1 : On regarde si on peut commencer un nouveau round
    if (millis() - lastTime_EndRound > T_delayStartNewRound && StartRound1) {
        lastTime_NewRound = millis();
        StartRound1 = false;
        Round1_in_progress = true;
        // On active toute les cartes écoles
        Serial.println("############################################");
        Serial.println("beginning of the round 1 :"); // Debug
        Serial.println("degraded acceptance test");
        Serial.println("############################################");
  
        
        for (int i = 0; i < School_number; i++) {
            sendmessage(I2CAdressSchool[i], 0x01); // Init LDR
            Serial.println("Request Arduino " + String(I2CAdressSchool[i]) + " Init LDR"); // DEBUG
        }
 
        lastTime_newWord = millis(); //Setup for saying the first word in 3000ms 
    }

    // Round 2 : On regarde si on peut commencer un nouveau round
    if (millis() - lastTime_EndRound > T_delayStartNewRound && StartRound2) {
        lastTime_NewRound = millis();
        StartRound2 = false;
        Round2_in_progress = true;
        // On active toute les cartes écoles
        Serial.println("############################################");
        Serial.println("beginning of the round 2 :"); // Debug
        Serial.println("No reception acceptance test");
        Serial.println("############################################");
  
        
        for (int i = 0; i < School_number; i++) {
            sendmessage(I2CAdressSchool[i], 0x01); // Init LDR
            Serial.println("Request Arduino " + String(I2CAdressSchool[i]) + " Init LDR"); // DEBUG
        }
 
        lastTime_newWord = millis(); //Setup for saying the first word in 3000ms 
    }



    //Round 1 : On regarde si on peut commencer à dire un nouveau mot
    if (millis() - lastTime_newWord > T_delayChangeWord && Round1_in_progress){
        lastTime_newWord = millis();
        emitting_school = (emitting_school + 1) % School_number;
        receiving_school = (receiving_school + 1) % School_number;
        //Ask to the first school to listen
        sendmessage(I2CAdressSchool[receiving_school], 0x03);
        Serial.println("Request Arduino " + String(I2CAdressSchool[receiving_school]) + " Listen"); // DEBUG
        delay(500);
        // transmit order
        sendmessage(I2CAdressSchool[emitting_school], 0x02);
        Serial.println("Request Arduino " + String(I2CAdressSchool[emitting_school]) + " tosay a word"); // DEBUG
    }

    //Round 2 : On regarde si on peut commencer à dire un nouveau mot
    if (millis() - lastTime_newWord > T_delayChangeWord && Round2_in_progress){
        lastTime_newWord = millis();
        emitting_school = (emitting_school + 1) % School_number;
        receiving_school = (receiving_school + 1) % School_number;
        //Ask to the first school to listen
        sendmessage(I2CAdressSchool[receiving_school], 0x03);
        Serial.println("Request Arduino " + String(I2CAdressSchool[receiving_school]) + " Listen"); // DEBUG
        delay(500);
        // Ask the first school to speak
        if(emitting_school==0){
            sendmessage(I2CAdressSchool[emitting_school], 0x02);
            Serial.println("Request Arduino " + String(I2CAdressSchool[emitting_school]) + " tosay a word"); // DEBUG
        }
        else{
            Serial.println("Request Arduino " + String(I2CAdressSchool[emitting_school]) + " to not say anything"); // DEBUG")
        }
    }


    

    // On regarde si on fait terminer le round 1 
    if (millis() - lastTime_NewRound > T_delayEndRound && Round1_in_progress) {
        lastTime_EndRound = millis();
        StartRound2 = true;
        Round1_in_progress = false;
        Serial.println("end of round 1"); // Debug
        addLog("end of the round 1");
    }

    // On regarde si on fait terminer le round 2 
    if (millis() - lastTime_NewRound > T_delayEndRound && Round2_in_progress) {
        lastTime_EndRound = millis();
        StartRound1 = true;
        Round2_in_progress = false;
        Serial.println("end of round 2"); // Debug
        addLog("end of round 2");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Fonctions Utiles /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// School I2C Communication /////////////////////////////////

void initSchoolI2C()
{
    sw.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
    sw.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
    sw.setDelay_us(5);
    sw.setTimeout(1000);
    sw.begin();
}

// Fonction qui ajoute les logs
void addLog(String Log)
{
    String Log_ = getDate() + String(" ") + Log;
}

// Fonction qui récupére les logs de l'arduino
void getLog(int ArduinoNo)
{
    String Log = "Start";
    while (Log != "") {
        Log = "";
        int numBytes = sw.requestFrom(ArduinoNo, (uint8_t)32);
        for (int i = 0; i < numBytes; ++i) {
            char c = sw.read(); // receive a byte as character
            if (c == char('!')) {
                break; // Si on a le byte de fin de chaine on arrete la transmission
            }
            Log += String(c);
        }
        // Si on a recu quelque chose, on l'enregistre
        if (Log != "") {
            String Log_ = "From Arduino " + String(ArduinoNo) + " : " + Log;
            Serial.println("Log : " + Log_); // DEBUG
            addLog(Log_);
        }
    }
}

// Fonction qui donne le temps depuis lequelle l'arduino est démarré au format [hh:mm:ss]
String getDate()
{
    unsigned long currentMillis = millis(); // Obtient le temps écoulé depuis le démarrage de la carte Arduino
    int seconds = (currentMillis / 1000) % 60; // Calcule les secondes
    int minutes = (currentMillis / (1000 * 60)) % 60; // Calcule les minutes
    int hours = (currentMillis / (1000 * 60 * 60)) % 24; // Calcule les heures

    char buffer[12]; // Assez grand pour contenir "[hh;mm;ss]\0"
    sprintf(buffer, "[%02d:%02d:%02d]", hours, minutes, seconds);
    return (String(buffer));
}

// Envoie un message à l'arduino spécifié en argument via I2C
void sendmessage(int ArduinoNo, int msg)
{
    /*  0x01 -> initLDR
     *  0x02 -> send new word
     *  0x03 -> MorseCommunicationActivated = false : desactivate the morse communication
     *  0x04 -> MorseCommunicationActivated = true : activate the morse communication
     */
    sw.beginTransmission(ArduinoNo);
    sw.write(byte(msg));
    sw.endTransmission();
}

////////////////////////////////// Robinson I2C Communication /////////////////////////////////

void initRobinsonI2C()
{
    Wire.begin(Arduino_adress);
    Wire.onRequest(I2COnRequestEvent);
}

void I2COnRequestEvent()
{
    // A completer
}

void I2CReceiveEvent(int n)
{
    // A completer
}
