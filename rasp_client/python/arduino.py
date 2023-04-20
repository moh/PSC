import serial
import serial.tools.list_ports
import json
import time
import sys

port = "/dev/ttyACM0"

# data = {"yaw" : None, "pitch" : None, "roll" : None}

# Detect which port the arduino is connected to 
ports = list(serial.tools.list_ports.comports())

for p in ports:
    p = str(p)
    if ("GENUINO" in p):
        # get port name 
        port = p.split("-")[0]
        port = port.replace(" ", "")


def clear_answer(answer):
    data = dict()
    answer = answer.decode()
    answer = answer.replace("\r", "")
    answer = answer.replace("\n", "")
    answer = answer.split(",")
    for info in answer:
        info = info.split(":")
        data[info[0]] = float(info[1])
        
    return data

while serial.Serial(port, 9600, timeout = 1) as arduino:
    time.sleep(0.1)
    if (arduino.isOpen()):
        try:
            while True:
                while (arduino.inWaiting() == 0):pass
                if (arduino.inWaiting() > 0):
                    answer = arduino.readline()
                    answer_data = clear_answer(answer)
                    print(json.dumps(answer_data))
                    sys.stdout.flush()            
        except Exception as e:
            print(json.dumps({"error": e}))
            sys.stdout.flush()
        