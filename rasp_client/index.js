/**
 * This file simulate connection between raspberry pi client and server
 */
import { RSA_X931_PADDING } from "constants";
import { WebSocket } from "ws";
//import {spawn} from "child_process";
import {PythonShell} from "python-shell";

// the id of the raspberry pi client
const remote_device_id = "rasp_123";
const server_address = "ws://localhost:3000";
var socket;

// var related to connection with pc
var connected = false;
var connected_to_PC = false;
var associated_PC = null;

// var related to python script
const python_gps = new PythonShell("python/gps.py");
const python_servos = new PythonShell("python/servo.py")


function connection_main(){
  console.log("trying to connect ...");
  socket = new WebSocket(server_address);

  
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
    // print data 
    console.log("data =   ");
    console.log(data);
    if((data["type"] == "connect") && (data["answer"] == "accepted")){
      connected = true;
      console.log("connected to server");
      
      //setInterval(send_gps, 500);
      //setInterval(send_servo, 1000);
      setInterval(send_wind, 1000);
    }
      if (data["type"] == "PC_presence") {
          if (data["answer"] == true) {
              connected_to_PC = true;
          } else {
              connected_to_PC = false;
          }
      }
      else if (data["type"] == "command") {
          python_servos.send(JSON.stringify(data["data"]));
          
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

/**
 * Send gps data to PC 
 * data : gps data given by python file
 * */
function send_gps(data) {
    console.log("Hello gps");
    if (!connected || !connected_to_PC) { return; }
    socket.send(JSON.stringify({
        type: "send_data",
        data_type: "GPS",
        data: JSON.parse(data)
    }));
}

/**
 * Send servo information to PC
 * data : servo data given by python file
 * */
function send_servo(data) {
    console.log("HELLOOO ");
    console.log(data);
    // parse data from python
    
    if (!connected || !connected_to_PC) { return; }
    
    socket.send(JSON.stringify({
        type: "send_data",
        data_type: "SERVO",
        data: JSON.parse(data)
    }));
    
}

/*
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

*/

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

python_gps.send("HII gps");
// for python output
python_gps.on('message', (data) => { console.log(data); });
    //send_gps(data);});
python_servos.on('message', (data) => { console.log(data); send_servo(data); });

connection_main();
