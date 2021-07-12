const express = require("express");
const fs = require("fs");
const axios = require("axios");

const localServerAddress = "http://localhost:5000/sensor";
const dataSendInterval = 10 * 1000;

const app = express();
const port = 3001;

const setVal = undefined;

app.use(express.urlencoded({extended: true}));

app.listen(port, () => {
	console.log(`Sensor-1 working at http://localhost:${port}`);
});

/* API to acquire current data */
app.get("/", (req, res) => {
	res.status(200).json({
		SENSOR_VALUE: generateRandomData().toString(),
		...readDataSet()
	});
});


/* Sending data to server periodically */
setInterval(async () => {
	try {
		const data = readDataSet();
		data.SENSOR_VALUE = generateRandomData();
		const response = await axios.request({
			url: localServerAddress,
			method: "POST",
			params: data,
			headers: {
				authorization: data.token ? `Bearer ${data.token}` : undefined
			}
		});
		if (response.status === 200)
			if (response.data.token) {
				const token = response.data.token;
				delete data.SENSOR_VALUE;
				data.token = token
				writeDataSet(data);
			}
	} catch (e) {
		console.error("Error connecting to local server");
	}
}, dataSendInterval);

/* Generating random values */
function generateRandomData(min = 0, max = 50) {
	let val = min + Math.random() * max;
	const hour = new Date().getHours();
	if (hour < 18 && hour > 6)
		val += max;
	return Math.ceil(val);
}

function readDataSet() {
	try {
		const data = fs.readFileSync("dataset.json");
		return JSON.parse(data);
	} catch (e) {
		console.error("Error reading from dataset!");
		return {};
	}
}

function writeDataSet(data) {
	if(!data)
		return;
	fs.writeFile("dataset.json", JSON.stringify(data), err => {
		// Checking for errors
		if (err) console.error("Error saving dataset!");
		console.info("Done writing"); // Success
	});
}
