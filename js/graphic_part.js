var ctx_servo_1, ctx_servo_2, ctx_servo_3; 
var canvas_width = 50;
var canvas_height = 50;
var ctx_servos = [ctx_servo_1, ctx_servo_2, ctx_servo_3];
var angle_servos = [0, 0, 0];

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
 * Rotate the servo with angle in respect to x axis
 * @param {*} servo the name of the servo, should be "servo_i" where i in {1,2,3}
 * @param {*} angle the angle
 */
function rotate_servo(servo, angle){
    var i = servo[servo.length - 1] - 1;
    rotate_figure(angle - angle_servos[i], ctx_servos[i]);
    angle_servos[i] = angle;
    document.querySelector("input[name='" + servo + "']").value = angle;
}

/**
 * 
 * @param {*} angle the anlge relative to last angle
 * @param {*} ctx the servo canvas
 */
function rotate_figure(angle, ctx){
    ctx.clearRect(-canvas_width/2,-canvas_height/2, canvas_width, canvas_height);
    ctx.rotate(angle * Math.PI / 180);
    ctx.fillStyle = 'black';
    ctx.fillRect(0,0, 25, 2);
}