
// Buid 2 structures 
#define NB_WORDS 7 // 7 words
#define LEN_WORDS 11 // 10 char max

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

// complete here the structure that contains two lists .id and .mot that will contain ids and words.
struct DictStruct {
 ...
};

DictStruct init_list;
DictStruct translated_list;


void setup(){
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("Start");

  for (int i = 0; i < NB_WORDS; ++i) {

    // fill ids for each list
    init_list.id[i] = ... ; // Id from 1 to 7
    translated_list.id[i] = ... ; // Id from 1 to 7
    // Assign words to lists
    switch (i) {
      case 0:
        strcpy();// add the word from initial list in init_list
        strcpy(); // add the corresponding translation in translated_list
        break;
      case 1:
        strcpy();// add the word from initial list in init_list
        strcpy(); // add the corresponding translation in translated_list
        break;
      case 2:
        strcpy();// add the word from initial list in init_list
        strcpy(); // add the corresponding translation in translated_list
        break;
      case 3:
        strcpy();// add the word from initial list in init_list
        strcpy(); // add the corresponding translation in translated_list
        break;
      case 4:
        strcpy();// add the word from initial list in init_list
        strcpy(); // add the corresponding translation in translated_list
        break;
      case 5:
        strcpy();// add the word from initial list in init_list
        strcpy(); // add the corresponding translation in translated_list
        break;
      case 6:
        strcpy();// add the word from initial list in init_list
        strcpy(); // add the corresponding translation in translated_list
        break;
    }
  }

  // add here : shuffle the translated list 


}

void loop() {
  Serial.println("Start translation");

  String to_translate;
  String word_to_send;


//////////////////////// Code to select a random word from init_list and add errors///////////////////////

  int j = random(0, NB_WORDS);
  to_translate = init_list.mot[j];


////////////////// Comment this paragraph if you don't want to add errors ///////////////////////////////////
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
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.println("Received word: " + to_translate);

  word_to_send = translate(to_translate);

  Serial.println("Translated word: " + word_to_send);
  delay(5000);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Usefull Functions  /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// function to shuffle the DictStruct
void shuffle(DictStruct &donnees) {
  ...
}

////////////////// optionnal function to calculate the distance between 2 words ////////////////////////////
int levenshtein(const char *s1, const char *s2) {
  ....
}

int levenshtein(const String& str1, const String& str2) {
  return levenshtein(str1.c_str(), str2.c_str());
}

// function to translate
String translate(String word_to_translate){
  int id = -1 ;
  int sending_id = -1;


 // iteration in the initial list to find the word to translate
  for (int i = 0; i < NB_WORDS; ++i) {
    ...
  }

  // Handle error if word has not been found in initial_list
  if (id == -1 ){
    
  }
    // if word has been found
  else {
    // iteration on translated_list to find the position of the word in translated_list. Find the id of the next word in the translated_list
    for (int j = 0; j < NB_WORDS; ++j){
      if (id == translated_list.id[j]){
        ...
      }
    }

    // iteration on the initial_list to find the word corresponding to the sending_id. return this word
    for (int k = 0; k < NB_WORDS; ++k){
      if (sending_id == init_list.id[k]){
      ...
      }
    }

    return ("error");
  }
}


