import { WebSocketServer } from "ws";

const server = new WebSocketServer({ port: 3000 });

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
function analyse_message(socket, data){
  console.log("analayse");
  data = JSON.parse(data);
  console.log(data);
  var type = data["type"];
  if (type == "connect"){
    deal_connection(socket, data);
  }
  // server.clients.forEach(function each(client)
}

// function that accept or reject the connection with client
function deal_connection(socket, data){
  var client_id = data["client_id"];
  var remote_device_id = data["remote_device_id"];
  var socket_type = data["type"];

  if((devices.includes(client_id) ) && (devices.includes(remote_device_id))){
    socket.send(JSON.stringify({
      type: "connect",
      answer : "accepted"
    }));
    socket.client_id = client_id;
    socket.remote_device_id = remote_device_id;
    socket.type = socket_type
    console.log("accepted");
  } else{
    socket.send(JSON.stringify({
      type: "connect",
      answer : "rejected",
      message : "Error in id"
    }));
    console.log("rejected");
    socket.close();
  }
}

// check if remote_device is already connected to a device
// !!
function is_remote_device_busy(remote_device_id){
  return;
}