import json

# servo name sent in json
SERVO_1 = "servo_1"
SERVO_2 = "servo_2"
SERVO_3 = "servo_3"



while True:
    try:
        # load json object from string 
        servos = json.loads(input())

        print(servos["data"])
    # in case it is not a valid JSON object, we continue
    except json.decoder.JSONDecodeError:
        continue