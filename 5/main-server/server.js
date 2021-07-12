const express = require("express");
const fs = require("fs");
const axios = require("axios");
const jwt = require("jsonwebtoken");

const secret = "IoT-Platform";
const localServerAddress = "http://localhost:5000";

const app = express();
const port = 8000;

app.use(express.urlencoded({extended: true}));

app.listen(port, () => {
	console.log(`Main server working at http://localhost:${port}`);
});
/* Register/Login a new user */
app.post("/user/auth", (req, res) => {
	const userAccessName = req.query.access_name;
	if (userAccessName) {
		const users = readDataSet("users.json");
		for (const user of users) {
			if (user.access_name === userAccessName) {
				const difference = Date.now() - user.last_access;
				if (difference < 5 * 60 * 1000) {
					user.last_access = Date.now();
					writeDataSet("users.json", users);
					res.status(200).json({token: user.token});
					return;
				} else {
					const index = users.indexOf(user);
					users.splice(index, 1);
					break;
				}
			}
		}
		const user = {
			access_name: userAccessName,
			token: jwt.sign({userAccessName}, secret),
			last_access: Date.now()
		};
		users.push(user);
		writeDataSet("users.json", users);
		res.status(200).json({token: user.token});
		return;
	}
	res.status(422).json({msg: "Wrong params!"});
});
/* Register a new thing */
app.post("/thing/register", (req, res) => {
	const token = req.headers.authorization;
	if (!authorization(token.split(" ")[1])) {
		res.status(401).json({msg: "Unauthorized"});
		return;
	}
	const {userAccessName} = jwt.decode(token.split(" ")[1]);
	const {ID, mode, address, entry_code} = req.query;
	if (ID && entry_code && address) {
		const things = readDataSet("things.json");
		for (const thing of things) {
			if (thing.ID === ID) {
				if (!thing.users.includes(userAccessName)) {
					thing.users.push(userAccessName);
					writeDataSet("things.json", things);
				}
				res.status(200).json({msg: "Success!"});
				return;
			}
		}
		things.push({
			ID, mode, address, entry_code,
			users: [userAccessName]
		});
		writeDataSet("things.json", things);
		return;
	}
	res.status(422).json({msg: "Wrong params!"});
});
/* Add data of a thing */
app.post("/thing/data", (req, res) => {
	const token = req.headers.authorization;
	if (!token || !authorization(token.split(" ")[1])) {
		res.status(401).json({msg: "Unauthorized"});
		return;
	}
	const {IDCheck} = jwt.decode(token.split(" ")[1]);
	const {ID, address, entry_code, SENSOR_VALUE} = req.query;
	const auth = authorizeSensor(ID);
	if (!auth) {
		res.status(401).json({msg: "Unauthorized"});
		return;
	}
	const dataset = readDataSet("dataset.json");
	for (const thing of dataset) {
		if (thing.ID === ID) {
			thing.data.push({time: Date.now(), val: SENSOR_VALUE});
			writeDataSet("dataset.json", dataset);
			res.status(200).json(auth);
			return;
		}
	}
	dataset.push({
		ID,
		data: [
			{time: Date.now(), val: SENSOR_VALUE}
		]
	});
	writeDataSet("dataset.json", dataset);
	res.status(200).json(auth);
});
/* Get FULL sensor data */
app.get("/thing/data", (req, res) => {
	const token = req.headers.authorization;
	if (!authorization(token.split(" ")[1])) {
		res.status(401).json({msg: "Unauthorized"});
		return;
	}
	const {userAccessName} = jwt.decode(token.split(" ")[1]);
	const {ID} = req.query;
	if(!ID){
		res.status(422).json({msg: "Wrong params!"});
		return;
	}
	const things = readDataSet("things.json");
	for (const thing of things) {
		if (thing.ID === ID) {
			if (thing.users.includes(userAccessName)) {
				const dataset = readDataSet("dataset.json");
				for (const data of dataset) {
					if (data.ID === ID) {
						res.status(200).json({data: data.data});
						return;
					}
				}
			}
			break;
		}
	}
	res.status(400).json({msg: "Can't retrieve data!"});
});

function readDataSet(file) {
	try {
		const data = fs.readFileSync(file);
		return JSON.parse(data.toString());
	} catch (e) {
		console.error("Error reading from dataset!");
		return {};
	}
}

function writeDataSet(file, data) {
	if(!data)
		return;
	fs.writeFile(file, JSON.stringify(data), err => {
		// Checking for errors
		if (err) console.error("Error saving dataset!");
		console.info("Done writing"); // Success
	});
}

function authorization(token) {
	try {
		const valid = jwt.verify(token, secret);
		return !!valid;
	} catch (e) {
		return false;
	}
}

function authorizeSensor(ID) {
	if (ID) {
		const things = readDataSet("things.json");
		for (const thing of things) {
			if (thing.ID === ID) {
				const difference = Date.now() - thing.last_access;
				if (difference < 5 * 60 * 1000 && thing.token) {
					thing.last_access = Date.now();
					writeDataSet("things.json", things);
				} else {
					thing.token = jwt.sign({
						ID
					}, secret);
					thing.last_access = Date.now();
					writeDataSet("things.json", things);
				}
				return {token: thing.token};
			}
		}
		return false;
	}
	return false;
}
