const express = require("express");
const WebSocket = require("ws");
const bodyParser = require("body-parser");
const cors = require("cors");
const path = require("path");

const app = express();
const port = 3000;

app.use(cors());
app.use(bodyParser.json());

// Serve static files (HTML, CSS, JS)
app.use(express.static(path.join(__dirname, "public")));

const wss = new WebSocket.Server({ port: 3500 });

let currentAngle = 90;

wss.on("connection", (ws) => {
    console.log("ESP32 Connected");

    ws.on("message", (message) => {
        console.log(`Received from ESP32: ${message}`);
    });

    ws.on("close", () => {
        console.log("ESP32 Disconnected");
    });

    ws.send(`angle-${currentAngle}`);
});

app.post("/set-angle", (req, res) => {
    const { angle } = req.body;

    if (typeof angle !== "number" || angle < 0 || angle > 180) {
        return res.status(400).json({ error: "Invalid angle. Must be between 0 and 180." });
    }

    currentAngle = angle;

    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(`angle-${currentAngle}`);
        }
    });

    console.log(`Updated angle: ${currentAngle}`);
    res.json({ message: `Angle set to ${currentAngle}` });
});

// Serve index.html
app.get("/", (req, res) => {
    res.sendFile(path.join(__dirname, "index.html"));
});

app.listen(port, () => {
    console.log(`Express server running on http://localhost:${port}`);
});
