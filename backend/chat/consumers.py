from channels.generic.websocket import AsyncWebsocketConsumer

class ChatConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        if not self.scope.get("device"):
            await self.close(code=4401)
            return
        await self.accept()

    async def disconnect(self, code):
        pass
