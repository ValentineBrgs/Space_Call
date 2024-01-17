#include <AsyncDelay.h>
#include <SoftWire.h> //bibliotheque permettant de piloter un second bus I2C en tant que maitre
#include <Wire.h>

///////////////////////Communication I2C avec Robinson///////////////////////
#define Arduino_adress 0x16 // Adresse de l'Arduino sur le bus I2C de Robinson

///////////////////////Communication I2C avec les ecoles///////////////////////
const int sdaPin = A0;// changer ceci
const int sclPin = A0;//changer ceci
SoftWire sw(sdaPin, sclPin);
char swTxBuffer[32]; // These buffers must be at least as large as the largest read or write you perform.
char swRxBuffer[32]; // These buffers must be at least as large as the largest read or write you perform.
int I2CAdressSchool[] = { 1, 2 }; // List of the I2C adress of the school board
int School_number = 2; // List of the I2C adress of the school board
int firstSchool = -1; // index of the first school to speak during the round
int emitting_school = -1; // index of the school to speak during the round
int receiving_school = -1; // index of the school to listen during the round


AsyncDelay delayGetlog;

AsyncDelay delayStartnewRound; // delay between the end of the last round and the beginning of the new one
AsyncDelay delayEndRound; // delay between the beginning and the end of the round
unsigned long lastTime_EndRound = 0;
unsigned long lastTime_NewRound = 0;
const int T_delayEndRound = 1 * 60 * 1000; // delay between the beginning and the end of the round
const int T_delayStartNewRound = 1 * 10 * 1000; // delay between the end of the last round and the beginning of the new one
bool StartNewRound = true; // dit si on a le droit de commencer un nouveau round ou non
bool EndRound = false; // dit si on a le drit de terminer le round ou pas

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
    delayGetlog.start(2000, AsyncDelay::MILLIS);
    Serial.println("Master connected");
    delay(1000);
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
    
    // On regarde si on peut commencer un nouveau round
    if (millis() - lastTime_EndRound > T_delayStartNewRound && StartNewRound) {
        lastTime_NewRound = millis();
        StartNewRound = false;
        EndRound = true;

        Serial.println("beginning of the round"); // Debug
        // On commence la boucle

        // Initialisation of the LDR
        for (int i = 0; i < School_number; i++) {
            sendmessage(I2CAdressSchool[i], 0x01); // Init LDR
            Serial.println("Request Arduino " + String(I2CAdressSchool[i]) + " Init LDR"); // DEBUG
        }
        delay(3000);


        //Boucle qui active l'écoute pour l'école s+1 et la transmission pour l'école s
        for (int s = 0; s < School_number; s++){
        int emitting_school= s ;
        receiving_school = (s + 1) % School_number;
        //Ask to the first school to listen
        sendmessage(I2CAdressSchool[receiving_school], 0x03);
        Serial.println("Request Arduino " + String(I2CAdressSchool[firstSchool]) + " Listen"); // DEBUG
        delay(500);
        // transmit order
        sendmessage(I2CAdressSchool[emitting_school], 0x02);
        Serial.println("Request Arduino " + String(I2CAdressSchool[firstSchool]) + " tosay a word"); // DEBUG
 
        delay(6000); // Wait for 6 second before going to next school
        }
        
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
            Serial.println("Log recu : " + Log_); // DEBUG
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
