import { WebSocketServer } from "ws";
import {json_answer_connection, json_answer_remote_device_status,json_remote_device_associated} from "./json_format.js";

// server intitating 
const server = new WebSocketServer({ port: 3000 });
// the type of the client trying to connect to server
const PC_type = "PC";
const RASP_type = "RASP";

// a list that contains the remote devices that are connected to server
server.remote_devices_connected_list = new Set();

const devices = ["pc_123", "rasp_123", "PC_1", "ras_1"];

console.log("Server listening at 3000");

server.on("connection", (socket) => {
  // receive a message from the client
  socket.on("message", (data) => {analyse_message(socket, data);});

  socket.on("close", () => {
    connection_closed(socket);
  });
});

// analyse the message received from clients
// call specific function depending on the type of message

/**
 * This function is called when server receive data from client
 * @param {*} socket socket resulted from connection with client 
 * @param {*} data data sent by client
 */
function analyse_message(socket, data){
  console.log("analayse");
  data = JSON.parse(data);
  console.log(data);
  var type = data["type"];
  if (type == "connect"){
    deal_connection(socket, data);
  } else if (type == "command" || type == "target" || type == "navigation"){
    send_command(socket, data);
  } else{
    route_message(socket, data);
  }
}

/**
 * This function decides wether to accept or reject the connection with the client,
 * in both case it sends the answer to the client
 * If connection is accepted : 
 *  Answer : {type : "connect", answer : "accepted"} 
 *  it stores the information sent in the socket
 * If connection is rejected : 
 *  Answer : {type : "connect", answer : "rejected", message : "Error ..."} 
 *  it closes the connection after sending an answer
 * @param {*} socket socket resulted from connection with client 
 * @param {*} data data sent by client in JSON format
 */
function deal_connection(socket, data){
  var client_id = data["client_id"];
  var remote_device_id = data["remote_device_id"];
  var socket_type = data["client_type"];

  if((socket_type == PC_type)){
    if ((devices.includes(client_id) ) && (devices.includes(remote_device_id))){   
      if (!is_remote_device_busy(server, remote_device_id)){
        
        socket.send(json_answer_connection(true));
        socket.client_id = client_id;
        socket.remote_device_id = remote_device_id;
        socket.client_type = socket_type;
        socket.remote_device_socket = null;
        
        
        // if remote_device is connected to server then send to the pc and bind sockets
        if (is_remote_device_connected(server, socket.remote_device_id)){
          socket.send(json_answer_remote_device_status(true, remote_device_id));
          // bind the sockets 
          bind_pc_remote_device_sockets(server, socket, socket.remote_device_id);
        } else{
          socket.send(json_answer_remote_device_status(false, remote_device_id));
        }
        

        // check if remote device is associated to a client, and inform remote device if it is the case
        send_remote_device_associated(server, socket, true);

        console.log("accepted");
      } else{
        reject_connection(socket, "Remote device is busy");
      } 
  } else{
    reject_connection(socket, "Error in id");
  }
  }

  if ((socket_type == RASP_type)){
    if (devices.includes(remote_device_id)){
      socket.send(json_answer_connection(true));
      socket.remote_device_id = remote_device_id;
      socket.client_type = socket_type;
      socket.pc_socket = null;
      // update that remote device is connected to PC 
      remote_device_connected(server, socket, remote_device_id);
      // check if remote device is associated to a client, and inform remote device if it is the case
      send_remote_device_associated(server, socket, true);
      
      console.log("accepted");
    } else{
    reject_connection(socket, "Error in id");
    }
  }
}

/**
 * This function reject connection with a client and send him the error
 * @param {*} socket socket used for connection with client 
 * @param {*} error  error to be sent
 */
function reject_connection(socket, error){
  socket.send(json_answer_connection(false, error));
  console.log("rejected");
  socket.close();
}

/**
 * Function called when connection is closed with a socket
 * If the socket is a remote device, 
 * then remove it from list and inform PC that remote device is down
 * @param {*} socket the socket that is down
 */
function connection_closed(socket){
  if(socket.client_type == RASP_type){
    remote_device_disconnected(server, socket, socket.remote_device_id);
  } else if(socket.client_type == PC_type){
    send_remote_device_associated(server, socket, false);
  } 
}

/*
-------------------------
Remote device part 
-------------------------
*/

/**
 * Check if remote_device (raspberry) is already linked to a client (PC)
 * @param {*} server : the server object
 * @param {*} remote_device_id : the id of the remote_device
 * @returns true if remote_device is connected to a PC, false if not
 */
