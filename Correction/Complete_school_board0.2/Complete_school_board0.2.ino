//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////     Wiring   ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/* ARDUINO UNO
 *
 ** pin13 --------------- R=220Ohms  --- Led+
 ** Gnd   --------------- Led-
 *
 ** 5V --------------- Photoresistance
 ** A0 --------------- Photoresistance + R220Ohms
 ** Gnd   ------------ R220Ohms
 */

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  Variable  /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>

////////////// Constants ///////////////////////////////////////////////////////////
const int ledPin = 7; // LED Pin
const int LDRPin = A0; // Photoresistor Pin
const unsigned long dotDuration = 100; // Morse dot duration in milliseconds
#define NB_WORDS 7 // 7 words
#define LEN_WORDS 11 // 10 characters max

///////////// Morse Emission Variables ////////////////////////////////////////////////
String MessageToSend = ""; // plain message to be sent in morse (initialized as 1st word of list but is changed once a message has been heard)

///////////// Morse Reception Variables ///////////////////////////////////////////////
String Translation = ""; //plain word that will be emitted  
int threshold = 50; // Adjust this value based on ambient light
String receivedMessageMorse = ""; // Received message in Morse
String receivedMessagePlain = ""; // Received message in plain text
unsigned long lastTime0 = 0; // Date at which a light signal turned off
unsigned long lastTime1 = 0; // Date at which a light signal turned on
int signalPinState = LOW; // State of the received light signal

///////////// Process Variables ////////////////////////////////////////////////////
bool MorseCommunicationActivated = false; // Whether Arduinos are allowed to generate new Morse messages or not
volatile bool I2CEventReceived = false; // If the Arduino received I2C events
bool bool_initLDR = false;
bool ordreTransmetReceived = false;
bool ordreEcouteReceived = false;


/////////// Translation Structures //////////////////////////////////////////////////////////////
struct DictStruct {
  int id[NB_WORDS];
  char mot[NB_WORDS][LEN_WORDS];
};

DictStruct init_list;
DictStruct translated_list;

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c))) // Simple function to find the min between 3 variables

///////////// Log Variables ////////////////////////////////////////////////////
const int Log_Tablesize = 8; // Log Table Size
String LogTable[Log_Tablesize] = { "", "", "", "", "", "", "", "" }; // Array containing logs
int Log_StartingIndex = 0; // Index of the first log
int Log_length = 0; // Number of logs in the log array

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  Main Code  ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  /// Communication Initialization /////////////////////////

    // put your setup code here, to run once:
    Serial.begin(9600);
    // I2C communication initialization
    Wire.begin(1);
    Wire.onRequest(I2CRequestEvent);
    Wire.onReceive(I2CReceiveEvent);
    // Pins Initialization
    pinMode(ledPin, OUTPUT);
    addLog("Let's start");
    delay(2000); // Ensure all boards have started properly
    initLDR();

    /// Traduction  initialisation//////////////////

    for (int i = 0; i < NB_WORDS; ++i) {
    init_list.id[i] = i + 1; // Id from 1 to 7
    translated_list.id[i] = i + 1; // Id from 1 to 7
    // Assign words to lists
    switch (i) {
      case 0:
        strcpy(init_list.mot[i], "space"); 
        strcpy(translated_list.mot[i], "espace"); // add your translation
        break;
      case 1:
        strcpy(init_list.mot[i], "launch");
        strcpy(translated_list.mot[i], "lancement"); // add your translation
        break;
      case 2:
        strcpy(init_list.mot[i], "student");
        strcpy(translated_list.mot[i], "etudiant"); // add your translation
        break;
      case 3:
        strcpy(init_list.mot[i], "cubsat");
        strcpy(translated_list.mot[i], "cubsat"); // add your translation
        break;
      case 4:
        strcpy(init_list.mot[i], "challenge");
        strcpy(translated_list.mot[i], "defi"); // add your translation
        break;
      case 5:
        strcpy(init_list.mot[i], "orbit");
        strcpy(translated_list.mot[i], "orbite"); // add your translation
        break;
      case 6:
        strcpy(init_list.mot[i], "experiment");
        strcpy(translated_list.mot[i], "experience"); // add your translation
        break;
    }
  }

  // shuffle the translated list 
  shuffle(translated_list);

  for (int i = 0; i < NB_WORDS; ++i){
    Serial.println(translated_list.mot[i]);
    Serial.println(translated_list.id[i]);

    }
  for (int i = 0; i < NB_WORDS; ++i){
      Serial.println(init_list.mot[i]);
      Serial.println(init_list.id[i]);

  }

  //Initialisation will use the first word of the list
  MessageToSend = translated_list.mot[1]; 
 
}

