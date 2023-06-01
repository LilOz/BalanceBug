const express = require("express");
const app = express();
var bodyParser = require("body-parser");
var htmlParser = require("node-html-parser");
const cors = require("cors");
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.json());

var acc; // Declare acc as a global variable
var gyr; // Declare gyr as a global variable
app.use(cors({ origin: "*" }));
app.use(
  cors({
    methods: ["GET", "POST", "DELETE", "UPDATE", "PUT", "PATCH"],
  })
);

app.post("/acc", (req, res) => {
  const now = new Date();
  const hours = now.getHours().toString().padStart(2, "0");
  const minutes = now.getMinutes().toString().padStart(2, "0");
  const seconds = now.getSeconds().toString().padStart(2, "0");
  console.log(`${hours}:${minutes}:${seconds}`);
  acc = JSON.stringify(req.body); // Assign value to the global variable acc
  console.log("got acc" + JSON.stringify(req.body));
  res.writeHead(200, { "Content-Type": "text/plain" });
  res.end("Got Acc");
});

app.post("/gyr", (req, res) => {
  const now = new Date();
  const hours = now.getHours().toString().padStart(2, "0");
  const minutes = now.getMinutes().toString().padStart(2, "0");
  const seconds = now.getSeconds().toString().padStart(2, "0");
  console.log(`${hours}:${minutes}:${seconds}`);
  gyr = JSON.stringify(req.body); // Assign value to the global variable gyr
  console.log("got gyr" + JSON.stringify(req.body));
  res.writeHead(200, { "Content-Type": "text/plain" });
  res.end("Got gyr");
});

let htmlContent = `<!DOCTYPE html>
<html>
<head>
  <title>My Website</title>
</head>
<body>
  <h1>!!!!</h1>
  <p>-Junaid</p>
</body>
</html>`;

app.get("/", (req, res) => {
  const now = new Date();
  const hours = now.getHours().toString().padStart(2, "0");
  const minutes = now.getMinutes().toString().padStart(2, "0");
  const seconds = now.getSeconds().toString().padStart(2, "0");
  console.log(`${hours}:${minutes}:${seconds}`);
  console.log("home");
  res.writeHead(200, { "Content-Type": "text/html" });
  res.end(htmlContent);
});

app.get("/acc", (req, res) => {
  var jsonData = acc;
  res.writeHead(200, { "Content-Type": "application/json" });
  res.end(JSON.stringify(jsonData));
});

app.get("/gyr", (req, res) => {
  var jsonData = gyr;
  res.writeHead(200, { "Content-Type": "application/json" });
  res.end(JSON.stringify(jsonData));
});

app.listen(3000, "0.0.0.0", () => {
  console.log("started");
});
