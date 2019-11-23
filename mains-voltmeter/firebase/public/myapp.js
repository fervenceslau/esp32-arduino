
// Firebase Configuration & initialization
const firebaseConfig = {
	apiKey: 			"XXX",
	authDomain: 			"XXX",
	databaseURL: 			"XXX",
	projectId: 			"XXX",
	storageBucket: 			"XXX",
	messagingSenderId: 		"XXX",
	appId: 				"XXX"
};
  
// Initialize Firebase
firebase.initializeApp(firebaseConfig);

// Create sensor chart
var ctxSensor = document.getElementById('chart-sensor').getContext('2d');
var chartSensor = new Chart(ctxSensor, {
	type: 'line',
	data: {
		labels: [], // Array.from(Array(sensorData.length).keys())
		datasets: [{ 
			data: [], // sensorData
			label: "Sensor Data",
			borderColor: "#3e95cd",
			fill: false,
			lineTension: 0
		}]
	},
	options: {
		maintainAspectRatio: false,
		elements: {
			point: {
				radius: 2
			}
		},
		legend: {
			display: false
		},
		title: {
			display: true,
			text: 'Sampled Sine Wave Data'
		},
		responsive: true,
		animation: {
			duration: 0 // general animation time
		},
		hover: {
			animationDuration: 1000 // duration of animations when hovering an item
		},
		//events: [],
		scales: {
			yAxes: [{
				ticks: {
					min: 0,
					max: 4096
				}
			}],
			xAxes: [{
				ticks: {
					maxTicksLimit : 11
				}
			}]
		}
	}
});

// Create FFT chart
var ctxFFT = document.getElementById('chart-fft').getContext('2d');
var chartFFT = new Chart(ctxFFT, {
	type: 'line',
	data: {
		labels: [],
		datasets: [{ 
			data: [],
			label: "FFT",
			borderColor: "#3e95cd",
			fill: false,
			lineTension: 0
		}]
	},
	options: {
		maintainAspectRatio: false,
		elements: {
			point: {
				radius: 2
			}
		},
		legend: {
			display: false
		},
		title: {
			display: true,
			text: 'Fast Fourrier Transform'
		},
		responsive: true,
		animation: {
				duration: 0 // general animation time
		},
		hover: {
				animationDuration: 1000 // duration of animations when hovering an item
		},
		//events: [],
		scales: {
			yAxes: [{
				ticks: {
					min: 0
				}
			}],
			xAxes: [{
				ticks: {
					maxTicksLimit : 17
				}
			}]
		}
	}
});

// Create THD chart
var ctxTHD = document.getElementById('chart-thd').getContext('2d');
var chartTHD = new Chart(ctxTHD, {
	type: 'line',
	data: {
		datasets: [{ 
			data: [],
			label: "THD",
			borderColor: "#3e95cd",
			fill: false,
			lineTension: 0
		}]
	},
	options: {
		maintainAspectRatio: false,
		legend: {
			display: false
		},
		title: {
			display: true,
			text: 'Total Harmonic Distortion (Daily Chart)'
		},
		responsive: true,
		animation: {
			duration: 0 // general animation time
		},
		hover: {
			animationDuration: 1000 // duration of animations when hovering an item
		},
		//events: [],
		scales: {
			yAxes: [{
				ticks: {
					min: 0
				}
			}],
			xAxes: [{
				type: 'time',
				time: {
					unit: 'hour',
					displayFormats: {
						hour: 'HH'
					}
				},
			}]
		}
	}
});




// Update sensor chart based on json data
firebase.database().ref('sensor').on('value', (snapshot) => {
	var sensorData = snapshotValToArray(snapshot);
	updateChartData(chartSensor, Object.keys(sensorData), sensorData, 0);
});

// Update FFT chart based on json data
firebase.database().ref('fft').on('value', function(snapshot) {
	var FFTData = snapshotValToArray(snapshot);
	updateChartData(chartFFT, Object.keys(FFTData), FFTData, 0);
});


const num_thd = 24*(60/10);

firebase.database().ref('timestamped_thd').limitToLast(num_thd).on('value', function(snapshot) {
	var thdData = snapshotValueToArray(snapshot);
	var thdTime = snapshotTimestampToArray(snapshot);
	var thdDataObjArray = [];
	for(let i = 0; i < thdData.length; i++){
		var newDataObj = {
			x: thdTime[i],
			y: thdData[i]
		};
		thdDataObjArray.push(newDataObj);
	}
	updateTimeChartData(chartTHD, thdDataObjArray);
});


//
function updateChartData(chart, label, data, datasetIndex) {
    chart.data.labels = label;
    chart.data.datasets[datasetIndex].data = data;
    chart.update();
}

//
function updateTimeChartData(chart, data) {
	var minDate = new Date();
	minDate.setHours(0);
	minDate.setMinutes(0);
	minDate.setSeconds(0);
	
	var maxDate = new Date();
	maxDate.setHours(23);
	maxDate.setMinutes(59);
	maxDate.setSeconds(59);


	chart.data.datasets[0].data = data;
	chart.config.options.scales.xAxes[0].time.min = minDate
	chart.config.options.scales.xAxes[0].time.max = maxDate
    chart.update();
}

// converts snapshot value to array (to plot using chart.js)
function snapshotValToArray(snapshot) {
    var returnArr = [];
    snapshot.forEach(function(childSnapshot) {
        var item = childSnapshot.val();
        item.key = childSnapshot.key;
        returnArr.push(item);
    });
    return returnArr;
};

// converts snapshot timestamp to array (to plot using chart.js)
function snapshotTimestampToArray(snapshot) {
    var returnArr = [];
    snapshot.forEach(function(childSnapshot) {
        var item = childSnapshot.val();
        returnArr.push(moment(item.timestamp).format('YYYY-MM-DD HH:mm:ss'));
    });
    return returnArr;
};

// converts snapshot timestamp to array (to plot using chart.js)
function snapshotValueToArray(snapshot) {
    var returnArr = [];
    snapshot.forEach(function(childSnapshot) {
        var item = childSnapshot.val();
		returnArr.push(item.value);
    });
    return returnArr;
};
