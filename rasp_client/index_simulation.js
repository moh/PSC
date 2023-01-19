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
const python_meteo_gps = new PythonShell("python_simulation/meteo_GPS_simulation.py");
const python_servos = new PythonShell("python_simulation/servos_simulation.py");
const python_imu = new PythonShell("python_simulation/imu_simulation.py");


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

/**
 * Send GPS or wind data
 * @param {*} data 
 * @returns 
 */
function send_meteo_gps(data){
    if(!connected || !connected_to_PC){return;}
    socket.send(JSON.stringify({
      type: "send_data",
      data_type: "*",
      data : JSON.parse(data)
    }));
}


function send_imu(data){
    if(!connected || !connected_to_PC){return;}
    socket.send(JSON.stringify({
      type: "send_data",
      data_type: "IMU",
      data : JSON.parse(data)
    }));
}


// for python output
python_meteo_gps.on('message', (data) => { console.log("Meteo GPS"); send_meteo_gps(data); });
//send_gps(data);});
python_servos.on('message', (data) => { console.log("Servos"); send_servo(data); });
// python_imu.on('message', (data) => {console.log("IMU"); send_imu(data); }); 

connection_main();
