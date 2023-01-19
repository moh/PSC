from simulation import Simulation
import json

host = "127.0.0.1"

voile = Simulation(host, 4003, "voile")
gouvernail = Simulation(host, 4004, "gouvernail")

voile_angle = 0
gouvernail_angle = 0

answer_data = {"servo_1" : voile_angle, "servo_2" : 0, "servo_3" : gouvernail_angle}
print(json.dumps(answer_data))

while True:
    data = input()
    data = json.loads(data)
    try:
        voile_angle = voile.send_to_servo_voile(int(data["servo_1"]))
        gouvernail_angle = gouvernail.send_to_servo_gouvernail(int(data["servo_3"]))
    except Exception as e:
        pass
    answer_data = {"servo_1" : voile_angle, "servo_2" : 0, "servo_3" : gouvernail_angle}
    print(json.dumps(answer_data))