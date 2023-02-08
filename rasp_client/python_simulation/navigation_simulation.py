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

while True:
    data = input()
    data = json.loads(data)

    # update data value 
    for field in data_fields:
        if(field in data):
            data_fields[field] = data[field]

    # set servos angle
    # if (data_fields["yaw"] != None): 
    # error = 
    gouvernail_angle = 20
    answer_data = {"servo_1" : voile_angle, "servo_2" : 0, "servo_3" : gouvernail_angle}
    
    print(json.dumps(answer_data))
    sys.stdout.flush()
