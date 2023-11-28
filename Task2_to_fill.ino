//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// 	Wiring   ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/* ARDUINO UNO
 *
 ** 5V --------------- Photoresistance
 ** A0 --------------- Photoresistance + R220Ohms
 ** Gnd   ------------ R220Ohms
 *
 */

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  Code  /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

const int LDRPin = A0;
int threshold = 180; // Set this value according to the ambient brightness.
String receivedMessage = "";
unsigned long lastTime1 = 0; // date on which a signal light came on
unsigned long lastTime0 = 0; // date when a signal light went out
unsigned long dotDuration = 250; //Point duration in milliseconds
int signalPinState = LOW; // Received light signal status

void setup()
{
	Serial.begin(9600);	
            initLDR();
}

void loop()
{
	int lightValue = analogRead(LDRPin);
	unsigned long currentTime = millis();

	// If an UP edge is detected
	if (/*condition*/) {
    	/* call the right function*/
	}

	//  If an DOWN edge is detected
	if (/*condition*/) {
    	/* call the right function*/
	}

  // Processing the end of a message
	if (EndMessage(receivedMessage, 7) == "/...-.-") {
    	//Large pause corresponding to the end of the message
    	receivedMessage += "/";
    	Serial.println(""); // DEBUG
    	Serial.print("message recu : ");
    	Serial.println(receivedMessage); // DEBUG
    	decodePhrase(receivedMessage);
    	receivedMessage = "";
	}

} // end Loop

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Useful functions  /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Function for calibrating photoresistors according to ambient brightness
void initLDR()
{
	int lightValue = analogRead(LDRPin);
	int lightValueMin = lightValue;
	int lightValueMax = lightValue;
	int n = 0;
	float lightValueMean = lightValue;
	float lightValueStandartDeviation = 0;
	unsigned long startingTime;
	Serial.println("Photoresistor calibration ..."); // DEBUG
	startingTime = millis();
	// We look at the ambient brightness over a period of time
	while ((millis() - startingTime) < 5000) {
    	lightValue = analogRead(LDRPin);
    	if (lightValue < lightValueMin) {
        	lightValueMin = lightValue;
    	}
    	if (lightValue > lightValueMax) {
        	lightValueMax = lightValue;
    	}
    	lightValueMean = (n * lightValueMean + lightValue) / (n + 1); // Average calculation
    	lightValueStandartDeviation = (lightValueStandartDeviation * n + (lightValueMean - lightValue) * (lightValueMean - lightValue)) / (n + 1); // Variance calculation
    	n += 1;
    	delay(50);
	}
	threshold = lightValueMean + 20;
	// DEBUG
	Serial.println("Calibration Done!");
	Serial.print("Luminosite min = ");
	Serial.println(lightValueMin);
	Serial.print("Luminosite Max = ");
	Serial.println(lightValueMax);
	Serial.print("Luminosite Mean = ");
	Serial.println(lightValueMean);
	Serial.print("Variance : ");
	Serial.println(lightValueStandartDeviation);
	Serial.print("treshold = ");
	Serial.println(threshold);
	Serial.println("");
}


// Function returning the last n characters of a String
String EndMessage(String message, int n)
{
	/*your code here*/
}

// Function called on detection of a luminous UP edge
void LDRrisingEdge(unsigned long currentTime)
{
	unsigned long duration = currentTime - lastTime0;
	signalPinState = HIGH;
	lastTime1 = currentTime;

	/*your code here*/

}

// Function called on detection of a luminous DOWN edge
void LDRfallingEdge(unsigned long currentTime)
{
	signalPinState = LOW;
	lastTime0 = currentTime;
	unsigned long duration = currentTime - lastTime1;

	/*your code here*/
	
}

// decodes a Morse code sentence into a clear one
void decodePhrase(String morseCode)
{
	const int n = morseCode.length();
	String Message = "";
	String lettremorse = "";

	 /*your code here*/
}

// decodes a Morse code letter of the form .-- into an alphanumeric letter
String decodeMorse(String lettremorse)
{
	String lettre = "?";

	/*your code here*/
  
}
