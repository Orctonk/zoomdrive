#!/usr/bin/python

import serial
import paho.mqtt.client as mqtt

serialdev = '/dev/ttyUSB0'
broker = "tfe.iotwan.se"
port = 1883
publish_topic = "zoomdrive/test"
subscribe_topic = "zoomdrive/tests"


#MQTT callbacks
def on_connect(client,userdata,flags,rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(subscribe_topic)

def on_message(client, userdata, msg):
    print("recieved message: \"{}\" from topic \"{}\"".format(msg.payload,msg.topic))
    ser.write(msg.payload)
    ser.flush()

def on_publish(client,userdata,mid):
    print("Published {}".format(mid))


#called on exit
#close serial, disconnect MQTT

def cleanup():
    print("Ending and cleaning up")
    ser.close()
    client.disconnect()


try:
    print("Connecting... {}".format(serialdev))
    #connect to serial port
    ser = serial.Serial(serialdev,4800)
except:
    print("Failed to connect serial")
    #unable to continue with no serial input
    raise SystemExit


try:
    ser.flushInput()
    #create an mqtt client
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_publish = on_publish

    client.username_pw_set("student","Hbrygga")

    client.connect_async(broker,port,60)
    
    client.loop_start()

    #remain connected to broker
    #read data from serial and publish
    while True:
        line = ser.readline().strip()
        #second list element is temp
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