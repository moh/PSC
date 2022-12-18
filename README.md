# PSC

## Start Application 

`$ npm start `

## Requirements

* For rasp client:
  * npm install python-shell

## Project parts

express_server : server based on javascript


Server address : ws://localhost:3000

## Data format

* Connection to server : 
  * Client (PC) to server : { type: 'connect', client_id: '...', remote_device_id: '...', client_type : 'PC' }
  * Client (RASP) to server : { type: 'connect', 
  remote_device_id: '...', client_type : 'RASP' }
  * Server to client : accept or reject connection
    * { type: "connect", answer: "accepted" }
    * { type: "connect", answer: "rejected", message: A }, where A in {"Error in id"}
* Info from server to client (PC) : 
  * About connection of remote_device : {type : "remote_device_presence", 
  answer : true or false, remote_device_id : ""}
* Info from server to client (RASP) : 
  * That RASP is associated to a PC : {type : "PC_presence", answer : true or false, client_id : pc_client_id}
* Command from client to remote_device : 
  * set servos : {type : "command", data : {servo_1 : .., servo_2 : .., servo_3 : ..}}
  * Ask for data : {type : "get_data", data : A} where A in {"*", "SERVO", "GPS", "WIND"}
* From remote_device to client : 
  * Send data : {type : "send_data", data_type : A, data : {...}} depends on the data type asked by client, where A in {"*", "SERVO", "GPS", "WIND"}
  * wind data : {type : "send_data", data_type : "WIND", data : {"wind_direction" : angle, "wind_speed" : 000}}
 
## Socket information : 
In the express server index.js : 
* attributes of socket of PC : 
  * client_id : the id of the PC gui
  * remote_device_id : the id of remote device that it wants to control
  * client type : "PC" or PC_type
  * remote_device_socket : socket of the remote device bound to it ( it is initialised when the remote device is connected to server)
* attributes of socket of RASP :
  * remote_device_id : the id of itself
  * client_type : "RASP" or RASP_type
  * pc_socket : the socket of the pc bind to it ( initialised when the two are connected )


## Planning :
* GUI connection to server : 
  * Prevent connecting while connected [Done]
  * Deal with exceptions [Done]
  * Prevent trying to connect while fields are empty [Done] 
  * Deal when server is down, or disconnect [Done]
* Raspberry pi connection to server : 
  * try to connect to server automatically every 1s in case of server down or error [DONE]
* GUI : [Priority]
  * Define the design 
* Server : 
  * prevent connection to remote_device while busy [DONE]
  * Inform PC when remote device is connected or not [DONE]
  * Route messages [DONE]
* Organisation :
  * Define a format for data 