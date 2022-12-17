/**
 * This file simulate connection between raspberry pi client and server
 */
import { RSA_X931_PADDING } from "constants";
import { WebSocket } from "ws";
import {spawn} from "child_process";


// the id of the raspberry pi client
const remote_device_id = "rasp_123";
var socket;

// var related to connection with pc
var connected = false;
var connected_to_PC = false;
var associated_PC = null;

// var related to python script
const python_executer = spawn("python", ["python/test.py"]);


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
    // test python 
    var data = JSON.stringify([1,2,3,4,5]);
    python_executer.stdin.write(data);
    // End data write
    python_executer.stdin.end();
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
      setInterval(send_wind, 1000);
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
  var lat = Math.random()*100;
  var lon = Math.random()*10;
  if(!connected || !connected_to_PC){return;}
  socket.send(JSON.stringify({
    type: "send_data",
    data_type: "GPS",
    data : {"alt" : rand_nb(), "lat" : 48.715861, "lon" : 2.211261, "speed" : rand_nb(), "sat" : rand_nb() }
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

function send_wind(){
  if(!connected || !connected_to_PC){return;}
  socket.send(JSON.stringify({
    type: "send_data",
    data_type: "WIND",
    data : {"wind_direction" : rand_nb(), "wind_speed" : rand_nb()}
  }));
}

// a test function 
function rand_nb(){
  return Math.floor(Math.random() * 100);
}

// for python output
python_executer.stdout.on('data', (data) => {
  console.log("call python ");
  console.log(String.fromCharCode.apply(null, data));
});

connection_main();