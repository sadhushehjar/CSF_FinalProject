##############
## Script listens to serial port and writes contents into a file
##############
## requires pySerial to be installed 
import serial
import multiprocessing
import time
# Handel invalid byte exception for the Ardurino default jubrish print 
def invalid_byte(line):
    try: 
        line.decode('UTF-8')
        return 1
    except Exception:
        return 0

serial_port = '/dev/cu.usbserial-0001'
baud_rate = 9600; #In arduino, Serial.begin(baud_rate)
write_to_file_path = "/Users/shehjarsadhu/Desktop/HardwareSecurityFinal/data_ECG/serial_output.txt"
def serial_read_run():
    output_file = open(write_to_file_path, "w+");
    ser = serial.Serial(serial_port, baud_rate)
    while True:
        line = ser.readline()
        print("ACTUAL PAYLOAD= \n",line,type(line))
        if invalid_byte(line)!= 0: # If valid first bad.
            line_str = line.decode('UTF-8')
            print(line_str,type(line_str))
            output_file.write(line_str)
if __name__ == '__main__':
    p = multiprocessing.Process(target=serial_read_run, name="serial_read_run")
    p.start()
    # Wait for 2 mins to collect data then stop.
    time.sleep(120)
      # Terminate foo
    p.terminate()
    p.join()
