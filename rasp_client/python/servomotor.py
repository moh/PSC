import time

# class to represent servo motors
class ServoMotor:
	servos = []
	
	"""
	pi : the pi board using pi =  pigpio.pi()
	name : name of the servo 
	pin : pin of the servo
	min_width : min width time of the servo (see description of the servo)
	max_ width : max width time of the servo
	max_angle : max angle of the servo
	default_angle : the angle at the start
	"""
	def __init__(self, pi, name, pin, min_width, max_width, max_angle, default_angle = 0):
		self.pi = pi
		self.name = name
		self.pin = pin
		self.min_width = min_width
		self.max_width = max_width
		self.max_angle = max_angle
		self.angle = default_angle
		ServoMotor.servos.append(self)
		# rotate to default angle
		self.rotate(self.angle)
	
	# rotate a servo
	def rotate(self, angle):
		if (angle > self.max_angle or angle < 0):
			raise Exception("Angle is negative or too big")
		width = (self.max_width - self.min_width)*angle/self.max_angle + self.min_width
		self.pi.set_servo_pulsewidth(self.pin, width)
		time.sleep(0.1)
		self.angle = angle
		return angle
	
	# class function
	# rotate a servo given its name
	def rotate_by_name(name, angle):
		for servo in ServoMotor.servos:
			if (servo.name == name):
				return servo.rotate(angle)