function is_remote_device_busy(server, remote_device_id){
  for(let client of server.clients){
    if ((client.client_type == PC_type) && (client.remote_device_id == remote_device_id)){
      return true;
    }
  }
  return false; 
}


/**
 * function excuted when a remote_device is connected to the server
 * The function send to the PC that is bind to this remote device 
 * in case it is connected to server
 *  
 * @param {*} server : the server object 
 * @param {*} socket : the socket of remote device (RASP)
 * @param {*} remote_device_id : the id of the remote_device
 */
function remote_device_connected(server, remote_device_socket, remote_device_id){
  server.remote_devices_connected_list.add(remote_device_id);
  console.log("Hereee ");
  // search for the PC bind to that remote_device
  for(let client of server.clients){
    if ((client.client_type == PC_type) && (client.remote_device_id == remote_device_id)){
      // store the PC socket in the socket related to remote device bound to it
      remote_device_socket.pc_socket = client;
      // store the remote device socket to the PC socket bound to it
      client.remote_device_socket = remote_device_socket;
      client.send(json_answer_remote_device_status(true, remote_device_id));
    }
  }
}

/**
 * this function is called when a PC is connected to server, 
 * it searches for the remote device socket associated to it and bind the sockets 
 * @param {*} server the server socket object
 * @param {*} pc_socket the socket of client type PC
 * @param {*} remote_device_id the remote device id of rasp connected to pc_socket
 */
function bind_pc_remote_device_sockets(server, pc_socket, remote_device_id){
  for(let client of server.clients){
    if ((client.client_type == RASP_type) && (client.remote_device_id == remote_device_id)){
      pc_socket.remote_device_socket = client;
      client.pc_socket = pc_socket;
      client.send(json_remote_device_associated(true, pc_socket.client_id));
    }
  }
}

/**
 * function excuted when remote device disconnect from the server, 
 * it informs the PC bound to the remote device.
 * 
 * @param {*} server the server socket object
 * @param {*} remote_device_socket remote device socket 
 * @param {*} remote_device_id the id of the remote device 
 */
function remote_device_disconnected(server, remote_device_socket, remote_device_id){
  server.remote_devices_connected_list.delete(remote_device_id);
  // search for the PC bind to that remote_device
  if (remote_device_socket.pc_socket != null){
    remote_device_socket.pc_socket.send(json_answer_remote_device_status(false, remote_device_id));
    // reset the remote device socket of the pc 
    remote_device_socket.pc_socket.remote_device_socket = null; 
  }
}

/**
 * Check if server is connected to server, if it is in the list or not
 * @param {*} server : the server object 
 * @param {*} remote_device_id : the id of remote_device 
 * @returns true if remote device is connected to server
 */
function is_remote_device_connected(server, remote_device_id){
  return server.remote_devices_connected_list.has(remote_device_id);
}

/**
 * This function informs the remote device if it associated to a PC or not
 * @param {*} server : server object
 * @param {*} socket : socket of the client (PC or RASP)
 * @param {*} associated : bool if the remote device is associated to PC or not
 */
function send_remote_device_associated(server, socket, associated){
  // if socket is a PC, then search for remote device (RASP) that is connected to server and associated to this PC
  // then send to the remote device that it is associated to a PC or not (value of associated)
  if (socket.client_type == PC_type){
    if (socket.remote_device_socket != null){
      socket.remote_device_socket.send(json_remote_device_associated(associated, socket.client_id));
    }
  }

  // if socket is a RASP, then search for PC that is connected to server and associated to that remote device
  // then send to the remote device that it is associated to a PC or not (value of associated)
  else if (socket.client_type == RASP_type){
    if (socket.pc_socket != null){
      socket.send(json_remote_device_associated(associated, socket.pc_socket.client_id));
    }  
  }
}

/*
-----------------
part related to sending data 
------------------
*/

/**
 * This function send the command data send from PC to RASP
 * @param {*} socket socket of PC
 * @param {*} data data sent by PC in JSON form
 */
function send_command(socket, data){
  if ((socket.client_type == PC_type) && (socket.remote_device_socket != null)){
    socket.remote_device_socket.send(JSON.stringify(data));
  }
}

/**
 * This function route the message sent from remote device to pc
 * @param {*} socket socket of remote device  
 * @param {*} data data sent by remote device in JSON format
 */
function route_message(socket, data){
  if ((socket.client_type == RASP_type) && (socket.pc_socket != null)){
    socket.pc_socket.send(JSON.stringify(data));
  }
}