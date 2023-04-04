import json
import sys
import math
import time
# to transform lat lng to x and y coordinates
import utm

data_fields = {"wind_direction" : None, "wind_speed" : None, 
        "yaw" : None, "pitch" : None, "roll" : None, 
        "alt" : None, "lat" : None, "lon" : None, 
        "speed" : None, "sat" : None, "target" : None};

voile_angle = 0
gouvernail_angle = 0
answer_data = {"servo_1" : voile_angle, "servo_2" : 0, "servo_3" : gouvernail_angle}

target_changed = False

last_time = time.time()
last_pos = [0, 0]
velocity = 1

# min distance to go to target 

MIN_DIST_TARGET = 3

# test
# desired_angle
# file = open("output.txt", "a")

DESIRED_ANGLE = 20
LIMIT_WIND_BOAT_ANGLE = 40
previous_yaw = 0

"""
adapt voile angle (servo 1 ) to get max speed
"""
def algo1():
    wind_direction = data_fields["wind_direction"]
    yaw = data_fields["yaw"]
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

    data_fields["servo_1"] = servo1
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



def algo3():
    global last_pos, last_time, velocity
    target = data_fields["target"]
    if(target == None):
        return (-1, 0, 0)
    if((data_fields["lat"] == None) or (data_fields["lon"] == None)):
        return (-1, 0, 0)
    
    x_target, y_target = utm.from_latlon(target["lat"], target["lng"])[0:2]
    x_boat, y_boat = utm.from_latlon(data_fields["lat"], data_fields["lon"])[0:2]
    
    
    # distance between boat and target 
    dist_boat_target = ((x_target - x_boat)**2 + (y_target - y_boat)**2)**0.5
    dst = ((last_pos[0] - x_boat)**2 + (last_pos[1] - y_boat)**2)**0.5
    if (dst > 0):
        t = time.time()
        velocity = dst /(t - last_time)
        last_pos = [x_boat, y_boat]
        last_time = t
    
    if (dist_boat_target <= MIN_DIST_TARGET):
        return ("d : " + str(dist_boat_target), velocity)
    

    # angle between x axis and (boat, target) when distance is small compared to earth deformation
    angle_boat_target = math.atan2(y_target - y_boat, x_target - x_boat)*180/math.pi

    gouvernail_angle = ((90 - data_fields["yaw"]) - angle_boat_target) % 360
    
    angle_dest_wind = (data_fields["wind_direction"] - 90) - angle_boat_target - 180

    
    if(gouvernail_angle > 180): gouvernail_angle = gouvernail_angle - 360
    data_fields["servo_3"] = gouvernail_angle
    return (gouvernail_angle, velocity, angle_dest_wind)
    
    
    

while True:
    data = input()
    data = json.loads(data)

    target_changed = False
    # update data value 
    for field in data_fields:
        if(field in data):
            if(field == "target"):
                target_changed = True
            data_fields[field] = data[field]
    
    # do nothing for the first loop the target change 
    if target_changed: continue
    # gouvernail_angle = 20
    # voile_angle = algo1(data_fields["wind_direction"], data_fields["yaw"])
    # gouvernail_angle = algo2(data_fields["yaw"])
    # txt = "wind_direction = " + data_fields["wind_direction"] + "  yaw = " + data
    gouvernail_angle, velocity, angle_dest_wind = algo3()
    voile_angle = algo1()

    answer_data = {"servo_1" : voile_angle, "servo_2" : 0, 
                   "servo_3" : gouvernail_angle, "speed" : velocity, "dst_wind" : angle_dest_wind}
    
    print(json.dumps(answer_data))
    sys.stdout.flush()


