const express = require("express");
const fs = require("fs");
const axios = require("axios");

const mainServerAddress = "http://localhost:8000/";

const app = express();
const port = 5000;

let tempThreshold, lightThreshold = undefined;

app.use(express.urlencoded({extended: true}));

app.listen(port, () => {
	console.log(`Local server listening at http://localhost:${port}`);
});

app.post("/sensor", async (req, res) => {
	const authorization = req.headers.authorization;
	const data = req.query;
	try {
		if (data.ID === "sensor-0") {
			if (Number.parseInt(data.SENSOR_VALUE) > tempThreshold)
				await turnOffFan();
		} else if (data.ID === "sensor-1") {
			if (Number.parseInt(data.SENSOR_VALUE) > lightThreshold)
				await turnOffLight();
		}
		const response = await axios.post(mainServerAddress + "thing/data", data, {
			headers: {
				authorization: authorization
			},
			params: data
		});
		if (response.data.token) {
			res.status(200).json({token: response.data.token});
			return;
		}
	} catch (e) {
		console.log("Error in request to main server!");
	}
	res.status(201).send("Thank you!");
});

app.post("/sensor/threshold", (req, res) => {
	const {temp, light} = req.query;
	if (!!temp)
		tempThreshold = Number.parseInt(temp);
	if (!!light)
		lightThreshold = Number.parseInt(light);
	res.status(200).json({msg: "Success!"});
});

app.get("/sensor", async (req, res) => {
	let {address, ID} = req.query;
	if (!address && !ID) {
		res.status(400).json({msg: "Bad parameter!"});
		return;
	}
	if (!address) {
		const map = readMap();
		if (map[ID])
			address = map[ID];
		else {
			res.status(400).json({msg: "Something went wrong"});
			return;
		}
	}
	try {
		const response = await axios.get(address);
		res.status(200).json({data: response.data});
	} catch (e) {
		console.log(e);
		res.status(400).json({msg: "Something went wrong"});
	}
});

app.post("/actuator", async (req, res) => {
	let {address, ID, request} = req.query;
	if (!address && !ID) {
		res.status(400).json({msg: "Bad parameter!"});
		return;
	}
	if (!address) {
		const map = readMap();
		if (map[ID])
			address = map[ID];
		else {
			res.status(400).json({msg: "Something went wrong"});
			return;
		}
	}
	try {
		const response = await axios.post(address, {request}, {
			params: {request}
		});
		res.status(200).json({data: response.data});
	} catch (e) {
		res.status(400).json({msg: "Something went wrong"});
	}
});

function readMap() {
	try {
		const data = fs.readFileSync("address-ID.json");
		return JSON.parse(data.toString());
	} catch (e) {
		console.error("Error reading from dataset!");
		return {};
	}
}

async function turnOffFan() {
	const map = readMap();
	const address = map["actuator-0"];
	const request = false.toString();
	try {
		const response = await axios.post(address, {request}, {
			params: {request}
		});
	} catch (e) {
		console.log("Error in turning off the fan");
	}
}

async function turnOffLight() {
	const map = readMap();
	const address = map["actuator-1"];
	const request = false.toString();
	try {
		const response = await axios.post(address, {request}, {
			params: {request}
		});
	} catch (e) {
		console.log("Error in turning off the light");
	}
}
