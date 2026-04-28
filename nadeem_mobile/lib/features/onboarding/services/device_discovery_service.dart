import 'package:nadeem_mobile/core/api/device_client.dart';
import 'package:nadeem_mobile/core/models/device_info.dart';

class DeviceDiscoveryTimeoutError implements Exception {}

class DeviceDiscoveryService {
  final DeviceClient _client;
  final Duration _probeInterval;
  final Duration _timeout;

  DeviceDiscoveryService(
    this._client, {
    Duration probeInterval = const Duration(seconds: 2),
    Duration timeout = const Duration(seconds: 30),
  })  : _probeInterval = probeInterval,
        _timeout = timeout;

  Future<DeviceInfo> discover() async {
    final deadline = DateTime.now().add(_timeout);

    while (DateTime.now().isBefore(deadline)) {
      try {
        final info = await _client.fetchInfo();
        if (info.fw.isNotEmpty) return info;
      } on DeviceNotReachableError {
        // probe again after interval
      }
      await Future.delayed(_probeInterval);
    }

    throw DeviceDiscoveryTimeoutError();
  }
}
