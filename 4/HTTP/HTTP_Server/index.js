const express = require('express')
const fs = require('fs');
const fetch = require("node-fetch");

const app = express()
const port = 3000


app.listen(port, () => {
    console.log(`HTTP server running at http://localhost:${port}`)
});

app.get('/', (req, res) => {
    const rfid = req.query.RFID;
    fetch("http://192.168.1.114:5000/" + "?RFID=" + rfid)
      .then(response => {
	response.text().then(function (text) {
          res.status(200).send(text);
	});
      })
      .catch(error => {
        res.status(404).send("Not found!");
      });
});
