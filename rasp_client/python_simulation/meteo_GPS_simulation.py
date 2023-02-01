from simulation import Simulation
import json
import sys
import time

host = "127.0.0.1"
counter = 0 

meteo_GPS = Simulation(host, 4001, "meteo_GPS")

while True:
    answer_data = meteo_GPS.read_meteo_GPS()
    # case if we get a string with code not defined
    if (answer_data != None):
        answer_data["counter"] = counter
        print(json.dumps(answer_data))
        sys.stdout.flush()
        counter += 1
    time.sleep(0.5)