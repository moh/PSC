import { RSA_X931_PADDING } from "constants";
import { WebSocket } from "ws";
import * as readline from "readline";


const socket = new WebSocket("ws://localhost:3000");
//const readline = require("readline");

socket.addEventListener("open", () => {
  // send a message to the server
  socket.send(JSON.stringify({
    type: "connect",
    client_id : "PC_1",
    remote_device_id : "ras_1"
  }));



});

// receive a message from the server
socket.addEventListener("message", ({ data }) => {
  const packet = JSON.parse(data);

  console.log(packet);
});

