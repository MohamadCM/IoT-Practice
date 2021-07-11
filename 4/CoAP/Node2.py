"""
    CaAP Node-2 Implementation
    Author: Mohamad Jabbari Zadegan [9631015]
    Author: Mohamad ChamanMotlagh   [9631018]
"""

import json
from types import SimpleNamespace

import pandas as pd
import aiocoap.resource as resource
import aiocoap
import logging
import asyncio

from aiocoap import *


class findDataResource(resource.ObservableResource):
    def __init__(self):
        super().__init__()
        self.tagID = ""

    def set_ID(self, tagID):
        self.tagID = tagID

    async def render_put(self, request):
        print('PUT payload: %s' % request.payload)
        self.set_ID(request.payload)

        #find data for the current id

        payload = data_query(self.tagID)

        return aiocoap.Message(code=aiocoap.CHANGED, payload=payload)

    def get_ID(self):
        return self.tagID





def server():
    # logging setup
    #
    # logging.basicConfig(level=logging.INFO)
    # logging.getLogger("coap-server").setLevel(logging.DEBUG)

    # Resource tree creation
    root = resource.Site()

    root.add_resource(['find_data'], findDataResource())

    asyncio.Task(aiocoap.Context.create_server_context(root, bind=("192.168.1.114", 5684)))

    asyncio.get_event_loop().run_forever()


def data_query(message):
    f = open('dataset.json')
    rfid = (str(message)[3:14]).lower()
    dataset = json.load(f, object_hook=lambda d: SimpleNamespace(**d))
    payload = ""
    for data in dataset:
        if data.RFID == rfid:
            print("Sending back [ " + str(data) + " ]")
            payload = bytes("temperature=" + str(data.temp) + ",water=" + str(data.water), 'utf-8')
    return payload


if __name__ == "__main__":
    server()

