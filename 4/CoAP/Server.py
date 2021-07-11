"""
    CaAP Server Implementation
    Author: Mohamad Jabbari Zadegan [9631015]
    Author: Mohamad ChamanMotlagh   [9631018]
"""

import aiocoap.resource as resource
import aiocoap
import logging
import asyncio

from aiocoap import *


class forwardIDResource(resource.ObservableResource):
    def __init__(self):
        super().__init__()
        self.tagID = ""

    def set_ID(self, tagID):
        self.tagID = tagID

    async def render_put(self, request):
        print('PUT payload: %s' % request.payload)
        self.set_ID(request.payload)

        # send to node 2 and get the response, then send the response
        protocol = await Context.create_client_context()

        request = Message(code=Code.PUT, uri='coap://192.168.1.114:5684/find_data', payload=self.get_ID())

        try:
            response = await protocol.request(request).response
        except Exception as e:
            print('Failed to fetch resource:')
            print(e)
        else:
            print('Result: %s\n%r' % (response.code, response.payload))

        return aiocoap.Message(code=aiocoap.CHANGED, payload=response.payload)

    def get_ID(self):
        return self.tagID





def server():
    # logging setup

    # logging.basicConfig(level=logging.INFO)
    # logging.getLogger("coap-server").setLevel(logging.DEBUG)
    #
    # Resource tree creation
    root = resource.Site()

    root.add_resource(['getData'], forwardIDResource())

    asyncio.Task(aiocoap.Context.create_server_context(root, bind=("192.168.1.114", 5683)))

    asyncio.get_event_loop().run_forever()


if __name__ == "__main__":
    server()
