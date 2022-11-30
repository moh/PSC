// i added this const because WebSocket didn't work inside function

const WebSocketGlobal = WebSocket;

window.addEventListener('load', main);

// true if client is connected (PC) to server
var connected = false;
// true if remote device is connected and associated to PC
var remote_device_connected = false;
// the socket that result from the connection
var socket;


/**
 * It is called when button "connect" is clicked
 * This function verify that the network fields (server, Your id, remote device id) are not empty,
 * then connect to the server and send those informations,
 * we pass the required functions to connect_to_server,
 * process_server_answer : function called when client receives answer from server
 * server_connection_closed : function called when connection is closed with server
 * server_connection_error : function called when error is raised in connecting to server
 *  */ 
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


/**
 * Process the answer received from server, 
 * and call the function depending on the type of the answer
 * possible type : connect, command
 * @param {*} data : data received from the server 
 */
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
    // answer about remote device presence 
    else if(data["type"] == "remote_device_presence"){
        if (data["answer"]){
            change_remote_device_connection(true, data["remote_device_id"]);
        } else{
            change_remote_device_connection(false, data["remote_device_id"]);
        }
    }
    // answer about GPS data
    // type : "send_data", data_type
    else if(data["type"] == "send_data"){
        if (data["data_type"] == "GPS"){
            update_gps_data(data["data"]);
        }

        if (data["data_type"] == "SERVO"){
            update_servo_data(data["data"]);
        }

        if (data["data_type"] == "WIND"){
            update_wind_direction(data["data"]);
        }

        if (data["data_type"] == "*"){
            update_gps_data(data["data"]);
            update_servo_data(data["data"]);
            update_gps_data(data["data"]);
        }

    }
}

/**
 * This function is called when the connection with the server is closed
 * if the client was already connected then the function change the state of connection,
 * In this case, connection is interrupted, the function show the error
 */
 function server_connection_closed(){
    if (connected){
        change_connection_state(false);
        show_network_error("Connection interrupted");
    }
}

/**
 * This function is called when an error occured on socket connection
 * @param {*} error : error received from socket
 */ 
function server_connection_error(error){
    show_network_error("Can't connect to server");
}


// change the connection status depending on server
/**
 * Change fields related to connection when there is a connection or not.
 * if client is connected, (state = true), then hide error part and show "connected to server"
 * if client is not connected, (state = false), sow "Not connected to server"
 * @param {*} state boolean that indicates if client is connected to server or not
 */
function change_connection_state(state){
    if (state){
        connected = true;
        hide_network_error();
        document.getElementById("connect_button").disabled = true ;
        document.getElementById("connect_button").classList.remove("connect_button_activated");
        document.getElementById("connection_state").classList.add("connected");
        document.getElementById("connection_state").innerHTML = "Connected to Server";
    } else{
        connected = false;
        document.getElementById("connect_button").disabled = false ;
        document.getElementById("connect_button").classList.add("connect_button_activated");
        document.getElementById("connection_state").classList.remove("connected");
        document.getElementById("connection_state").innerHTML = "Not connected to server";
    }
}


/**
 * Check if some information are missing from network forum field
 * @returns boolean indicate if forum_field is empty or note
 */
function check_network_forum_field(){
    l = document.querySelectorAll(".input_block input");
    for(var i = 0; i < l.length; i++){
        if(l[i].value == ""){
            return false;
        }
    }
    return true;
}

/**
 * This function show the error description "msg" in the error field
 * @param {*} msg : string that describe the error 
 */
function show_network_error(msg){
    document.getElementById("connection_error").innerHTML = msg;
    document.getElementById("connection_error").style.display = "block";
}

/**
 * This function hide the error field
 */
function hide_network_error(){
    document.getElementById("connection_error").innerHTML = "";
    document.getElementById("connection_error").style.display = "none";
}


/**
 * 
 * @param {*} status boolean, true if remote device is connected
 * @param {*} remote_device_id id of the remote device 
 */
function change_remote_device_connection(status, remote_device_id){
    remote_device_connected = status;
    if(status){
        document.getElementById("remote_device_status").innerHTML = "Remote device (" + remote_device_id + ") : <div class='rd_connected'>Connected</div>";
    } else{
        document.getElementById("remote_device_status").innerHTML = "Remote device (" + remote_device_id + ") : <div class='rd_disconnected'>Not connected</div>";
    }
}

// -----------------------
// About GPS 
// -----------------------

/**
 * display GPS information 
 * @param {*} data a dictionnary that contains the informations 
 */
function update_gps_data(data){
    for(info in data){
       el = document.querySelector('[data-gps="' + info + '"]');
       if(el != null){
        el.innerHTML = data[info];
       } 
    }
}

// -----------------------
// update servo and wind direction data
// -----------------------

/**
 * update servo angle from data
 * @param {*} data 
 */
function update_servo_data(data){
    for(info in data){
        if(info.includes("servo")){
            rotate_servo(info, data[info]);
        }
    }
}

/**
 * update wind direction from data
 * @param {*} data 
 */
function update_wind_direction(data){
    for(info in data){
        if(info == "wind_direction"){
            rotate_wind_direction(data[info]);
        }
    }
}


// Test function 
function test_send(){
    socket.send(JSON.stringify({
        type : "command",
        data : { servo_1 : 120, servo_2 : 90, servo_3 : 100}
    }));
    console.log("Testing command ... ");
}

function main(){
    document.getElementById("connect_button").addEventListener("click", connect_server);
    document.getElementById("servo_change_button").addEventListener("click", send_servo_values);
    initiate_figures();
    initiate_servo_input();
    // Test part
    // document.getElementById("test_send_button").addEventListener("click", test_send);
}