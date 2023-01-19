from simulation import Simulation
import json

host = "127.0.0.1"

meteo_GPS = Simulation(host, 4001, "meteo_GPS")

while True:
    answer_data = meteo_GPS.read_meteo_GPS()
    if (answer_data != None):
        print(json.dumps(answer_data))
