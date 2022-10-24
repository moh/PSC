
/* connect to server 
 this function send basic information to server, 
 and bind the answer to the action function, 
 and when connection is closed bind it to action_closed
*/
 function connect_to_server(host, client_id, remote_device_id, action, action_closed, action_error){
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

  socket.addEventListener("close", (event) => {
    action_closed();
  });

  socket.onerror = function (error){
    action_error(error);
  }

  return socket;
}


function send_to_server(socket, data){
  socket.send(JSON.stringify({
    type : "command",
    command : data
  }));
}
