import paho.mqtt.client as mqtt
import time
import RPi.GPIO as GPIO
import yaml

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)


with open("/home/pi/cancello/config.yaml", 'r') as ymlfile:
    configurazione = yaml.load(ymlfile)

ip_mqtt_server = configurazione['dati_mqtt']['ip_mqtt_server']
pin_rele_uno = configurazione['dati_pin']['pin_rele_uno']
command_topic = configurazione['dati_pin']['command_topic']

releuno = pin_rele_uno  # RELE' CANCELLO
GPIO.setup(releuno, GPIO.OUT)
GPIO.output(releuno, 1)

salta_mqtt = time.time() + float(5)

def comando_cancello():
    GPIO.output(releuno, 0)
    print "cancello azionato"
    time.sleep(1.5)
    GPIO.output(releuno, 1)
    print "azionamento cancello terminato"
    time.sleep(1)

def on_disconnect(client, userdata, rc):
     if rc != 0:
        print " disconnesso dal server"

def on_connect(client, userdata, flags, rc):
    client.subscribe(command_topic)
    print "connesso al server MQTT"

def on_message(client, userdata, msg):
     messaggio = str(msg.payload)
     topic = str(msg.topic)
     ora = time.time()
     if (messaggio == 'azione' and   topic == command_topic and  ora > salta_mqtt):
         comando_cancello()


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
try:
    client.connect(ip_mqtt_server, 1883, 60)
except :
    print "connessione errata"
client.on_disconnect = on_disconnect
client.loop_forever()
