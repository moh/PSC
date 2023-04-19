import pigpio 
import time
import json
import sys
from servomotor import ServoMotor

# initialise with "sudo pigpiod"
pi = pigpio.pi()
servo_1 = ServoMotor(pi, "servo_1", 23, 600, 2400, 2826)
servo_2 = ServoMotor(pi, "servo_2", 24, 600, 2400, 2826)
# send the first time 
answer_data = {"servo_1" : servo_1.angle, "servo_2" : servo_2.angle, "servo_3" : 0}
print(json.dumps(answer_data))

while True:
	data = input()
	data = json.loads(data)
	try:
		servo_1.rotate(int(data["servo_1"]))
		servo_2.rotate(int(data["servo_2"]))	
	except Exception as e:
		pass
	answer_data = {"servo_1" : servo_1.angle, "servo_2" : servo_2.angle, "servo_3" : 0}
	print(json.dumps(answer_data))
	sys.stdout.flush()

