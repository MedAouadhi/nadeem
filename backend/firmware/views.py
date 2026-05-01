from rest_framework.decorators import api_view, authentication_classes, permission_classes
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response

from devices.auth import DeviceTokenAuthentication


def _version_tuple(v: str) -> tuple:
    try:
        return tuple(int(x) for x in v.strip().split("."))
    except (ValueError, AttributeError):
        return (0,)


@api_view(["GET"])
@authentication_classes([DeviceTokenAuthentication])
@permission_classes([IsAuthenticated])
def ota_check(request):
    # DeviceTokenAuthentication returns (device.user, device) — device is request.auth
    device = request.auth
    current_version = request.query_params.get("current_version", "0.0.0")

    group = getattr(device, "release_group", None)
    if not group or not group.assigned_release:
        return Response({"has_update": False})

    target = group.assigned_release
    if _version_tuple(target.version) > _version_tuple(current_version):
        return Response(
            {
                "has_update": True,
                "version": target.version,
                "download_url": target.file.url if target.file else None,
                "changelog": target.changelog,
            }
        )

    return Response({"has_update": False})
