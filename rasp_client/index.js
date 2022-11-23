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
      setInterval(send_gps, 500);
      setInterval(send_servo, 1000);
    }
    if(data["type"] == "PC_presence"){
      if (data["answer"] == true){
        connected_to_PC = true;
      } else{
        connected_to_PC = false;
      }
    }
  });

  // when connection is closed, try to reconnect 
  socket.on("close", () => {
    console.log("closed ... ");
    console.log(connected);
    connected_to_PC = false;
    // if it was connected 
    if(connected){
      console.log("closed trying  ... ");
      setTimeout(connection_main, 1000);
    }
  });


  socket.onerror = function (error){
    console.log("test");
    connected_to_PC = false;
    if(!connected){
      console.log("error ... ");
      setTimeout(connection_main, 1000);
    }
  }
}


function send_gps(){
  if(!connected || !connected_to_PC){return;}
  socket.send(JSON.stringify({
    type: "send_data",
    data_type: "GPS",
    data : {"alt" : rand_nb(), "lat" : rand_nb(), "lon" : rand_nb(), "speed" : rand_nb(), "sat" : rand_nb() }
  }));
}

function send_servo(){
  if(!connected || !connected_to_PC){return;}
  socket.send(JSON.stringify({
    type: "send_data",
    data_type: "SERVO",
    data : {"servo_1" : rand_nb(), "servo_2" : rand_nb(), "servo_3" : rand_nb()}
  }));
}

// a test function 
function rand_nb(){
  return Math.floor(Math.random() * 100);
}

connection_main();