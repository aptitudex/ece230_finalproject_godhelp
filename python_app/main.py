import argparse 
import keyGen
import time
import serial
import logging
import hashlib

from datetime import datetime

from terminal import start_terminal

DEBUG = True

rtc_timedelta: datetime = datetime.now()
logger = logging.getLogger()

def logging_write(string, port: serial.Serial):
    data_out = bytes(string,"utf-8")
    logging.debug(f"Writing {data_out} to buffer")
    written = port.write(bytes(string, "utf-8"))
    logging.debug(f"Wrote {written} bytes")
    return written

def simple_request(rq: str, port: serial.Serial, length = 2):
    # Write a request
    logging_write(rq, port)
    port.write(bytes(rq,'ascii'))
    print("Sent req.")
    # Wait until buffer is at least mostly populated.
    while port.in_waiting < length: 
        time.sleep(0.001)
    
    data = port.read_all()
    return str(data,'ascii')[:length]

def generate_otp(date_time: datetime):
    hasher = hashlib.sha1()
    round_sec = round(date_time.second)
    second_tenth = round_sec - (round_sec % 10)
    # print(f"{second_tenth},{time.minute:02d},{time.hour:02d},{time.day:02d},{time.month:02d},{time.year}")
    hasher.update(bytes(f"{second_tenth},{date_time.minute:02d},{date_time.hour:02d},{date_time.day:02d},{date_time.month:02d},{date_time.year}", 'utf-8'))

    digest = hasher.digest()
    return [d % 10 for d in digest]

def handle_otp(port: serial.Serial):
    """Handles the operations between the embedded device and user for creating the one-time password."""
    # Request time
    time_str = simple_request("timerq", port, 19).split(",")

    rtc_time = datetime(second=int(time_str[0]), minute=int(time_str[1]), hour=int(time_str[2]), day=int(time_str[3]), month=int(time_str[4]), year=int(time_str[5]))
    cur_time = datetime.now()
    differential = rtc_time - cur_time

    prev_code = None
    # While not received the confirmation to continue.
    port.write(bytes("reqotp", "ascii"))
    
    auth_text = ""
    print("\x1b[2J") #Clear the console
    waiting_for_auth = True
    while waiting_for_auth:            
        code = generate_otp(datetime.now() + differential)

        temp_time = datetime.now()+differential
        second_tenth = 10 - round(temp_time.second) % 10
        print(f"\r[{second_tenth:2}] Please type the following code into the keypad: {code[:4]}", end='')
        auth_text += str(port.read_all(),'ascii')
        if 'auth' in auth_text:
            waiting_for_auth = False

    # Once received begin encrypted commmunications good to proceed to next step.
    return True
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog="ECE230 Term Project Z&A Encryption Support",
    description="Helpers for running our project",
    epilog="Good luck!")

    parser.add_argument("port")
    # parser.add_argument("")

    args = parser.parse_args()
    
    #pkeyGen.encryp_file('C:\\Users\\armstrjj\\workspace_v12\\ece230TermProjectZiemer&Armstrong\\python_app\\NeverGonnaGiveYouUp.txt', outfile='NeverGonnaGiveYouUp.hash')
    logger.info("Opening: COM"+str(args.port))

    output = ""

    # Seven bytes for otp negotiation
    with serial.Serial('COM'+str(args.port), 38400, timeout=0.5, parity=serial.PARITY_NONE, rtscts=1, xonxoff=True) as port:
        handle_otp(port)
        
        start_terminal(port, "key")

    exit()
    