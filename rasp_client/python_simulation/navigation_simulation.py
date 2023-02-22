import json
import sys

data_fields = {"wind_direction" : None, "wind_speed" : None, 
        "yaw" : None, "pitch" : None, "roll" : None, 
        "alt" : None, "lat" : None, "lon" : None, 
        "speed" : None, "sat" : None};

voile_angle = 0
gouvernail_angle = 0
answer_data = {"servo_1" : voile_angle, "servo_2" : 0, "servo_3" : gouvernail_angle}

# test
# desired_angle
# file = open("output.txt", "a")

DESIRED_ANGLE = 20
previous_yaw = 0

"""
adapt voile angle (servo 1 ) to get max speed
"""
def algo1(wind_direction, yaw):
    if ((wind_direction == None) or (yaw == None)):
        return 0
    wind_direction = float(wind_direction)
    yaw = float(yaw)
    # because 0 is the south for wind direction 
    wind_direction = 180 - wind_direction 
    alpha = abs(yaw - wind_direction)
    
    if alpha <= 30:
        servo1 = 0 
    else:
        servo1 = (alpha - 30)*90/(180 - 30)
    
    return servo1


def algo2(yaw):
    global previous_yaw
    servo3 = 0
    if (yaw == None):
        return 0
    yaw = float(yaw)
    beta = delta = DESIRED_ANGLE - yaw
    #angle_change = yaw - previous_yaw
    #previous_yaw = yaw
    #beta = delta + angle_change
    if (beta > 30):
        servo3 = 30
    elif (beta < -30):
        servo3 = -30
    else:
        servo3 = beta

    return servo3

while True:
    data = input()
    data = json.loads(data)

    # update data value 
    for field in data_fields:
        if(field in data):
            data_fields[field] = data[field]

    
    # gouvernail_angle = 20
    # voile_angle = algo1(data_fields["wind_direction"], data_fields["yaw"])
    gouvernail_angle = algo2(data_fields["yaw"])
    # txt = "wind_direction = " + data_fields["wind_direction"] + "  yaw = " + data
    # file.write( data_fields["wind_direction"])
    answer_data = {"servo_1" : voile_angle, "servo_2" : 0, "servo_3" : gouvernail_angle}
    
    print(json.dumps(answer_data))
    sys.stdout.flush()


