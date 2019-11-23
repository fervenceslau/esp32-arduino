# Mains Voltmeter Remote Monitoring

![Firebase Voltmeter Demo](demo/voltmeter-demo.gif)


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

* Arduino core for the ESP32 (https://github.com/espressif/arduino-esp32)
* Google's Firebase Realtime Database Arduino Library for ESP32 (https://github.com/mobizt/Firebase-ESP32)
* Fast Fourier Transform for Arduino Library (https://github.com/kosme/arduinoFFT)


### Installing




#### Creating a Firebase Realtime Database

Go to https://medium.com/@vibrologic/serverless-iots-with-firebase-realtime-database-and-esp32-2d86eda06ff1 to see the step by step guide with images that are described in this section.

1. Create a new Firebase Project.
2. Create a new Realtime Databse.
3. Set the Database Rules to true (**warning** anyone can alter the database).
4. Get the Database host name and password.
5. Update [firmware](firmware/firmware.ino) with your WiFi and Realtime Database credentials (obtained in the previous step).

```
#define WIFI_SSID           "XXX"                 // owner's network ssid
#define WIFI_PASSWORD       "XXX"                 // owner's network password
#define FIREBASE_HOST       "XXX"                 // firebase's project site
#define FIREBASE_AUTH       "XXX"                 // secret (40 chars) (deprecated...)
```

#### Setting up Firebase Hosting

1. Click on the Cog Icon next to **Project Overview** (Project Settings) and select **Project Configuration**. Under **General** tab, look for **Your Apps** and click the **</>** symbol to add a new web app. Choose an App nickname and check the box below to set up **Firebase Hosting**. Follow the instructions and then click on the last button to go back to the configuration screen.
2. Click the **Config** radio button under **Firebase SDK snippet** to get the firebase configuration script.
3. Update [myapp](firebase/public/myapp.js) with the information obtained in the previous step.

```
const firebaseConfig = {
	apiKey: 			"XXX",
	authDomain: 			"XXX",
	databaseURL: 			"XXX",
	projectId: 			"XXX",
	storageBucket: 			"XXX",
	messagingSenderId: 		"XXX",
	appId: 				"XXX"
};
```

Upload the code to ESP32 board (DoIt used).



End with an example of getting some data out of the system or using it for a little demo

<!--

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

-->

## Built With

* [FirebaseESP32](https://github.com/mobizt/Firebase-ESP32) - Firebase Arduino Library for ESP32
* [arduinoFFT](https://github.com/kosme/arduinoFFT) - Arduino FFT Library
* [Chartjs](https://www.chartjs.org/) - Used to generate the graphs


## Authors

* **Fernando Venceslau Isensee** - *Initial work* - [fervenceslau](https://github.com/fervenceslau/)

## License

<!--- This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details --->
