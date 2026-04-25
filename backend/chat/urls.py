from django.urls import path

from .views import ChatSessionDetailView, ChatSessionListView

urlpatterns = [
    path("api/chat-sessions", ChatSessionListView.as_view()),
    path("api/chat-sessions/<int:pk>", ChatSessionDetailView.as_view()),
]
