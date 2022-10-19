// i added this const because WebSocket didn't work inside function

const WebSocketGlobal = WebSocket;

window.addEventListener('load', main);

var connected = false;
var socket;


// connect to the server 
function connect_server(){
    var host = document.getElementById("server_name").value;
    var client_id = document.getElementById("client_id").value;
    var remote_device_id = document.getElementById("remote_device_id").value;

    socket = connect_to_server(host, client_id, remote_device_id, process_server_answer);
}


// process answer from the server 
function process_server_answer(data){
    console.log(data);
}



function main(){
    document.getElementById("connect_button").addEventListener("click", connect_server);
}