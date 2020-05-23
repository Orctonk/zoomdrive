#!/usr/bin/python3

import serial
import paho.mqtt.client as mqtt

serialdev = '/dev/ttyACM1'
broker = "tfe.iotwan.se"
port = 1883
subscribe_topic = "zoomdrive/#"


#MQTT callbacks
def on_connect(client,userdata,flags,rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(subscribe_topic)

def on_message(client, userdata, msg):
    rec = msg.payload.decode()
    print("recieved message: \"{}\" from topic \"{}\"".format(rec,msg.topic))
    ser.write((rec + "\n").encode())

#called on exit
#close serial, disconnect MQTT

def cleanup():
    print("Ending and cleaning up")
    ser.close()
    client.disconnect()


try:
    print("Connecting... {}".format(serialdev))
    #connect to serial port
    ser = serial.Serial(serialdev,9600)
except:
    print("Failed to connect serial")
    #unable to continue with no serial input
    raise SystemExit


try:
    ser.flushInput()
    # create an mqtt client
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.username_pw_set("student","Hbrygga")

    client.connect_async(broker,port,60)
    
    client.loop_start()

    topicdict = {
        0b00100000 : "zoomdrive/to_car",
        0b01000000 : "zoomdrive/from_car",
        0b10000000 : "zoomdrive/info"
    }

    #remain connected to broker
    #read data from serial and publish
    while True:
        line = ser.readline().decode().strip()
        split = line.split(' ')
        topic = split[0] 
        publish_topic = topicdict[int(topic) & 0b11100000]
        
        line = split[0]
        for arg in split[1:]:
            line = line + " " + arg
        print("publishing \"{}\"".format(line))
        client.publish(publish_topic, line)


# handle list index error (i.e. assume no data received)
except (IndexError):
    print("No data received within serial timeout period")
    cleanup()
# handle app closure
except (KeyboardInterrupt):
    print("Interrupt received")
    cleanup()
except (RuntimeError):
    print("uh-oh! time to die")
    cleanup()