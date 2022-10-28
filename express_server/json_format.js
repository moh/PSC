/*
------------------------------------------
Part of JSON message to send to client
------------------------------------------
*/



/**
 * This function returns a JSON string that indicate if the connection is accpeted or not
 * @param {*} accepted boolean to indicate if connection is accepted or not
 * @param {*} message message in case if connection is srejected
 * @returns JSON that contains the data
 */
export function json_answer_connection(accepted, message = ""){
    if(accepted){
      return JSON.stringify({
        type: "connect",
        answer : "accepted"
      })
    } else{
      return JSON.stringify({
        type: "connect",
        answer : "rejected",
        message : message
      })
    }
}

/**
 * This function returns JSON indicating if the remote_device is connected to server or not
 * @param {*} present boolean, true if remote_device is connected, false if not 
 * @returns JSON that contains the data 
 */
export function json_answer_remote_device_status(present, remote_device_id){
    
    return JSON.stringify({
        type : "remote_device_presence",
        answer : present,
        remote_device_id : remote_device_id
    });   
}

/**
 * This function return JSON indcating if the remote_device is associated to a PC or not
 * @param {*} associated true or false 
 * @param {*} client_id the client_id of PC associated to remote device
 * @returns JSON that contains the data 
 */
export function json_remote_device_associated(associated, client_id){
  return JSON.stringify({
    type : "PC_presence",
    answer : associated,
    client_id : client_id
  });
}