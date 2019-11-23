# Mains Voltmeter Remote Monitoring

![Firebase Voltmeter Demo](demo/voltmeter-demo.gif)


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

* Arduino core for the ESP32 (https://github.com/espressif/arduino-esp32)
* Google's Firebase Realtime Database Arduino Library for ESP32 (https://github.com/mobizt/Firebase-ESP32)
* Fast Fourier Transform for Arduino Library (https://github.com/kosme/arduinoFFT)


### Installing

https://medium.com/@vibrologic/serverless-iots-with-firebase-realtime-database-and-esp32-2d86eda06ff1


#### Creating Realtime Database

1. Create a new firebase project
2. Create a new Realtime Databse
3. Set the Database Rules to true - anyone can alter the database, next authentication
4. Get the Database host name and password
5. Update [firmware](firmware/firmware.ino) with your WiFi and Database credentials obtained in the previous step.

```
#define WIFI_SSID           "XXX"                 // owner's network ssid
#define WIFI_PASSWORD       "XXX"                 // owner's network password
#define FIREBASE_HOST       "XXX"                 // firebase's project site
#define FIREBASE_AUTH       "XXX"                 // secret (40 chars) (deprecated...)
```

#### Setting Firebase Hosting

1. Click on the Cog Icon next to **Project Overview** (Project Settings) and select **Project Configuration**. Under **General** tab, look for **Your Apps** and click the **</>** symbol to add a new web app. Choose an App nickname and check the box below to set up **Firebase Hosting**. Follow the instructions and then click on the last button to go to the console.
2. Now that we are back to the previous screen, click the "Config" radio button under "Firebase SDK snippet" to get the firebase configuration script.
3. Update [jscript](firebase/public/myapp.js) with the information obtained in the previous step.

```
const firebaseConfig = {
	apiKey: 				"XXX",
	authDomain: 			"XXX",
	databaseURL: 			"XXX",
	projectId: 				"XXX",
	storageBucket: 			"XXX",
	messagingSenderId: 		"XXX",
	appId: 					"XXX"
};
```

Upload the code to ESP32 board (DoIt used).



End with an example of getting some data out of the system or using it for a little demo

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

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc
