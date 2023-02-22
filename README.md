# PSC

## Start Application 

`$ npm start `

## Requirements

* For rasp client:
  * npm install python-shell
  * pyserial, GPIO


## Project parts

express_server : server based on javascript
rasp_client : contain the program in the raspberry pi
UxVSim      : a sail boat simulator


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
  * GPS data : {type : "send_data", data_type : "GPS", data : {"alt" : altitude, "lat" : latitude, "long" : longitude, "speed" : speed, "sat" : num_sats}}
 
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

## Simulator : UxVSim
The folder contains a file "UxVSim.txt" which contains all the parameter for the simulation.
The parameter "robid" should be equal to "0x00008000" to simulate a sailboat
To run the simulator : 
`
make
./UxVSim
`

When we start the program, it will produce some csv files that contain the different data read from sensors and the servos angle. We should delete those file each time before starting the program.

The program have multiple servers that each simulate an electronic device:
* Simulation of meteo station with GPS data (NMEA) on port 4001 (telnet 127.0.0.1 4001)
  * For data starting with "$WIMDA", like "$WIMDA,30.0000,I,1.0000,B,15.5,C,,,,,,,10.2,T,10.2,M,13.0,N,6.7,M*1A"
   * The "10.2,T," is wind direction where the 0 is to the north, and 90 is to the east.
   * The "6.7, M*1A" is the speed of wind in m/s
  * For data starting with "$HCHDG", like "$HCHDG,21.8,0.0,E,0.0,W*6B":
   * The "21.8" is the direction of the cap (the ship direction), where 0 is to the north and 90 to the east
* Simulation of IMU (Razor AHRS) on port 4007 (telnet 127.0.0.1 4007)
  * the data is of format "#YPR=-52.26,-2.73,-12.44" that it means "yaw(z axis)", "pitch (y axis)" and "Roll (x axis)"
   * for yaw data : it is between -180 and 180 where 0 is the north and 90 is the est  
* Simulation of servo to control the gouvernail on port 4004:
  * Send command of type "#0P1000\r" (an extreme side) to "#0P2000\r" (the other side)
  * See the command in the file "SSC32Interface.csv" using the command "tail -f SSC32Interface.csv"
* Simulation of servo to control the voile on port 4003:
  * Send command of type "R0\r" (voile closed) to "R116000\r" (voile open to max), it supports also negative value "R-116000\r" for the other side
  * See the command in the file "IM483IInterface.csv" using the command "tail -f IM483IInterface.csv"
   
## Problems 

* Problem 1: javascript not reading every time the python print when there is a while true in python: 
  * Solution : use "sys.stdout.flush()" after print in python


## Planning :
* GUI connection to server : 
  * Prevent connecting while connected [Done]
  * Deal with exceptions [Done]
  * Prevent trying to connect while fields are empty [Done] 
  * Deal when server is down, or disconnect [Done]
* Raspberry pi connection to server : 
  * try to connect to server automatically every 1s in case of server down or error [DONE]
* Raspberry pi : 
  * Connecting arduino to application
  * Reading wind speed by arduino
  * Test GPS with application 
  * Path planning algorithm
* GUI : [Priority]
  * Define the design [DONE]
  * Add arduino information part [DONE]
  * Add map and functionalities
  * Path planning 
* Server : 
  * prevent connection to remote_device while busy [DONE]
  * Inform PC when remote device is connected or not [DONE]
  * Route messages [DONE]
* Simulation :
  * Integrate simulation with rasp_client [DONE]

* Navigation : 
  * Activate and desactivate navigation

* Organisation :
  * Define a format for data 
