from types import SimpleNamespace

import paho.mqtt.client as mqtt
import json


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("RFID")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("Message received; topic: " + msg.topic + ", payload: " + str(msg.payload))
    if msg.topic == "RFID":
        f = open('dataset.json')
        rfid = str(msg.payload)[3:14]
        dataset = json.load(f, object_hook=lambda d: SimpleNamespace(**d))
        for data in dataset:
            if data.RFID == rfid:
                print("Publishing [ " + str(data) + " ]")
                client.publish("temperature", payload=data.temp, qos=0, retain=False)
                client.publish("water", payload=data.water, qos=0, retain=False)
        f.close()


def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("192.168.1.114", 1883, 60)

    # Blocking call that processes network traffic, dispatches callbacks and
    # handles reconnecting.
    # Other loop*() functions are available that give a threaded interface and a
    # manual interface.
    client.loop_forever()


if __name__ == "__main__":
    main()
