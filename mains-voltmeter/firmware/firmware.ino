

//=================================================================================================
// LIBRARIES
//=================================================================================================

#include <Math.h>
#include "FirebaseESP32.h"  // https://github.com/mobizt/Firebase-ESP32
#include "arduinoFFT.h"     // https://github.com/kosme/arduinoFFT


//=================================================================================================
// DEFINITIONS
//=================================================================================================

// Definitions for Wifi and Firebase
#define WIFI_SSID           "TP-LINK_NANDO"                 // owner's network ssid
#define WIFI_PASSWORD       "Garfield01"                 // owner's network password
#define FIREBASE_HOST       "esp32-mains-voltage-monitor.firebaseio.com"                 // firebase's project site
#define FIREBASE_AUTH       "lfpr8JAyeZBn0xafZv2gvmdsKAnA9XJP98cKILah"                 // secret (40 chars) (deprecated...)

// Definitions for ESP32 pinout
#define ESP32_VP            36                    // ESP32 analog read pin VP

// Definitions for sine wave sampling
#define SAMPLE_SIZE         512                   // nº of samples
#define SAMPLE_FREQ         2048                  // in Hz
#define SAMPLE_PERIOD       1000000/SAMPLE_FREQ   // in us
#define SIGNAL_FREQ         60                    // in Hz (sine wave freq.)
#define NUM_HARMONICS       8                     // nº of harmonics for THD calculation

// Definition of period between sine wave measurements
#define SLEEP_TIME          10                    // in min, sleep (measurement) interval


//=================================================================================================
// GLOBAL VARIABLES
//=================================================================================================

// ESP32 variables
hw_timer_t *timer = NULL;           // interrupt counter
RTC_DATA_ATTR int bootCount = 0;    // boot counter

// Firebase data container
FirebaseData firebaseData;

// Varibles used to store sensor measurements (voltage)
unsigned int sensorCount = 0;
double       sensorData[SAMPLE_SIZE];

// Variables used to calculate FFT and THD
double       vReal[SAMPLE_SIZE];
double       vImag[SAMPLE_SIZE];
unsigned int harmonicIndices[NUM_HARMONICS];
double       harmonicPeaks[NUM_HARMONICS];
double       THD = 0;
arduinoFFT   FFT = arduinoFFT();


//=================================================================================================
// AUXILIAR FUNCTIONS
//=================================================================================================

// TIMER function used to sample sine wave data
void cbTimer(){
    sensorData[sensorCount] = (double) analogRead(ESP32_VP);
    vReal[sensorCount] = sensorData[sensorCount];
    vImag[sensorCount] = 0.0;
    if (++sensorCount == SAMPLE_SIZE) {
        stopTimer();
    }
}

// TIMER function used to start the timer
void startTimer(){
    timer = timerBegin(0, 80, true);                // timer 0 - prescale 80   - prog. count
    timerAttachInterrupt(timer, &cbTimer, true);    // *timer  - cb fcn addr   - gen. int.
    timerAlarmWrite(timer, SAMPLE_PERIOD, true);    // *timer  - nº count (us) - auto repeat 
    timerAlarmEnable(timer);                        // enables timer
}

// TIMER function used to stop the timer
void stopTimer(){
    timerEnd(timer);
    timer = NULL; 
}

// Calculated the FFT from the sampled sinewave
void updateFFT(){
    FFT.DCRemoval(vReal, SAMPLE_SIZE);
    FFT.Windowing(vReal, SAMPLE_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLE_SIZE, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLE_SIZE);
    vReal[0] = 0;
}

// Updates the harmonic peaks found in the FFT (used for THD calculation)
void updateHarmonicPeaks() {
    for (int i=0; i<NUM_HARMONICS; i++) {
        harmonicIndices[i] = (int)round((SIGNAL_FREQ*(i+1)*SAMPLE_SIZE)/((double)SAMPLE_FREQ));
        harmonicPeaks[i]   = vReal[harmonicIndices[i]];
    }
}

// Calculates the THD in %
void updateTHD() {
    THD = 0;
    for (int i=1; i<NUM_HARMONICS; i++) {
      THD = THD + pow(harmonicPeaks[i], 2);
    }
    THD = 100*(sqrt(THD)/harmonicPeaks[0]);
}


//=================================================================================================
// SYSTEM SETUP
//=================================================================================================

void setup(){
    
    // Initializes serial monitor
    delay(1000);
    Serial.begin(115200);

    // Connect to a WiFi network
    delay(1000);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Connect to Firebase
    delay(1000);
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
  
    //Set Sleep Timer
    bootCount += 1; // register boot counts between sleeps
    esp_sleep_enable_timer_wakeup(SLEEP_TIME*60*1000000);
    Serial.print("\n# Boot Count = ");
    Serial.println(bootCount);
        
    // Enable sampling timer
    delay(1000);
    sensorCount = 0;
    startTimer();
}


//=================================================================================================
// SYSTEM MAIN
//=================================================================================================

void loop(){

    Serial.println("Wait for the sampling to finish");
    while (sensorCount != SAMPLE_SIZE) {
        delay(100);
    }

    Serial.println("Calculating FFT, harmonics and THD...");
    updateFFT();
    updateHarmonicPeaks();
    updateTHD();

    Serial.println("Creating Json objects...");
    FirebaseJson json_data;
    FirebaseJson json_fft;
    FirebaseJson json_harmonicPeaks;
    FirebaseJson json_thd;
    FirebaseJson json_composition;
    
    for (int i=0; i<SAMPLE_SIZE; i++) {
        json_data.addInt(String(i), (int)sensorData[i]);
    }
    for(int i=0; i<SAMPLE_SIZE/2; i++) {
        json_fft.addDouble(String(i), vReal[i]);
    }
    for(int i=0; i<NUM_HARMONICS; i++) {
        json_harmonicPeaks.addDouble(String(harmonicIndices[i]), harmonicPeaks[i]);
    }
    json_thd.addDouble("curr_measure", THD);

    json_composition.addJson("sensor", &json_data);
    json_composition.addJson("fft", &json_fft);
    json_composition.addJson("harmonic_peaks", &json_harmonicPeaks);
    json_composition.addJson("thd", &json_thd);

    Serial.println("Sending data to Firebase...");
    if (Firebase.updateNode(firebaseData, "/", json_composition)) {
        //Serial.println(firebaseData.dataPath());
        //Serial.println(firebaseData.pushName());
        //Serial.println(firebaseData.dataPath() + "/"+ firebaseData.pushName());
        Serial.println("Data sent to Firebase without errors!");
    } else {
        Serial.println(firebaseData.errorReason());
    }
    delay(500);
    
    // Puts ESP32 in timed deep sleep mode - Setup() will be called after waking up
    Serial.println("Entering Sleep Mode...");
    esp_deep_sleep_start();
}
