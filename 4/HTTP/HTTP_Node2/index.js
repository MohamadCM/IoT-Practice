const express = require('express');
const fs = require('fs');

const app = express();
const port = 5000;


app.listen(port, () => {
    console.log(`Node 2 is listening at http://localhost:${port}`);
});

app.get('/', (req, res) => {
    const rfid = req.query.RFID;
    let dataset = fs.readFileSync('dataset.json');
    let datasetParsed = JSON.parse(dataset);
    for (const data of datasetParsed) {
        if (rfid && rfid.trim() === data.RFID) {
            res.status(200).send(`temperature=${data.temp},water=${data.water}`);
            return;
        }
    }
    res.status(404).send("Not found!");
})
