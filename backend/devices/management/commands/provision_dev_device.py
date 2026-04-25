from django.contrib.auth import get_user_model
from django.core.management.base import BaseCommand, CommandError

from devices.models import Device
from devices.tokens import generate_token, hash_token


class Command(BaseCommand):
    help = "Bind a device_id to a user without the mobile-app handshake. Dev only."
    def add_arguments(self, parser):
        parser.add_argument("--email", required=True)
        parser.add_argument("--device-id", required=True)
    def handle(self, *args, **opts):
        try:
            user = get_user_model().objects.get(email=opts["email"])
        except Exception as e:
            raise CommandError(f"no such user: {opts['email']}") from e
        device_id = opts["device_id"].lower()
        raw = generate_token()
        Device.objects.update_or_create(device_id=device_id, defaults={"user": user, "token_hash": hash_token(raw)})
        self.stdout.write(f"device_token: {raw}")
