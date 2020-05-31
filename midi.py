import paho.mqtt.client as mqtt
import mido
import time

inport = mido.open_input(mido.get_input_names()[1])

broker = "tfe.iotwan.se"
port = 1883
subscribe_topic = "zoomdrive/#"
dm = False

def handleNote(msg):
    global dm
    mqmsg = ""
    if(msg.note == 43):
        if msg.type == 'note_on':
            dm = True
            arg = 1
        if msg.type == 'note_off':
            dm = False
            arg = 0
        mqmsg = "35 3 %d" % arg
    if dm:
        if(msg.note == 55):
            if msg.type == 'note_on':
                arg = -1
            if msg.type == 'note_off':
                arg = 0
            mqmsg = "32 %d" % arg
        elif(msg.note == 56):
            if msg.type == 'note_on':
                arg = 1
            else:
                arg = 0
            mqmsg = "32 %d" % arg
        elif(msg.note == 57):
            if msg.type == 'note_on':
                arg = 1
            else:
                arg = 0
            mqmsg = "33 %d" % arg
        elif(msg.note == 53):
            if msg.type == 'note_on':
                arg = -1
            else:
                arg = 0
            mqmsg = "33 %d" % arg

    if mqmsg != "":
        client.publish("zoomdrive/to_car",mqmsg)


#MQTT callbacks
def on_connect(client,userdata,flags,rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(subscribe_topic)

def on_message(client, userdata, msg):
    rec = msg.payload.decode()
    print("recieved message: \"{}\" from topic \"{}\"".format(rec,msg.topic))

#called on exit
#close serial, disconnect MQTT

def cleanup():
    print("Ending and cleaning up")
    client.disconnect()

try:
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
    for msg in inport:
        handleNote(msg)

# handle app closure
except (KeyboardInterrupt):
    print("Interrupt received")
    cleanup()
except (RuntimeError):
    print("uh-oh! time to die")
    cleanup()