from django.urls import path

from .views import FirmwareManifestView, SemsemDetailWebView, SemsemListWebView

urlpatterns = [
    path("semsem/<str:uid_hex>/manifest", FirmwareManifestView.as_view()),
    path("api/semsems", SemsemListWebView.as_view()),
    path("api/semsems/<str:uid_hex>", SemsemDetailWebView.as_view()),
]
