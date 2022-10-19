
/* connect to server 
 this function send basic information to server, 
 and bind the answer to the action function
*/
 function connect_to_server(host, client_id, remote_device_id, action){
  socket = new WebSocketGlobal(host);

  // send information to the server when connection is opened
  socket.addEventListener("open", () => {
    socket.send(JSON.stringify({
      type: "connect",
      client_id : client_id,
      remote_device_id : remote_device_id
    }));
  });

  // receive a message from the server
  socket.addEventListener("message", ({ data }) => {
    action(data);
  });

  return socket;
}

/*
  send message of type command to server 
*/

function send_to_server(socket, data){
  socket.send(JSON.stringify({
    type : "command",
    command : data
  }));
}
