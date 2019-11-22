// TODO: improved THD calculation
// TODO: adc calibration
// OK - TODO: sleep mode - Deep Sleep Mode with timer

#include <Math.h>
#include "FirebaseESP32.h"
#include "arduinoFFT.h"

// Definitions for Wifi and firebase
#define WIFI_SSID           "TP-LINK_NANDO" // owner's network ssid
#define WIFI_PASSWORD       "Garfield01"    // owner's network password

#define FIREBASE_HOST       "esp32-sensor-teste.firebaseio.com"         // firebase's project site
#define FIREBASE_AUTH       "bQdWPusagBKXRgBs5f9BrroPWb15yI9CfX7XgsU1"  // secret (40 chars) (deprecated...)

// Definitions for sine wave sampling
#define VP                  36              // ESP32 analog read pin VP
#define VN                  39              // ESP32 analog read pin VN
#define SAMPLE_SIZE         512                   // power of 2
#define SAMPLE_FREQ         2048                  // in hz (power of 2 to keep fft's bins in discrete freq. interval)
#define SAMPLE_PERIOD       1000000/SAMPLE_FREQ   // in us
#define SIGNAL_FREQ         60                    // in hz - sine wave
#define NUM_HARMONICS       8

// Measurement delay between thd readings
#define SLEEP_TIME          10                    // in minutes, time between thd measurements = esp32 sleep time


// Global variables
hw_timer_t * timer = NULL;

RTC_DATA_ATTR int bootCount = 0;

FirebaseData firebaseData;

unsigned int sensor_counter = 0;
double sensor_data[SAMPLE_SIZE];
double vReal[SAMPLE_SIZE];
double vImag[SAMPLE_SIZE];
unsigned int harmonicIndices[NUM_HARMONICS];
double harmonicPeaks[NUM_HARMONICS];
double thd = 0;

arduinoFFT FFT = arduinoFFT();


// TIMES functions used to sample sine wave data
void cb_timer(){
    sensor_data[sensor_counter] = (double) analogRead(VN);
    vReal[sensor_counter] = sensor_data[sensor_counter];
    vImag[sensor_counter] = 0.0;
    if (++sensor_counter == SAMPLE_SIZE) {
        stopTimer();
    }
}

void startTimer(){
    timer = timerBegin(0, 80, true);                // timer 0, prescale 80, progressive count
    timerAttachInterrupt(timer, &cb_timer, true);   // hw_timer_t *timer, cb fcn address, gen. int.
    timerAlarmWrite(timer, SAMPLE_PERIOD, true);    // hw_timer_t *timer, nº counts in us, auto-repeat true 
    timerAlarmEnable(timer);                        // enables timer
}

void stopTimer(){
    timerEnd(timer);
    timer = NULL; 
}


void updateFFT(){
    FFT.DCRemoval(vReal, SAMPLE_SIZE);
    FFT.Windowing(vReal, SAMPLE_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLE_SIZE, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLE_SIZE);
    vReal[0] = 0;
}

void updateHarmonicPeaks() {
    for (int i=0; i<NUM_HARMONICS; i++) {
        harmonicIndices[i] = (int)round((SIGNAL_FREQ*(i+1)*SAMPLE_SIZE)/((double)SAMPLE_FREQ));
        harmonicPeaks[i]   = vReal[harmonicIndices[i]];
    }
}

void updateTHD() {
    thd = 0;
    for (int i=1; i<NUM_HARMONICS; i++) {
      thd = thd + pow(harmonicPeaks[i], 2);
    }
    thd = 100*(sqrt(thd)/harmonicPeaks[0]);
}


// System setup
void setup(){
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
    sensor_counter = 0;
    startTimer();
}

void loop(){

    Serial.println("Wait sampling");
    while (sensor_counter != SAMPLE_SIZE) {
        delay(1000);
    }

    Serial.println("Calculating FFT, harmonics and THD...");
    updateFFT();
    updateHarmonicPeaks();
    updateTHD();

    Serial.println("Creating Json object...");
    FirebaseJson json_data;
    FirebaseJson json_fft;
    FirebaseJson json_harmonicPeaks;
    FirebaseJson json_thd;
    FirebaseJson json_composition;
    
    for (int i=0; i<SAMPLE_SIZE; i++) {
        json_data.addInt(String(i), (int)sensor_data[i]);
    }
    for(int i=0; i<SAMPLE_SIZE/2; i++) {
        json_fft.addDouble(String(i), vReal[i]);
    }
    for(int i=0; i<NUM_HARMONICS; i++) {
        json_harmonicPeaks.addDouble(String(harmonicIndices[i]), harmonicPeaks[i]);
    }
    json_thd.addDouble("curr_measure", thd);

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
    
    // Puts ESP32 in deep sleep mode with timer - Setup() will be called after waking up
    Serial.println("Entering Sleep Mode...");
    esp_deep_sleep_start();
}
