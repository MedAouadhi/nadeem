from django.urls import path
from .views import FirmwareManifestView

urlpatterns = [path("semsem/<str:uid_hex>/manifest", FirmwareManifestView.as_view())]
