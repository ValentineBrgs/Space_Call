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
	
	unsigned long currentTime = millis();

	// The LED flashes at the appropriate moment
	if (/* condition */) {
    	/* swich the LED state for the right duration and stock the lastTime_Led */
	}

	// See if you have a message to send
	if (/* condition */) {
    	// see if you've finished transmitting the previous message
    	if (/* condition */) {
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
void switch_Led_State(int ledPin, char c){
  /* your code here*/
}

// Function that returns the next Morse code to be sent or an empty code if all codes have been sent.
char Next_MorseCharacterToSend(){
  /* your code here*/
}

// Function which returns the time in milliseconds during which the LED must remain off or on according to the Morse code given as an argument.
unsigned long Next_duration_Led(char c){
  /* your code here*/
}

String PlainToMorse(String plainText){
  /* your code here*/
}