//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// 	Wiring   ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/* ARDUINO UNO
 *
 ** pin13 --------------- R=220Ohms  --- Led+
 ** Gnd   --------------- Led-
 *
 */

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  Code  /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

////////////// Constants ///////////////////////////////////////////////////////////
const int ledPin = 13; // LED pin
const unsigned long dotDuration = 250; // Point duration in milliseconds

///////////// emission variables ////////////////////////////////////////////////////
unsigned long lastTime_Led = 0; //date on which led was switched on or off
unsigned long duration_Led = 0; // Length of time LED must remain on or off
String MessageToSend = ""; // plain text message to send
String MessageToSendMorse = ""; // morse code message to be sent with ";" morse code character separator and "/" letter separator
char MorseCharacterToSend = ' '; // morse code to be sent
int index_MorseCharacterToSend = -1; // morse code index to send

/////////////  Process Variables ////////////////////////////////////////////////////
unsigned long lastTime_Process = 0; // date when we last processed our data
const unsigned long Periode_Process = 1000; // Time in milliseconds between two processes

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(9600);
	pinMode(ledPin, OUTPUT);
}

void loop()
{
	// put your main code here, to run repeatedly
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

} // end Loop

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Usefull Functions  /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Function that changes the status LED according to the Morse code to be sent
void switch_Led_State(int ledPin, char c)
{
	if (c == '.') {
    	digitalWrite(ledPin, HIGH);
	}
	if (c == '-') {
    	digitalWrite(ledPin, HIGH);
	}
	if (c == ';') {
    	digitalWrite(ledPin, LOW);
	}
	if (c == '/') {
    	digitalWrite(ledPin, LOW);
	}
	if (c == ' ') {
    	digitalWrite(ledPin, LOW);
	}
}

// Function that returns the next Morse code to be sent or an empty code if all codes have been sent.
char Next_MorseCharacterToSend()
{
	index_MorseCharacterToSend += 1;
	//check whether you've reached the end of the morse characters or not
	if (index_MorseCharacterToSend >= MessageToSendMorse.length()) {
    	MessageToSendMorse = ""; // we've sent the whole message so we can reset it
    	index_MorseCharacterToSend = -1;
    	return (' ');
	}
	return (MessageToSendMorse.charAt(index_MorseCharacterToSend));
}

// Function which returns the time in milliseconds during which the LED must remain off or on according to the Morse code given as an argument.
unsigned long Next_duration_Led(char c)
{
	if (c == '.') {
    	return (dotDuration);
	}
	if (c == '-') {
    	return (dotDuration * 3);
	}
	if (c == ';') {
    	return (dotDuration);
	} // the LED is switched off for 1 beat between each Morse code character
	if (c == '/') {
    	return (dotDuration * 3);
	} // the LED is switched off for 3 beats when the word is changed
	if (c == ' ') {
    	return (dotDuration);
	} // If there are no more characters to transmit, we wait 1 time before checking if there are any new characters.
}

String PlainToMorse(String plainText)
{
	String morseCode = "";

	for (int i = 0; i < plainText.length(); i++) {
    	char c = toupper(plainText.charAt(i));

    	switch (c) {
    	case 'A':
        	morseCode += ".;-/";
        	break;
    	case 'B':
        	morseCode += "-;.;.;./";
        	break;
    	case 'C':
        	morseCode += "-;.;-;./";
        	break;
    	case 'D':
        	morseCode += "-;.;./";
        	break;
    	case 'E':
        	morseCode += "./";
        	break;
    	case 'F':
        	morseCode += ".;.;-;./";
        	break;
    	case 'G':
        	morseCode += "-;-;./";
        	break;
    	case 'H':
        	morseCode += ".;.;.;./";
        	break;
    	case 'I':
        	morseCode += ".;./";
        	break;
    	case 'J':
        	morseCode += ".;-;-;-/";
        	break;
    	case 'K':
        	morseCode += "-;.;-/";
        	break;
    	case 'L':
        	morseCode += ".;-;.;./";
        	break;
    	case 'M':
        	morseCode += "-;-/";
        	break;
    	case 'N':
        	morseCode += "-;./";
        	break;
    	case 'O':
        	morseCode += "-;-;-/";
        	break;
    	case 'P':
        	morseCode += ".;-;-;./";
        	break;
    	case 'Q':
        	morseCode += "-;-;.;-/";
        	break;
    	case 'R':
        	morseCode += ".;-;./";
        	break;
    	case 'S':
        	morseCode += ".;.;./";
        	break;
    	case 'T':
        	morseCode += "-/";
        	break;
    	case 'U':
        	morseCode += ".;.;-/";
        	break;
    	case 'V':
        	morseCode += ".;.;.;-/";
        	break;
    	case 'W':
        	morseCode += ".;-;-/";
        	break;
    	case 'X':
        	morseCode += "-;.;.;-/";
        	break;
    	case 'Y':
        	morseCode += "-;.;-;-/";
        	break;
    	case 'Z':
        	morseCode += "-;-;.;./";
        	break;
    	case '0':
        	morseCode += "-;-;-;-;-/";
        	break;
    	case '1':
        	morseCode += ".;-;-;-;-/";
        	break;
    	case '2':
        	morseCode += ".;.;-;-;-/";
        	break;
    	case '3':
        	morseCode += ".;.;.;-;-/";
        	break;
    	case '4':
        	morseCode += ".;.;.;.;-/";
        	break;
    	case '5':
        	morseCode += ".;.;.;.;./";
        	break;
    	case '6':
        	morseCode += "-;.;.;.;./";
        	break;
    	case '7':
        	morseCode += "-;-;.;.;./";
        	break;
    	case '8':
        	morseCode += "-;-;-;.;./";
        	break;
    	case '9':
        	morseCode += "-;-;-;-;./";
        	break;
    	case ' ':
        	morseCode += "/"; // if there's a space, add "/" to create a "//" between words (6 beats)
        	break;
    	default:
        	// If the character is not a letter or number, ignore it or treat it as you wish.
        	break;
    	}

	} // end for
	// we add the ,Morse code corresponding to the end of the instruction
	morseCode += "/.;.;.;-;.;-/";
	return morseCode;
}
