import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/api/device_client.dart';
import 'package:nadeem_mobile/features/auth/auth_service.dart';
import 'package:nadeem_mobile/features/onboarding/services/device_discovery_service.dart';
import 'package:nadeem_mobile/features/onboarding/services/provision_service.dart';
import 'package:nadeem_mobile/features/onboarding/services/wifi_join_service.dart';

const kBackendUrl = 'https://api.nadeem.example';

final backendClientProvider = Provider<BackendClient>(
  (_) => BackendClient(kBackendUrl),
);

final deviceClientProvider = Provider<DeviceClient>(
  (_) => DeviceClient(),
);

final authServiceProvider = Provider<AuthService>((ref) {
  return AuthService(
    ref.read(backendClientProvider),
    const FlutterSecureStorage(),
  );
});

final wifiJoinServiceProvider = Provider<WifiJoinService>(
  (_) => WifiJoinService(),
);

final deviceDiscoveryServiceProvider = Provider<DeviceDiscoveryService>((ref) {
  return DeviceDiscoveryService(ref.read(deviceClientProvider));
});

final provisionServiceProvider = Provider<ProvisionService>((ref) {
  return ProvisionService(
    ref.read(deviceClientProvider),
    ref.read(backendClientProvider),
  );
});
