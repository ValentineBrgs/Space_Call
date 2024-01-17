////////////// Constants ///////////////////////////////////////////////////////////
// Global 
#define NB_WORDS 7 // 7 words
#define LEN_WORDS 11 // 10 char max
const int ledPin = 13; // LED pin
const unsigned long dotDuration = 250; // Point duration in milliseconds

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c))) // Simple function to find the min between 3 varibles

///////////// emission variables ////////////////////////////////////////////////////
unsigned long lastTime_Led = 0; //date on which led was switched on or off
unsigned long duration_Led = 0; // Length of time LED must remain on or off
String MessageToSend = ""; // plain text message to send
String MessageToSendMorse = ""; // morse code message to be sent with ";" morse code character separator and "/" letter separator
char MorseCharacterToSend = ' '; // morse code to be sent
int index_MorseCharacterToSend = -1; // morse code index to send

/////////// reception variables  ///////////////////////////////////////////////////
const int LDRPin = A0;
int threshold = 180; // Set this value according to the ambient brightness.
String receivedMessage = "";
unsigned long lastTime1 = 0; // date on which a signal light came on
unsigned long lastTime0 = 0; // date when a signal light went out
unsigned long dotDuration = 250; //Point duration in milliseconds
int signalPinState = LOW; // Received light signal status

 
/////////// Translation structures //////////////////////////////////////////////////////////////
struct DictStruct {
  int id[NB_WORDS];
  char mot[NB_WORDS][LEN_WORDS]; 
};

DictStruct init_list;
DictStruct translated_list;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// End of setting variables and structures ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Setup
void setup(){

    Serial.begin(9600); //

    // initalisation of emission 
    pinMode(ledPin, OUTPUT);

    // initialisation or reception
    initLDR();

    // initialisation of translation
    for (int i = 0; i < NB_WORDS; ++i) {  // fill dictionary
      init_list.id[i] = i + 1; // Id from 1 to NB_words
      translated_list.id[i] = i + 1; // Id from 1 to NB_words
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

      // Print the two lists #debug
      for (int i = 0; i < NB_WORDS; ++i){
        Serial.println(translated_list.mot[i]);
        Serial.println(translated_list.id[i]);

        }
      for (int i = 0; i < NB_WORDS; ++i){
          Serial.println(init_list.mot[i]);
          Serial.println(init_list.id[i]);

      }
    }



void loop() {

  // When transmit signal recived
    unsigned long currentTime = millis();

    // The LED flashes at the appropriate moment
    if (currentTime - lastTime_Led > duration_Led) {
        MorseCharacterToSend = Next_MorseCharacterToSend();
        switch_Led_State(ledPin, MorseCharacterToSend);
        duration_Led = Next_duration_Led(MorseCharacterToSend);
        lastTime_Led = currentTime;
    }

    // See if you have a message to send
    if (currentTime - lastTime_Process > Periode_Process) {
        // see if you've finished transmitting the previous message
        if (MessageToSendMorse == "") {
            delay(5000);
            MessageToSend = "1 to 2 Hello";
            MessageToSendMorse = PlainToMorse(MessageToSend);
            MessageToSend = "";
        }
    }


  // When listen signal recived
    int lightValue = analogRead(LDRPin);
    unsigned long currentTime = millis();

    // If an UP edge is detected
    if (lightValue > threshold and signalPinState == LOW) {
        LDRrisingEdge(currentTime);
    }

    //  If an DOWN edge is detected
    if (lightValue < threshold and signalPinState == HIGH) {
        LDRfallingEdge(currentTime);
    }

    if (EndMessage(receivedMessage, 7) == "/...-.-") {
        //Large pause corresponding to the end of the message
        receivedMessage += "/";
        Serial.println(""); // DEBUG
        Serial.print("message recu : ");
        Serial.println(receivedMessage); // DEBUG
        decodePhrase(receivedMessage);
        receivedMessage = "";
    }

  // When translate signal recived
  










  String to_translate;
  String word_to_send;

  int j = random(0, NB_WORDS);
  to_translate = init_list.mot[j];

  // Simulate errors during reception:
  if (to_translate.length() > 0) {
    int error_type = random(0, 3); // Randomly select an error type: 0 for modification, 1 for addition, 2 for deletion

    if (error_type == 0) {
      // Change a letter
      int pos = random(0, to_translate.length()); // Pick a random position in the string
      char new_char = 'a' + random(0, 26); // Change the character to a random lowercase letter
      to_translate.setCharAt(pos, new_char);
    } else if (error_type == 1) {
      // Add a letter
      int pos = random(0, to_translate.length() + 1); // Pick a random position in the string (may include the end)
      char new_char = 'a' + random(0, 26); // Generate a random lowercase letter
      to_translate = to_translate.substring(0, pos) + new_char + to_translate.substring(pos);
    } else if (error_type == 2 && to_translate.length() > 1) {
      // Delete a letter
      int pos = random(0, to_translate.length()); // Pick a random position in the string
      to_translate = to_translate.substring(0, pos) + to_translate.substring(pos + 1);
    }
  }

  Serial.println("Received word: " + to_translate);

  word_to_send = translate(to_translate);

  Serial.println("Translated word: " + word_to_send);
  delay(5000);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Usefull Functions  /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////// Translation functions //////////////////////////////////////////
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


// Translation
String translate(String word_to_translate){
  int id = -1 ;
  int sending_id = -1;


 // iteration in the initial list to find the word to translate
  for (int i = 0; i < NB_WORDS; ++i) {
    int dist = levenshtein(word_to_translate, init_list.mot[i]);
    if (dist <= 1) { // Check if the distance is 0 or 1 (assuming words are similar)
      id = init_list.id[i];
      Serial.println(id);
      break;
    }
  }

  if (id == -1 ){
    Serial.println("Error word not found");
    return ("error");
  }

  else {
    for (int j = 0; j < NB_WORDS; ++j){
      if (id == translated_list.id[j]){
        Serial.println((j+1 )% NB_WORDS);
        sending_id = translated_list.id[(j+1) % (NB_WORDS)]; // if the words is at the end of the list, the "next" word will be the first of the list
        Serial.println(sending_id);
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
///////////////////////////////////////// End of translation functions ///////////////////////////////////////////////

