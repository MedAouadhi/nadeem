from rest_framework import generics
from devices.auth import DeviceTokenAuthentication
from .models import Semsem
from .serializers import FirmwareManifestSerializer


class FirmwareManifestView(generics.RetrieveAPIView):
    authentication_classes = [DeviceTokenAuthentication]
    serializer_class = FirmwareManifestSerializer
    queryset = Semsem.objects.prefetch_related("tracks")
    lookup_field = "uid_hex"
    lookup_url_kwarg = "uid_hex"

    def get_object(self):
        self.kwargs["uid_hex"] = self.kwargs["uid_hex"].lower()
        return super().get_object()
