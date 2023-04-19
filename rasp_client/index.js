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
const python_servos = new PythonShell("python/servo.py");
const python_arduino_data = new PythonShell("python/arduino.py");
const python_navigation = new PythonShell("python/navigation.py");

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

      else if (data["type"] == "navigation"){
        console.log("navigation received ");
        navigation_enabled = !navigation_enabled;

        // send current navigation state to the client
        socket.send(JSON.stringify({
          type: "send_data",
          data_type: "navigation_status",
          data: navigation_enabled
        }));

      }

      else if (data["type"] == "target"){
        console.log("TARGET ------------------- ");
        console.log(JSON.stringify(data));
        python_navigation.send(JSON.stringify(data));
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


/**
 * send the data from the arduino to PC
 * @param {*} data 
 */
function send_arduino_data(data){
    console.log("send data from arduino ");

    if (!connected || !connected_to_PC) { return; }
    
    socket.send(JSON.stringify({
        type: "send_data",
        data_type: "*",
        data: JSON.parse(data)
    }));
}


/**
 * data produced by navigation algorithm
 * @param {*} data data of servos
 */
function navigation_command(data){
  python_servos.send(data);
}


//send_gps(data);});
python_servos.on('message', (data) => {
  if (connected_to_PC){ 
    // console.log("Servos"); 
    send_servo(data); 
  }
});


// for python output
python_gps.on('message', (data) => {
  if (connected_to_PC){
    // console.log("Meteo GPS"); 
    send_gps(data); 
  }
});


python_arduino_data.on('message', (data) => {
  if (connected_to_PC){
    send_arduino_data(data);
  }
});


python_navigation.on('message', (data) => {
  if (connected_to_PC){
    console.log("navigation : ");
    console.log(data);
    // test speed
    speed = JSON.parse(data)["speed"];
    //
    navigation_command(data);
  }
});


connection_main();
