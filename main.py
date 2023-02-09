import serial
import time
import threading


class SerialCom:
    def __init__(self, port="/dev/ttyACM0"):
        self.port = port
        self.baud_rate = 9600
        self.command = ""
        self.thread = threading.Thread(target=self.main)
        self.thread.start()

    def doStep(self, right, steps, sleep_micro_seconds):
        self.command = "right" if right else "left"
        self.command += "," + str(steps) + "," + str(sleep_micro_seconds) + "\r"

    def main(self):
        while True:
            try:
                _serial = serial.Serial(self.port, self.baud_rate)
                time.sleep(1)
                while True:
                    if self.command != "":
                        _serial.write(self.command.encode())
                        print("Command sent", self.command)
                        self.command = ""
                    time.sleep(0.001)
            except:
                print("Serial Com Exception")
            time.sleep(1)


if __name__ == "__main__":
    serial_com = SerialCom()
    while True:
        serial_com.doStep(True, 800*5, 200)
        time.sleep(2)
        serial_com.doStep(False, 800*5, 200)
        time.sleep(2)
        print("Doing steps")
