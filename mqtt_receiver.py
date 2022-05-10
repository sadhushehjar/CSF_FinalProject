import paho.mqtt.client as mqtt
import pandas as pd
import multiprocessing
import time
#Create the CSV file.
###
#CHANGE THIS FILE PATH TO YOUR LOCAL FILE PATH.
###
filepath = "/Users/shehjarsadhu/Desktop/HardwareSecurityFinal/data_ECG/"
# Write Header once
header = "Topic,MacAdress,ECG,Timestamp \n"
with open(filepath + "mqtt_output.txt",'a+') as f:
        f.write(header)
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe([("/csf/ecg/",1)])
# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("Payload \n",msg.payload,"Type: \n",type(msg.payload))
    print("Converted to str: ",msg.payload.decode('UTF-8'))
    # print(msg.topic+" "+str(msg.payload))
    with open(filepath + "mqtt_output.txt",'a+') as f:
        f.write(msg.payload.decode('UTF-8') + "\n")

def mqtt_reciver_run():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("test.mosquitto.org")

    client.loop_forever()

if __name__ == '__main__':
    p = multiprocessing.Process(target=mqtt_reciver_run, name="mqtt_reciver_run")
    p.start()
    # Wait for 2 mins to collect data then stop.
    time.sleep(120)
      # Terminate foo
    p.terminate()
    p.join()
