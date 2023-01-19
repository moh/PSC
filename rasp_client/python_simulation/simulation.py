import telnetlib
import pynmea2

class Simulation:
    
    def __init__(self, host, port, name):
        self.tn = telnetlib.Telnet()
        self.name = name
        self.tn.open(host, port)

    def read_YPR(self):
        r_data = dict()
        if (self.name == "YPR"):
            data = self.tn.read_until(b'\n').decode()
            if (data[0:4] == "#YPR"):
                data = data[5:].replace("\r\n", "")
                data = data.split(",")
                r_data["yaw"] = float(data[0])
                r_data["pitch"] = float(data[1])
                r_data["roll"] = float(data[2])
            return r_data

        else:
            raise Exception("Error in name")

    def read_meteo_GPS(self):
        r_data = dict()
        if (self.name == "meteo_GPS"):
            data = self.tn.read_until(b'\n').decode().replace("\r\n", "")
            # GPS data
            if ("GPGGA" in data):
                return self.parse_gps_data(data)
            elif ("WIMDA" in data):
                return self.parse_wind_data(data)
        else:
            raise Exception("Error in name")
                

    def parse_gps_data(self, data):
        gps_data = pynmea2.parse(data)
        return {"alt" : gps_data.altitude, "lat" : gps_data.latitude, 
                "long" : gps_data.longitude, "speed" : 0, "sat" : gps_data.num_sats}
    

    def parse_wind_data(self, data):
        wind_data = data[7:].split(",")
        wind_direction = wind_data[wind_data.index("T") - 1]
        wind_speed = wind_data[-2]
        return {"wind_direction" : wind_direction, "wind_speed" : wind_speed}

    
    def send_to_servo_gouvernail(self, angle):
        """
        Here angle is in degree, with max -+45 degree 
        Data of type b"#0P1500\r" which refer to 0, b"#0P1000\r" refer to -0.785398 rad,
        and b"#0P2000\r" refers to 0.785398 rad
        Read data from SSC32Interface.csv
        """
        if (self.name != "gouvernail"):
            raise Exception("Error in name")
        if (abs(angle) > 45):
            angle = (angle/abs(angle))*45
        angle_code = str.encode("#0P" + str(int((angle*500)//45) + 1500) + "\r")
        self.tn.write(angle_code)
        return angle

    def send_to_servo_voile(self, angle):
        """
        Here angle is in degree with max +-90
        Data of type "R0\r" which refer to 0 degree (voile closed), 
        or "R116000\r" which refer to -90 degree (voile open to max)
        it suppports negative value like "R-116000\r" for 
        the other side which refer to 90 degree
        """
        if (self.name != "voile"):
            raise Exception("Error in name")
        if (abs(angle) > 90):
            angle = (angle/abs(angle))*90
        angle_code = str.encode("R" + str(int((angle*(-116000)//90))) + "\r")
        self.tn.write(angle_code)
        return angle
        