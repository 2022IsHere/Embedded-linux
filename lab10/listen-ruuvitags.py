import argparse
import asyncio
import os
import json
import signal

os.environ["RUUVI_BLE_ADAPTER"] = "bleak"

import ruuvitag_sensor.log
from ruuvitag_sensor.ruuvi import RuuviTagSensor
import paho.mqtt.client as mqtt
from paho.mqtt import publish
from asyncio_paho import AsyncioPahoClient
from asyncio_paho.client import AsyncioMqttAuthError

def signal_handler(signal, frame):
    print("\nterminating gracefully.")
    client.disconnect()
    sys.exit(0)


def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT broker with result code {str(rc)}")

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("$SYS/#")

macs = ["E8:34:26:17:c6:4D"]

async def main():
    signal.signal(signal.SIGINT, signal_handler)
    ruuvitag_sensor.log.enable_console()
    parser = argparse.ArgumentParser(
        description="Program relays Ruuvitag BLE temperature and humidity" "advertisements to MQTT broker."
    )
    parser.add_argument("-b", "--broker", dest="mqtt_broker", required=True, help="mqtt broker address, ip or fqdn")
    parser.add_argument("-t", "--topic", dest="mqtt_topic", required=True, help="mqtt topic, e.g. ruuvitag/sauna")
    args = parser.parse_args()

    mqtt_broker = args.mqtt_broker
    mqtt_topic = args.mqtt_topic

    async with AsyncioPahoClient() as client:
        client.on_connect = on_connect
        client.connect_async(mqtt_broker, 1883, 60)
    async for data in RuuviTagSensor.get_data_async(macs):
        #print(data)
        # Flatten the nested structure
        flat_data = {"mac": data[0]}
        for key, value in data[1].items():
            if value is not None:
                flat_data[f"data_{key}"] = value
        mqtt_msg = json.dumps(flat_data)
        print(mqtt_msg)
        publish.single(mqtt_topic, mqtt_msg, hostname=mqtt_broker)
        print(".", end="", flush=True)


if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())

