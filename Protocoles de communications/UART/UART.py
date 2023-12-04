import serial
from time import sleep


ser = serial.Serial("/dev/serial0", 9600, timeout=1)
ser.flush()


def serial_ask(sentence):
    ser.write(sentence)
    sleep(10)
    received_data = ser.readline().decode("utf-8").rstrip()
    print(received_data)
    return received_data


def uart_test():
    try:
        if serial_ask(b"Turn Led On") != "You sent me: Turn Led On":
            return -1
        sleep(1)
        if serial_ask(b"Turn Led Off") != "You sent me: Turn Led Off":
            return -1
        return 1
    except:
        return -1


if __name__ == "__main__":
    print("UART Test")
    uart_test()