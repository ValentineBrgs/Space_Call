#include <AsyncDelay.h>
#include <SoftWire.h> // Library for controlling a second I2C bus as master
#include <Wire.h>

///////////////////////I2C Communication with Robinson///////////////////////
#define Arduino_address 0x16 // Address of Arduino on Robinson's I2C bus

///////////////////////I2C Communication with Schools///////////////////////
const int sdaPin = A0; // Change this
const int sclPin = A1; // Change this
SoftWire sw(sdaPin, sclPin);
char swTxBuffer[32]; // These buffers must be at least as large as the largest read or write you perform.
char swRxBuffer[32]; // These buffers must be at least as large as the largest read or write you perform.
int I2CAddressSchool[] = {1, 2, 3}; // List of the I2C addresses of the school board
int SchoolNumber = 3; // Number of school boards
int firstSchool = -1; // Index of the first school to speak during the round
//
AsyncDelay delayGetLog;

AsyncDelay delayStartNewRound; // Delay between the end of the last round and the beginning of the new one
AsyncDelay delayEndRound;     // Delay between the beginning and the end of the round
unsigned long lastTime_EndRound = 0;
unsigned long lastTime_NewRound = 0;
const int T_delayEndRound = 1 * 60 * 1000;     // Delay between the beginning and the end of the round
const int T_delayStartNewRound = 1 * 10 * 1000; // Delay between the end of the last round and the beginning of the new one
bool StartNewRound = true; // Indicates whether we are allowed to start a new round or not
bool EndRound = false;      // Indicates whether we are allowed to end the round or not

void setup(){
    Serial.begin(9600);

    // initSchoolI2C(); // Initialization of the school I2C bus
    sw.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
    sw.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
    sw.setDelay_us(5);
    sw.setTimeout(1000);
    sw.begin();
    initRobinsonI2C(); // Initialization of the Robinson I2C bus
    // Initialization of delays
    delayGetLog.start(2000, AsyncDelay::MILLIS);
    Serial.println("Master connected");
    delay(1000);
}

void loop(){
    // Regularly check for logs to record
    if (delayGetLog.isExpired()){
        for (int i = 0; i < SchoolNumber; i++){
            getLog(I2CAddressSchool[i]);
        }
        delayGetLog.repeat();
    }

    // Check if we can start a new round
    if (millis() - lastTime_EndRound > T_delayStartNewRound && StartNewRound){
        lastTime_NewRound = millis();
        StartNewRound = false;
        EndRound = true;

        // Start the loop

        // Initialization
        for (int i = 0; i < SchoolNumber; i++){
            sendMessage(I2CAddressSchool[i], 0x01); // Init LDR
            Serial.println("Request Arduino " + String(I2CAddressSchool[i]) + " Init LDR"); // DEBUG
        }

        // Loop that activates listening for school s+1 and transmission for school s
        for (int s = firstSchool; firstSchool + SchoolNumber;){
            // Listen order
            firstSchool = (s + 2) % SchoolNumber;
            sendMessage(I2CAddressSchool[firstSchool], 0x03);
            Serial.println("Request Arduino " + String(I2CAddressSchool[firstSchool]) + " Listen"); // DEBUG

            // Transmit order
            firstSchool = (s + 1) % SchoolNumber;
            sendMessage(I2CAddressSchool[firstSchool], 0x02);
            Serial.println("Request Arduino " + String(I2CAddressSchool[firstSchool]) + " to say a word"); // DEBUG

            delay(6000); // Wait for 6 seconds before going to the next school
        }
    }

    // Check if we should end the round
    if (millis() - lastTime_NewRound > T_delayEndRound && EndRound){
        lastTime_EndRound = millis();
        StartNewRound = true;
        EndRound = false;
        Serial.println("End of the round"); // Debug
        addLog("End of the round");
        for (int i = 0; i < SchoolNumber; i++)
        {
            sendMessage(I2CAddressSchool[i], 0x03); // Disable Morse communication
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Useful Functions /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// School I2C Communication /////////////////////////////////

void initSchoolI2C(){
    sw.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
    sw.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
    sw.setDelay_us(5);
    sw.setTimeout(1000);
    sw.begin();
}

// Function that adds logs
void addLog(String Log){
    String Log_ = getDate() + String(" ") + Log;
}

// Function that retrieves logs from the Arduino
void getLog(int ArduinoNo){
    String Log = "Start";
    while (Log != ""){
        Log = "";
        int numBytes = sw.requestFrom(ArduinoNo, (uint8_t)32);
        for (int i = 0; i < numBytes; ++i){
            char c = sw.read(); // Receive a byte as character
            if (c == char('!')){
                break; // If we have the end-of-string byte, stop the transmission
            }
            Log += String(c);
        }
        // If we received something, record it
        if (Log != ""){
            String Log_ = "From Arduino " + String(ArduinoNo) + " : " + Log;
            Serial.println("Log received : " + Log_); // DEBUG
            addLog(Log_);
        }
    }
}

// Function that returns the time since the Arduino started in the format [hh:mm:ss]
String getDate(){
    unsigned long currentMillis = millis(); // Get the time elapsed since the Arduino board started
    int seconds = (currentMillis / 1000) % 60;           // Calculate seconds
    int minutes = (currentMillis / (1000 * 60)) % 60;     // Calculate minutes
    int hours = (currentMillis / (1000 * 60 * 60)) % 24;  // Calculate hours

    char buffer[12]; // Big enough to contain "[hh:mm:ss]\0"
    sprintf(buffer, "[%02d:%02d:%02d]", hours, minutes, seconds);
    return (String(buffer));
}

// Send a message to the specified Arduino via I2C
void sendMessage(int ArduinoNo, int msg){
    /*  0x01 -> initLDR
     *  0x02 -> send word
     *  0x03 -> execute Listen function
     */
     */
    sw.beginTransmission(ArduinoNo);
    sw.write(byte(msg));
    sw.endTransmission();
}

////////////////////////////////// Robinson I2C Communication /////////////////////////////////

void initRobinsonI2C(){
    Wire.begin(Arduino_adress);
    Wire.onRequest(I2COnRequestEvent);
}

void I2COnRequestEvent(){
   // not usefull in your case 

}

void I2CReceiveEvent(int n){
    // not usefull in your case 
}
