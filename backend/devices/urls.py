from django.urls import path
from .views import (
    ProvisioningTokenCreateView,
    BootstrapView,
    DevProvisionView,
    DeviceListView,
    DeviceDestroyView,
)
urlpatterns = [
    path("api/provisioning-tokens", ProvisioningTokenCreateView.as_view()),
    path("bootstrap", BootstrapView.as_view()),
    path("api/dev/devices", DevProvisionView.as_view()),
    path("api/devices", DeviceListView.as_view()),
    path("api/devices/<str:device_id>", DeviceDestroyView.as_view()),
]

