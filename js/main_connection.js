const socket = new WebSocket("ws://localhost:3000");
var client_id = "";


socket.addEventListener("open", () => {
  // send a message to the server
  socket.send(JSON.stringify({
    type: "hello",
  }));
});

// receive a message from the server
socket.addEventListener("message", ({ data }) => {
  const packet = JSON.parse(data);
  if (("client_id" in packet) && (client_id == "")){
    client_id = packet["client_id"];
    console.log(client_id);
  }
  console.log(packet);
});

document.addEventListener("DOMContentLoaded", function(event) { 
  document.getElementById("form_test").addEventListener('submit', function(e){
    e.preventDefault();
    const name = document.getElementById("fname").value;
    socket.send(JSON.stringify({
      type : "data",
      client_id : client_id,
      content : name
    }));
    return false;
  });  
});


