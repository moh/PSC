import { RSA_X931_PADDING } from "constants";
import { WebSocket } from "ws";
import * as readline from "readline";


const socket = new WebSocket("ws://localhost:3000");
//const readline = require("readline");

socket.addEventListener("open", () => {
  // send a message to the server
  socket.send(JSON.stringify({
    type: "hello from client",
    content: [ 3, "4" ]
  }));

  while(true){
    const rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
    });

    rl.question("Message : ", function (answer){
      socket.send(JSON.stringify({
        content: answer
      }));

      console.log(answer);
    });
  }

});

// receive a message from the server
socket.addEventListener("message", ({ data }) => {
  const packet = JSON.parse(data);

  console.log(packet);
});

