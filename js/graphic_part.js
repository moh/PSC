var ctx_servo_1; 
var x = 0;

function initiate_servos(){
    canvas_s1 = document.getElementById("canvas_servo_1");
    centerX = canvas_s1.width/2;
    centerY = canvas_s1.height/2;
    ctx_servo_1 = canvas_s1.getContext('2d');
    ctx_servo_1.fillStyle = 'black';
    ctx_servo_1.fillRect(centerX, centerY, 25, 2);
    ctx_servo_1.translate(centerX, centerY);
    rotate_figure(380, ctx_servo_1, canvas_s1);
}

function rotate_figure(angle, ctx, canvas){
    
    ctx.clearRect(-canvas.width/2,-canvas.height/2, canvas.width, canvas.height);
    ctx.rotate(angle * Math.PI / 180);
    ctx.fillStyle = 'black';
    ctx.fillRect(0,0, 25, 2);
}