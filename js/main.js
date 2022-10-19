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
    data = JSON.parse(data);
    console.log(data);
    if (data["type"] == "connect"){
        change_connection_state(data["answer"] == "accepted");
    }
}



function change_connection_state(state){
    if (state){
        console.log("here");
        document.getElementById("connection_state").innerHTML = "Connected to Server";
    } else{
        document.getElementById("connection_state").innerHTML = "Not connected to server";
    }
}



function main(){
    document.getElementById("connect_button").addEventListener("click", connect_server);
}