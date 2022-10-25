/**
 * This file simulate connection between raspberry pi client and server
 */
import { RSA_X931_PADDING } from "constants";
import { WebSocket } from "ws";


// the id of the raspberry pi client
const remote_device_id = "rasp_123";
var socket;

var connected = false;

function connection_main(){
  console.log("trying to connect ...");
  socket = new WebSocket("ws://localhost:3000");

  console.log("open ... ");
  socket.addEventListener("open", () => {
    // send a message to the server
    socket.send(JSON.stringify({
      type: "connect",
      remote_device_id : remote_device_id,
      client_type : "RASP"
    }));
  });
  // receive a message from the server
  socket.addEventListener("message", ({ data }) => {
    const packet = JSON.parse(data);
    if((data["type"] == "connect") && (data["answer"] == "accepted")){
      connected = true;
      console.log("connected to server");
    }
  });

  socket.onerror = function (error){
    console.log("test");
    if(!connected){
      console.log("error ... ");
      setTimeout(connection_main, 1000);
    }
  }
}


connection_main();
console.log("Hello world");
