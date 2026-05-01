from django.db import models


class FirmwareRelease(models.Model):
    version = models.CharField(max_length=32, unique=True)
    file = models.FileField(upload_to="firmware/", blank=True, null=True)
    changelog = models.TextField(blank=True, default="")
    is_stable = models.BooleanField(default=False)
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        ordering = ["-created_at"]

    def __str__(self) -> str:
        return f"v{self.version}"


class ReleaseGroup(models.Model):
    name = models.CharField(max_length=64, unique=True)
    description = models.TextField(blank=True, default="")
    assigned_release = models.ForeignKey(
        FirmwareRelease,
        on_delete=models.SET_NULL,
        null=True,
        blank=True,
        related_name="release_groups",
    )
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        ordering = ["name"]

    def __str__(self) -> str:
        return self.name
