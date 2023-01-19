from simulation import Simulation
import json
import time

host = "127.0.0.1"

imu = Simulation(host, 4007, "YPR")

while True:
    answer_data = imu.read_YPR()
    print(json.dumps(answer_data))
    time.sleep(0.1)