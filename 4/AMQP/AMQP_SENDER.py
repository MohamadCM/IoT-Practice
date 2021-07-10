import pika


def callback(ch, method, properties, body):
    print(" [x] Received %r" % body)


def main():
    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
    channel = connection.channel()
    channel.queue_declare(queue='RFID')
    channel.basic_publish(exchange='',
                          routing_key='RFID',
                          body='70 b2 e2 a4')
    print("Sent RFID value")
    connection.close()

if __name__ == "__main__":
    main()
