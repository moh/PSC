# PSC

## Start Application 

`$ npm start `


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
* Info from server to client : 
  * About connection of remote_device : {type : "remote_device_presence", 
  answer : true or false, remote_device_id : ""}
* Command from client to remote_device : 
  * {type : "command", }


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
  * Route messages 
* Organisation :
  * Define a format for data 


type : "remote_device_presence",
            answer : present,
            remote_device_id : remote_device_id