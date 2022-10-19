import { WebSocketServer } from "ws";

const server = new WebSocketServer({ port: 3000 });

const devices = ["pc_123", "rasp_123"];

console.log("Server listening at 3000");

server.on("connection", (socket) => {
  
  // receive a message from the client
  socket.on("message", (data) => {analyse_message(socket, data);});

  socket.on("close", () => {
    console.log("connection closed id = " + socket.id);

  });

});


function analyse_message(socket, data){
  console.log("analayse");
  data = JSON.parse(data);
  console.log(data);
  var type = data["type"];
  if (type == "connect"){
    deal_connection(socket, data);
  }
}


function deal_connection(socket, data){
  var client_id = data["client_id"];
  var remote_device_id = data["remote_device_id"];

  if((devices.includes(client_id) ) && (devices.includes(remote_device_id))){
    socket.send(JSON.stringify({
      type: "connect",
      answer : "accepted"
    }));
    socket.id = client_id;
    socket.remote_device_id = remote_device_id;
    console.log("accepted");
  } else{
    socket.send(JSON.stringify({
      type: "connect",
      answer : "rejected"
    }));
    console.log("rejected");
    socket.close();
  }
}