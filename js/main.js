// i added this const because WebSocket didn't work inside function

const WebSocketGlobal = WebSocket;

window.addEventListener('load', main);

var connected = false;
var socket;


// connect to the server 
function connect_server(){
    if(!check_network_forum_field()){
        show_network_error("Fields are empty");
        return;
    }
    var host = document.getElementById("server_name").value;
    var client_id = document.getElementById("client_id").value;
    var remote_device_id = document.getElementById("remote_device_id").value;
    if(!connected){
        socket = connect_to_server(host, client_id, remote_device_id,
             process_server_answer, server_connection_closed, server_connection_error);
        
    }
}


// process answer from the server 
function process_server_answer(data){
    data = JSON.parse(data);
    console.log(data);
    if (data["type"] == "connect"){
        if (data["answer"] == "accepted"){
            change_connection_state(true);
        } else{
            change_connection_state(false);
            show_network_error(data["message"]);
        }
    }
}


// change the connection status depending on server
function change_connection_state(state){
    if (state){
        connected = true;
        hide_network_error();
        document.getElementById("connect_button").disabled = true ;
        document.getElementById("connection_state").innerHTML = "Connected to Server";
    } else{
        connected = false;
        document.getElementById("connect_button").disabled = false ;
        document.getElementById("connection_state").innerHTML = "Not connected to server";
    }
}


// function to be used when the connection with the server is closed
function server_connection_closed(){
    if (!connected){
        return 
    }
    change_connection_state(false);
    show_network_error("Connection interrupted");
}

// function server_connection_error 
function server_connection_error(error){
    show_network_error("Can't connect to server");
}

// check if form fields are not empty
function check_network_forum_field(){
    l = document.querySelectorAll(".input_block input");
    for(var i = 0; i < l.length; i++){
        if(l[i].value == ""){
            return false;
        }
    }
    return true;
}

// Connection error fields
function show_network_error(msg){
    document.getElementById("connection_error").innerHTML = msg;
    document.getElementById("connection_error").style.display = "block";
}

function hide_network_error(){
    document.getElementById("connection_error").innerHTML = "";
    document.getElementById("connection_error").style.display = "none";
}


function main(){
    document.getElementById("connect_button").addEventListener("click", connect_server);

}