void loop()
{   
    int lightValue = analogRead(LDRPin);
    unsigned long currentTime = millis();
    
      // Check if an I2C event has been received
    if (I2CEventReceived) {
        I2CEventReceived = false;
        handleI2CReceiveEvent();
    }

    // Check if the photoresistor needs to be reinitialized
    if (bool_initLDR) {
        bool_initLDR = false;
        initLDR();
    }

    // Check if there is a message to send
    if (ordreTransmetReceived){
        addLog(String("Message to send : ") + MessageToSend ); //DEBUG
        SendMorseMessage(MessageToSend);
        ordreTransmetReceived=false;
    }

    if (ordreEcouteReceived){
        // If rising edge detected
        if (lightValue > threshold and signalPinState == LOW) {
            LDRrisingEdge(currentTime);
        }

        // If falling edge detected
        if (lightValue < threshold and signalPinState == HIGH) {
            LDRfallingEdge(currentTime);
        }

        // Check if reception is complete and if the message can be decoded
        if ((EndMessage(receivedMessageMorse, 7) == "/...-.-")) {
            receivedMessageMorse += "/";
            receivedMessagePlain = decodePhrase(receivedMessageMorse);
            receivedMessagePlain = receivedMessagePlain.substring(0, receivedMessagePlain.length() - 5); // On retire le STOP final
            addLog(String("Messagage recu : ") + receivedMessagePlain);//DEBUG
            receivedMessageMorse = "";
            // Handling the next word to send 
            /* If the word has been correclty translated, the translated word will be sent. 
            Else, a random word will be sent and a error log will be saved */
            Translation = translate(receivedMessagePlain);
            if (Translation == "error"){
                addLog(String("Error no translation, Received message: ") + receivedMessagePlain);
                int j = random(0, NB_WORDS);
                Translation = init_list.mot[j];
            }
            MessageToSend = Translation;
            addLog(String("Traduction : ") + Translation);
            Translation = "";
            receivedMessagePlain = "";  
            ordreEcouteReceived = false; 
        }

        // If there has been no communication for a long time, consider the communication as terminated
        if (((currentTime - lastTime0) > (dotDuration * 20)) && (receivedMessageMorse != "")) {
            receivedMessageMorse += "/";  
            addLog(String("Error receiving message") );
            addLog(receivedMessageMorse);
            receivedMessageMorse = ""; // consider communication as failed
            int j = random(0, NB_WORDS);
            MessageToSend = init_list.mot[j];
            ordreEcouteReceived = false; 
        }



    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Useful Functions /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function to calibrate the photoresistor based on ambient light
void initLDR()
{
    int lightValue = analogRead(LDRPin);
    int lightValueMin = lightValue;
    int lightValueMax = lightValue;
    int n = 0;
    float lightValueMean = lightValue;
    float lightValueStandardDeviation = 0;
    unsigned long startingTime;
    addLog("Calibrating the LDR ...");
    digitalWrite(ledPin, LOW);
    startingTime = millis();
   // Check ambient light for a certain period
    while ((millis() - startingTime) < 2000) {
        lightValue = analogRead(LDRPin);
        if (lightValue < lightValueMin) {
            lightValueMin = lightValue;
        }
        if (lightValue > lightValueMax) {
            lightValueMax = lightValue;
        }
        lightValueMean = (n * lightValueMean + lightValue) / (n + 1); // Calcul de la moyenne
        lightValueStandardDeviation = (lightValueStandardDeviation * n + (lightValueMean - lightValue) * (lightValueMean - lightValue)) / (n + 1); // Calcul de la variance
        n += 1;
        delay(50);
    }
    threshold = lightValueMean + 10;
    
    // Send Logs

    addLog("LDR Calibration Done.");
    addLog("Luminosite Max = " + String(lightValueMax));
    addLog("Luminosite Moyenne = " + String(lightValueMean));
    addLog("Valeur de treshold = " + String(threshold));
}

////////////////////////////////// I2C Communication /////////////////////////////////

// Function called when the master requests a value
void I2CRequestEvent()
{
    String Log = getLog();
    char LogToSend[33];
    Log += "!"; // Append the end-of-string character
    Log.toCharArray(LogToSend, 33); // Convert the log to a char array
    Wire.write(LogToSend); // Send up to 32 characters
}

// Function called when the master writes a value
void I2CReceiveEvent(int n)
{
    I2CEventReceived = true;
    /*  0x01 -> initLDR
     *  0x02 -> send word
     *  0x03 -> execute Listen function
     */
}

// Function that reads what the master has sent and performs the instructions
void handleI2CReceiveEvent()
{
    /*  0x01 -> initLDR
     *  0x02 -> send word
     *  0x03 -> execute Listen function
     */

    byte c;
    while (Wire.available()) {
        c = Wire.read();
        addLog("Caractère reçu : " + String(c)); // DEBUG
        Serial.println("Caractère reçu : " + String(c)); // DEBUG
        switch (c) {
        case byte(0x01):
            bool_initLDR = true;
            break;
        case byte(0x02):
            // Ordre de transmettre
            ordreTransmetReceived = true;
            break;
        case byte(0x03):
            // Ordre d'écouter
            ordreEcouteReceived = true;
            break;     

        default:
            break;
        }
    }
}

////////////////////////////////////////////// Gestion des logs ////////////////////////////////////////

// Add a log to the LogTable
void addLog(String Log)
{
    int index;
    
    // Logs can only be up to 32 characters long
    while (Log.length() > 31) {
        // Determine where to write the new Log
        index = (Log_StartingIndex + Log_length) % (Log_Tablesize);
        LogTable[index] = Log.substring(0, 30) + "&";
        Log_length += 1;
        Log = Log.substring(30);
    }
    
    index = (Log_StartingIndex + Log_length) % (Log_Tablesize);
    Log_length += 1;
    LogTable[index] = Log;
}

// Dequeue the Log from the LogTable starting from the bottom
String getLog()
{
    if (Log_length == 0) { // Check if there are any Logs left to send
        return ("");
    }
    String Log = LogTable[Log_StartingIndex];
    LogTable[Log_StartingIndex] = ""; // Free up memory
    Log_StartingIndex += 1;
    Log_StartingIndex = Log_StartingIndex % (Log_Tablesize);
    Log_length -= 1;
    return (Log);
}
///////////////////////////////////////////// Communication Morse ////////////////////////////////////////////


///////////////// TRANSMISSION ///////////////////////////////////////////:

// Function called on detection of a luminous UP edge
void LDRrisingEdge(unsigned long currentTime)
{
    unsigned long duration = currentTime - lastTime0;
    signalPinState = HIGH;
    lastTime1 = currentTime;

    if (duration < (dotDuration * 2)) {
        // Short pause,
        receivedMessageMorse += "";
    } else if (duration < (dotDuration * 5)) {
        // "Medium pause corresponding to a change of letter
        receivedMessageMorse += "/";
        Serial.print("/"); // DEBUG
    } else {
        // "Long pause corresponding to a change of word
        if (receivedMessageMorse != "") {
            receivedMessageMorse += "//";
        }
        Serial.print("//"); // DEBUG
    }
}

// Function called on detection of a luminous DOWN edge
void LDRfallingEdge(unsigned long currentTime)
{
    signalPinState = LOW;
    lastTime0 = currentTime;
    unsigned long duration = currentTime - lastTime1;
    if (duration < (dotDuration / 2)) // Probably corresponds to a light artifact to be disregarded
    {
        // Do nothing
    } else if (duration < (dotDuration * 2)) {
        // Short signal corresponds to a dot
        receivedMessageMorse += ".";
    } else {
        // Long signal, correspond to a dash
        receivedMessageMorse += "-";
    }
}

// Function that sends the word given in plain
void SendMorseMessage(String plainMessage) {
    addLog("Message to send: " + plainMessage);
    for (int i = 0; i < plainMessage.length(); i++) {
        char currentChar = plainMessage.charAt(i);
        if (currentChar == ' ') {
            delay(dotDuration * 6); // Turn off the LED for 6 times when switching words
        } else {
            sendCharacterMorse(currentChar);
            delay(dotDuration * 3); // Turn off the LED for 3 times when switching alphanumeric characters
        }
    }

    delay(dotDuration * 6);
    sendCharacterMorse('!'); // Send the final character
    addLog("End of transmission");

}

// Generate dot in morse
void dot()
{
    digitalWrite(ledPin, HIGH);
    delay(dotDuration);
    digitalWrite(ledPin, LOW);
    delay(dotDuration);
}

// Generate dash in morse
void dash()
{
    digitalWrite(ledPin, HIGH);
    delay(dotDuration * 3);
    digitalWrite(ledPin, LOW);
    delay(dotDuration);
}

// Send backe morse letter for a given character
void sendCharacterMorse(char c)
{
    switch (c) {
    case 'A':
    case 'a':
        dot();
        dash();
        break;
    case 'B':
    case 'b':
        dash();
        dot();
        dot();
        dot();
        break;
    case 'C':
    case 'c':
        dash();
        dot();
        dash();
        dot();
        break;
    case 'D':
    case 'd':
        dash();
        dot();
        dot();
        break;
    case 'E':
    case 'e':
        dot();
        break;
    case 'F':
    case 'f':
        dot();
        dot();
        dash();
        dot();
        break;
    case 'G':
    case 'g':
        dash();
        dash();
        dot();
        break;
    case 'H':
    case 'h':
        dot();
        dot();
        dot();
        dot();
        break;
    case 'I':
    case 'i':
        dot();
        dot();
        break;
    case 'J':
    case 'j':
        dot();
        dash();
        dash();
        dash();
        break;
    case 'K':
    case 'k':
        dash();
        dot();
        dash();
        break;
    case 'L':
    case 'l':
        dot();
        dash();
        dot();
        dot();
        break;
    case 'M':
    case 'm':
        dash();
        dash();
        break;
    case 'N':
    case 'n':
        dash();
        dot();
        break;
    case 'O':
    case 'o':
        dash();
        dash();
        dash();
        break;
    case 'P':
    case 'p':
        dot();
        dash();
        dash();
        dot();
        break;
    case 'Q':
    case 'q':
        dash();
        dash();
        dot();
        dash();
        break;
    case 'R':
    case 'r':
        dot();
        dash();
        dot();
        break;
    case 'S':
    case 's':
        dot();
        dot();
        dot();
        break;
    case 'T':
    case 't':
        dash();
        break;
    case 'U':
    case 'u':
        dot();
        dot();
        dash();
        break;
    case 'V':
    case 'v':
        dot();
        dot();
        dot();
        dash();
        break;
    case 'W':
    case 'w':
        dot();
        dash();
        dash();
        break;
    case 'X':
    case 'x':
        dash();
        dot();
        dot();
        dash();
        break;
    case 'Y':
    case 'y':
        dash();
        dot();
        dash();
        dash();
        break;
    case 'Z':
    case 'z':
        dash();
        dash();
        dot();
        dot();
        break;
    case '0':
        dash();
        dash();
        dash();
        dash();
        dash();
        break;
    case '1':
        dot();
        dash();
        dash();
        dash();
        dash();
        break;
    case '2':
        dot();
        dot();
        dash();
        dash();
        dash();
        break;
    case '3':
        dot();
        dot();
        dot();
        dash();
        dash();
        break;
    case '4':
        dot();
        dot();
        dot();
        dot();
        dash();
        break;
    case '5':
        dot();
        dot();
        dot();
        dot();
        dot();
        break;
    case '6':
        dash();
        dot();
        dot();
        dot();
        dot();
        break;
    case '7':
        dash();
        dash();
        dot();
        dot();
        dot();
        break;
    case '8':
        dash();
        dash();
        dash();
        dot();
        dot();
        break;
    case '9':
        dash();
        dash();
        dash();
        dash();
        dot();
        break;
    case '!': // End of transmission caracter
        dot();
        dot();
        dot();
        dash();
        dot();
        dash();
        break;
    default:
        dash();
        dot();
        dot();
        break;
        break;
    }
}



// // Function returning the last n characters of a String
String EndMessage(String message, int n)
{
	int StartIndex = abs(static_cast<int>(message.length() - n));
	return (message.substring(StartIndex));
}

//////////////////////////// Listening /////////////////////////////////////
// Decode a Morse code phrase into plain text
String decodePhrase(String morseCode)
{
    const int n = morseCode.length();
    String message = "";
    String letterMorse = "";

    for (int i = 0; i < n; i++) {
        if (morseCode.charAt(i) == '/') {
            // Change to a new letter
            message += decodeMorse(letterMorse);
            letterMorse = "";
        } else {
            letterMorse += morseCode.charAt(i);
        }
    }
    return message;
}

// Decode a Morse code letter (e.g., .--) into an alphanumeric character
String decodeMorse(String letterMorse)
{
    String letter = "?";

    if (letterMorse == ".-") {
        letter = "A";
    } else if (letterMorse == "-...") {
        letter = "B";
    } else if (letterMorse == "-.-.") {
        letter = "C";
    } else if (letterMorse == "-..") {
        letter = "D";
    } else if (letterMorse == ".") {
        letter = "E";
    } else if (letterMorse == "..-.") {
        letter = "F";
    } else if (letterMorse == "--.") {
        letter = "G";
    } else if (letterMorse == "....") {
        letter = "H";
    } else if (letterMorse == "..") {
        letter = "I";
    } else if (letterMorse == ".---") {
        letter = "J";
    } else if (letterMorse == "-.-") {
        letter = "K";
    } else if (letterMorse == ".-..") {
        letter = "L";
    } else if (letterMorse == "--") {
        letter = "M";
    } else if (letterMorse == "-.") {
        letter = "N";
    } else if (letterMorse == "---") {
        letter = "O";
    } else if (letterMorse == ".--.") {
        letter = "P";
    } else if (letterMorse == "--.-") {
        letter = "Q";
    } else if (letterMorse == ".-.") {
        letter = "R";
    } else if (letterMorse == "...") {
        letter = "S";
    } else if (letterMorse == "-") {
        letter = "T";
    } else if (letterMorse == "..-") {
        letter = "U";
    } else if (letterMorse == "...-") {
        letter = "V";
    } else if (letterMorse == ".--") {
        letter = "W";
    } else if (letterMorse == "-.-") {
        letter = "X";
    } else if (letterMorse == "-.--") {
        letter = "Y";
    } else if (letterMorse == "--..") {
        letter = "Z";
    } else if (letterMorse == "-----") {
        letter = "0";
    } else if (letterMorse == ".----") {
        letter = "1";
    } else if (letterMorse == "..---") {
        letter = "2";
    } else if (letterMorse == "...--") {
        letter = "3";
    } else if (letterMorse == "....-") {
        letter = "4";
    } else if (letterMorse == ".....") {
        letter = "5";
    } else if (letterMorse == "-....") {
        letter = "6";
    } else if (letterMorse == "--...") {
        letter = "7";
    } else if (letterMorse == "---..") {
        letter = "8";
    } else if (letterMorse == "----.") {
        letter = "9";
    } else if (letterMorse == "") {
        letter = " "; // Morse letter is empty if two slashes follow each other, indicating a change of word
    } else if (letterMorse == "...-.-") {
        letter = "STOP"; // Morse letter corresponding to end of transmission
    }
    return letter;
}



///////////////////////// Transating //////////////////////////

// function to shuffle the DictStruct
void shuffle(DictStruct &donnees) {
  int n = NB_WORDS;

  for (int i = n - 1; i > 0; --i) {
    int j = random(0, i + 1);

    // Swap words using pointers
    char temp[LEN_WORDS];
    strncpy(temp, donnees.mot[i], LEN_WORDS - 1); // Copy and ensure space for null-terminator
    temp[LEN_WORDS - 1] = '\0'; // Null-terminate the copied string
    strncpy(donnees.mot[i], donnees.mot[j], LEN_WORDS - 1); // Copy and ensure space for null-terminator
    donnees.mot[i][LEN_WORDS - 1] = '\0'; // Null-terminate the copied string
    strncpy(donnees.mot[j], temp, LEN_WORDS - 1); // Copy and ensure space for null-terminator
    donnees.mot[j][LEN_WORDS - 1] = '\0'; // Null-terminate the copied string

    // Swap IDs
    int tempID = donnees.id[i];
    donnees.id[i] = donnees.id[j];
    donnees.id[j] = tempID;
  }
}

int levenshtein(const char *s1, const char *s2) {
  unsigned int s1len, s2len, x, y, lastdiag, olddiag;
  s1len = strlen(s1);
  s2len = strlen(s2);
  unsigned int column[s1len + 1];
  for (y = 1; y <= s1len; y++)
    column[y] = y;
  for (x = 1; x <= s2len; x++) {
    column[0] = x;
    for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
      olddiag = column[y];
      column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
      lastdiag = olddiag;
    }
  }
  return (column[s1len]);
}

int levenshtein(const String& str1, const String& str2) {
  return levenshtein(str1.c_str(), str2.c_str());
}


String translate(String word_to_translate){
  int id = -1 ;
  int sending_id = -1;


 // iteration in the initial list to find the word to translate
  for (int i = 0; i < NB_WORDS; ++i) {
    int dist = levenshtein(word_to_translate, init_list.mot[i]);
    if (dist <= 1) { // Check if the distance is 0 or 1 (assuming words are similar)
      id = init_list.id[i];
      break;
    }
  }

  if (id == -1 ){
    return ("error");
  }

  else {
    for (int j = 0; j < NB_WORDS; ++j){
      if (id == translated_list.id[j]){
        sending_id = translated_list.id[(j+1) % (NB_WORDS)]; // if the words is at the end of the list, the "next" word will be the first of the list
        addlog('Translation'+ translated_list.mot[(j+1) % (NB_WORDS)])
        break;
      }
    }

    for (int k = 0; k < NB_WORDS; ++k){
      if (sending_id == init_list.id[k]){
      return init_list.mot[k];
      }
    }

    return ("error");
  }
}



