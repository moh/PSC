# PSC

## Start Application 

`$ npm start `


## Project parts

express_server : server based on javascript


Server address : ws://localhost:3000

## Data format

* Connection to server : 
 * Client to server : { type: 'connect', client_id: '...', remote_device_id: '...' }
 * Server to client : accept or reject connection
   * { "type": "connect", "answer": "accepted" }
   * { "type": "connect", "answer": "rejected" }
* Command from client to remote_device : 
 * 


## Planning :
* GUI connection to server : 
  * Prevent connecting while connected
  * Deal with exceptions 
  * Prevent trying to connect while fields are empty 
* GUI : [Priority]
  * Define the design 
* Server : 
  * Route messages 
* Organisation :
  * Define a format for data 
