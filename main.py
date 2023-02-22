import serial
import time
import threading


class SerialCom:
    def __init__(self, port="/dev/ttyUSB0"):
        self.port = port
        self.baud_rate = 9600
        self.command = ""

        self.last_value = None
        self.start_value = None
        self.thread = threading.Thread(target=self.main)
        self.thread.start()

    def doStep(self, right, steps, sleep_micro_seconds):
        self.command = ("right" if right else "left") + "," + str(steps) + "," + str(sleep_micro_seconds) + "\r"

    def angle_changed(self, current_value):
        if self.last_value is None:
            self.last_value = current_value
            self.start_value = current_value
            return

        if current_value != self.last_value:
            print("Angle changed", self.last_value - current_value,
                  (((current_value - self.start_value) % 2400) / 2400) * 360)
            self.last_value = current_value

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

                    data = _serial.readline().decode().strip()
                    # print(data)
                    if data.startswith("#") and data.endswith("#"):
                        current_value = int(data.replace("#", ""))
                        self.angle_changed(current_value)

                    time.sleep(0.0001)
            except:
                print("Serial Com Exception")
            time.sleep(1)


if __name__ == "__main__":
    serial_com = SerialCom()
    while True:
        serial_com.doStep(True, 800, 200)
        time.sleep(1)
        serial_com.doStep(False, 800, 200)
        time.sleep(1)
        print("Doing steps")