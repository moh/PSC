/**
 * This file simulate connection between raspberry pi client and server
 */
import { RSA_X931_PADDING } from "constants";
import { WebSocket } from "ws";


// the id of the raspberry pi client
const remote_device_id = "rasp_123";
var socket;

var connected = false;
var connected_to_PC = false;
var associated_PC = null;



function connection_main(){
  console.log("trying to connect ...");
  socket = new WebSocket("ws://localhost:3000");

  
  socket.addEventListener("open", () => {
    // send a message to the server
    console.log("open ... ");
    socket.send(JSON.stringify({
      type: "connect",
      remote_device_id : remote_device_id,
      client_type : "RASP"
    }));
  });

  // receive a message from the server
  socket.addEventListener("message", ({ data }) => {
    data = JSON.parse(data);
    console.log("data =   ");
    console.log(data);
    if((data["type"] == "connect") && (data["answer"] == "accepted")){
      connected = true;
      console.log("connected to server");
    }
  });

  // when connection is closed, try to reconnect 
  socket.on("close", () => {
    console.log("closed ... ");
    console.log(connected);
    if(connected){
      console.log("closed trying  ... ");
      setTimeout(connection_main, 1000);
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