/**
 * This file contains the code for servo and wind part
 */
var ctx_servo_1, ctx_servo_2, ctx_servo_3; 
var ctx_wind;
var canvas_width = 50;
var canvas_height = 50;
var ctx_servos = [ctx_servo_1, ctx_servo_2, ctx_servo_3];
var angle_servos = [0, 0, 0];
var angle_wind = 0;


/**
 * initiate wind and servo figures
 */
function initiate_figures(){
    initiate_servos();
    initiate_wind_direction();
}

/**
 * this function initiate the rectangles that represent the three server
 */
function initiate_servos(){
    var canvas;
    for(var x = 0; x <= 2; x++){
        canvas = document.getElementById("canvas_servo_" + (x + 1));
        centerX = canvas.width/2;
        centerY = canvas.height/2;
        ctx_servos[x] = canvas.getContext('2d');
        ctx_servos[x].fillStyle = 'black';
        ctx_servos[x].fillRect(centerX, centerY, 25, 2);
        ctx_servos[x].translate(centerX, centerY);
    }
}

/**
 * Function that initiate wind direction 
 */
function initiate_wind_direction(){
    var canvas;
    canvas = document.getElementById("canvas_wind");
    centerX = canvas.width/2;
    centerY = canvas.height/2;
    ctx_wind = canvas.getContext('2d');
    ctx_wind.fillStyle = 'black';
    ctx_wind.fillRect(centerX, centerY, 25, 2);
    ctx_wind.translate(centerX, centerY);    
}


/**
 * Rotate the servo with angle in respect to x axis
 * @param {*} servo the name of the servo, should be "servo_i" where i in {1,2,3}
 * @param {*} angle the angle
 */
function rotate_servo(servo, angle){
    var i = servo[servo.length - 1] - 1;
    var inp_servo = document.querySelector("input[name='" + servo + "']");
    rotate_figure(angle - angle_servos[i], ctx_servos[i]);
    angle_servos[i] = angle;

    // modify the value if the input is not selected by user
    if (inp_servo.dataset.modify == "true"){
        inp_servo.value = angle;
    }
}

/**
 * Rotate the wind direction 
 * @param {*} angle angle relativ eto x axis 
 */
function rotate_wind_direction(angle){
    rotate_figure(angle - angle_wind, ctx_wind);
    angle_wind = angle;
    document.querySelector("input[name='wind_direction']").value = angle;
}

/**
 * rotate the figure by angle
 * @param {*} angle the anlge relative to last angle
 * @param {*} ctx the figure canvas
 */
function rotate_figure(angle, ctx){
    ctx.clearRect(-canvas_width/2,-canvas_height/2, canvas_width, canvas_height);
    ctx.rotate(angle * Math.PI / 180);
    ctx.fillStyle = 'black';
    ctx.fillRect(0,0, 25, 2);
}

/**
 * when servo value input is checked then we call the function input_is_checked
 */
function initiate_servo_input(){
    var servo_inputs = document.querySelectorAll(".servo_info input");
    for(var x = 1; x < servo_inputs.length; x++){
        servo_inputs[x].addEventListener("focus", input_is_checked);
    }
}

/**
 * Mark that the input is checked to stop changing its value
 * @param {*} event the event 
 */
function input_is_checked(event){
    event.target.dataset.modify = "false";
}

/**
 * 
 * @returns return a list of the value of server inputs
 */
function get_servos_input(){
    var servo_inputs = document.querySelectorAll(".servo_info input");
    var value = [];
    for(var x = 1; x < servo_inputs.length; x++){
        value.push(servo_inputs[x].value);
    }
    return value;
}

/**
 * reset the inputs so they can be modified by the program
 */
function reset_servos_input(){
    var servo_inputs = document.querySelectorAll(".servo_info input");
    for(var x = 1; x < servo_inputs.length; x++){
        servo_inputs[x].dataset.modify = "true";
    }
}

function send_servo_values(){
    // if PC not connected to server or remote device not connected to server
    // we do nothing
    if (!connected || !remote_device_connected){
        return;
    }

    var servo_values = get_servos_input();

    socket.send(JSON.stringify({
        type : "command",
        data : { servo_1 : servo_values[0], servo_2 : servo_values[1], servo_3 : servo_values[2]}
    }));

    reset_servos_input();
}