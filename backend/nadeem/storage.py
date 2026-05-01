from django.conf import settings
from storages.backends.s3 import S3Storage


class PublicS3Storage(S3Storage):
    """S3Storage that serves file URLs via S3_PUBLIC_ENDPOINT_URL.

    Uploads use the internal endpoint_url (e.g. http://minio:9000 in Docker),
    while URLs served to browsers use S3_PUBLIC_ENDPOINT_URL so they're reachable
    outside the container network.
    """

    def url(self, name, parameters=None, expire=None, http_method=None):
        url = super().url(name, parameters=parameters, expire=expire, http_method=http_method)
        public = getattr(settings, "S3_PUBLIC_ENDPOINT_URL", None)
        if public and self.endpoint_url:
            internal = self.endpoint_url.rstrip("/")
            public = public.rstrip("/")
            if internal != public:
                url = url.replace(internal, public, 1)
        return url
