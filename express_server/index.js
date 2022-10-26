import { WebSocketServer } from "ws";
import {json_answer_connection, json_answer_remote_device_status} from "./json_format.js";

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
        // send if remote_device is connected
        socket.send(json_answer_remote_device_status(
          is_remote_device_connected(server, socket.remote_device_id), remote_device_id));
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
    // update that remote device is connected 
    remote_device_connected(server, remote_device_id);
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
    remote_device_disconnected(server, socket.remote_device_id);
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
 * @param {*} remote_device_id : the id of the remote_device
 */
function remote_device_connected(server, remote_device_id){
  server.remote_devices_connected_list.add(remote_device_id);
  // search for the PC bind to that remote_device
  for(let client of server.clients){
    if ((client.client_type == PC_type) && (client.remote_device_id == remote_device_id)){
      client.send(json_answer_remote_device_status(true, remote_device_id));
    }
  }
}


function remote_device_disconnected(server, remote_device_id){
  server.remote_devices_connected_list.delete(remote_device_id);
  // search for the PC bind to that remote_device
  for(let client of server.clients){
    if ((client.client_type == PC_type) && (client.remote_device_id == remote_device_id)){
      client.send(json_answer_remote_device_status(false, remote_device_id));
    }
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