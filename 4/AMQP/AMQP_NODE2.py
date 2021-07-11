"""
    AMQP Node-2 Implementation
    Author: Mohamad ChamanMotlagh   [9631018]
"""
from types import SimpleNamespace

import pika
import json


def callback(ch, method, properties, body):
    print(" [x] Received %r" % body)
    message = body
    f = open('dataset.json')
    rfid = str(message)[2:13]
    dataset = json.load(f, object_hook=lambda d: SimpleNamespace(**d))
    for data in dataset:
        if data.RFID == rfid:
            print("Publishing [ " + str(data) + " ]")
            send(data.water, data.temp)
    f.close()


def send(water, temperature):
    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
    channel = connection.channel()
    channel.queue_declare(queue='water')
    channel.basic_publish(exchange='',
                          routing_key='water',
                          body=str(water))
    channel.queue_declare(queue='temperature')
    channel.basic_publish(exchange='',
                          routing_key='temperature',
                          body=str(temperature))

    connection.close()


def receive():
    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
    channel = connection.channel()
    channel.queue_declare(queue='RFID')
    channel.basic_consume(queue='RFID',
                          auto_ack=True,
                          on_message_callback=callback)
    print(' [*] Waiting for messages. To exit press CTRL+C')
    channel.start_consuming()


def main():
    receive()


if __name__ == "__main__":
    main()
