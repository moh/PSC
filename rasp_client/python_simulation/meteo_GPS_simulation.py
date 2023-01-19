from simulation import Simulation
import json
import time

host = "127.0.0.1"
counter = 0 

meteo_GPS = Simulation(host, 4001, "meteo_GPS")

while True:
    answer_data = meteo_GPS.read_meteo_GPS()
    answer_data["counter"] = counter
    if (answer_data != None):
        print(json.dumps(answer_data))
        counter += 1
    time.sleep(0.5)