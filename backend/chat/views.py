from rest_framework import generics
from rest_framework.permissions import IsAuthenticated
from rest_framework_simplejwt.authentication import JWTAuthentication

from .models import ProChatSession
from .serializers import ProChatSessionDetailSerializer, ProChatSessionListSerializer


class _MyMixin:
    authentication_classes = [JWTAuthentication]
    permission_classes = [IsAuthenticated]
    def get_queryset(self):
        return ProChatSession.objects.filter(device__user=self.request.user).select_related("device")

class ChatSessionListView(_MyMixin, generics.ListAPIView):
    serializer_class = ProChatSessionListSerializer

class ChatSessionDetailView(_MyMixin, generics.RetrieveAPIView):
    serializer_class = ProChatSessionDetailSerializer
    def get_queryset(self):
        return super().get_queryset().prefetch_related("entries")
