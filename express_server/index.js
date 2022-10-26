import { WebSocketServer } from "ws";

// server intitating 
const server = new WebSocketServer({ port: 3000 });
// the type of the client trying to connect to server
const PC_type = "PC";
const RASP_type = "RASP";

// a list that contains the remote devices that are connected to server
server.remote_devices_connected_list = [];

const devices = ["pc_123", "rasp_123", "PC_1", "ras_1"];

console.log("Server listening at 3000");

server.on("connection", (socket) => {
  // receive a message from the client
  socket.on("message", (data) => {analyse_message(socket, data);});

  socket.on("close", () => {
    console.log("connection closed id = " + socket.client_id);
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
        socket.client_type = socket_type
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

/*
-------------------------
Remote device part 
-------------------------
*/

/**
 * Check if remote_device (raspberry) is already linked to a client (PC)
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



function remote_device_connected(server, remote_device_id){
  server.remote_devices_connected_list.push(remote_device_id);
  for(let client of server.clients){
    if ((client.client_type == PC_type) && (client.remote_device_id == remote_device_id)){
      break;
    }
  }
}


/*
------------------------------------------
Part of JSON message to send to client
------------------------------------------
*/

/**
 * This function returns a JSON string that indicate if the connection is accpeted or not
 * @param {*} accepted boolean to indicate if connection is accepted or not
 * @param {*} message message in case if connection is srejected
 * @returns 
 */
function json_answer_connection(accepted, message = ""){
  if(accepted){
    return JSON.stringify({
      type: "connect",
      answer : "accepted"
    })
  } else{
    return JSON.stringify({
      type: "connect",
      answer : "rejected",
      message : message
    })
  }
}