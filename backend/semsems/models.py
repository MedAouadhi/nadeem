from django.core.exceptions import ValidationError
from django.core.validators import RegexValidator
from django.db import models

UID_RE = RegexValidator(r"^[0-9a-f]{2,20}$", "uid_hex must be 2..20 lowercase hex chars")

class SemsemGroup(models.Model):
    name = models.CharField(max_length=64, unique=True)
    description = models.TextField(blank=True, default="")
    icon = models.CharField(max_length=8, blank=True, default="")

    class Meta:
        ordering = ["name"]

    def __str__(self) -> str:
        return self.name


class Semsem(models.Model):
    uid_hex = models.CharField(max_length=20, unique=True, validators=[UID_RE])
    title = models.CharField(max_length=120)
    is_pro = models.BooleanField(default=False)
    role = models.CharField(max_length=31, blank=True, default="")
    created_at = models.DateTimeField(auto_now_add=True)
    groups = models.ManyToManyField(
        SemsemGroup,
        blank=True,
        related_name="semsems",
    )

    def clean(self):
        if self.uid_hex != self.uid_hex.lower():
            raise ValidationError("uid_hex must be lowercase")
        if self.is_pro and not self.role:
            raise ValidationError("pro semsem requires non-empty role")
        if not self.is_pro and self.role:
            raise ValidationError("regular semsem must not have a role")
        if len(self.role.encode("utf-8")) > 31:
            raise ValidationError("role exceeds 31 bytes")

    def save(self, *a, **kw):
        self.full_clean()
        return super().save(*a, **kw)

class Track(models.Model):
    semsem = models.ForeignKey(Semsem, on_delete=models.CASCADE, related_name="tracks")
    name = models.CharField(max_length=64)
    file = models.FileField(upload_to="audio/")
    position = models.PositiveIntegerField(default=0)

    class Meta:
        ordering = ["position", "id"]
        unique_together = [("semsem", "name")]
