// lib/features/onboarding/services/provision_service.dart
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/api/device_client.dart';
import 'package:nadeem_mobile/core/models/device.dart';

class ProvisionTimeoutError implements Exception {}

class ProvisionService {
  final DeviceClient _deviceClient;
  final BackendClient _backendClient;
  final Duration _pollInterval;
  final Duration _pollTimeout;

  ProvisionService(
    this._deviceClient,
    this._backendClient, {
    Duration pollInterval = const Duration(seconds: 3),
    Duration pollTimeout = const Duration(seconds: 60),
  })  : _pollInterval = pollInterval,
        _pollTimeout = pollTimeout;

  Future<Device> provisionAndWait({
    required String ssid,
    required String password,
    required String provisionToken,
    required String deviceId,
    required String jwt,
  }) async {
    await _deviceClient.provision(
      ssid: ssid,
      password: password,
      provisionToken: provisionToken,
    );

    final deadline = DateTime.now().add(_pollTimeout);
    while (DateTime.now().isBefore(deadline)) {
      try {
        final device = await _backendClient.getDevice(deviceId, jwt);
        if (device.bootstrapped) return device;
      } catch (_) {
        // backend may be momentarily unreachable — keep polling
      }
      await Future.delayed(_pollInterval);
    }

    throw ProvisionTimeoutError();
  }
}
