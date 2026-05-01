import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:nadeem_mobile/core/api/device_client.dart';
import 'package:nadeem_mobile/core/models/device_info.dart';
import 'package:nadeem_mobile/features/onboarding/services/device_discovery_service.dart';

class MockDeviceClient extends Mock implements DeviceClient {}

const _kTestInfo = DeviceInfo(
  deviceId: 'aabbccddeeff',
  fw: '0.1.0',
  status: 'onboarding',
);

void main() {
  late MockDeviceClient mockClient;
  late DeviceDiscoveryService service;

  setUp(() {
    mockClient = MockDeviceClient();
    service = DeviceDiscoveryService(
      mockClient,
      probeInterval: const Duration(milliseconds: 50),
      timeout: const Duration(milliseconds: 500),
    );
  });

  test('returns DeviceInfo when device responds immediately', () async {
    when(() => mockClient.fetchInfo()).thenAnswer((_) async => _kTestInfo);

    final info = await service.discover();
    expect(info.deviceId, 'aabbccddeeff');
  });

  test('retries until device responds', () async {
    var callCount = 0;
    when(() => mockClient.fetchInfo()).thenAnswer((_) async {
      callCount++;
      if (callCount < 3) throw DeviceNotReachableError();
      return _kTestInfo;
    });

    final info = await service.discover();
    expect(info.deviceId, 'aabbccddeeff');
    expect(callCount, 3);
  });

  test('throws DeviceDiscoveryTimeoutError after timeout', () async {
    when(() => mockClient.fetchInfo()).thenThrow(DeviceNotReachableError());

    expect(
      () => service.discover(),
      throwsA(isA<DeviceDiscoveryTimeoutError>()),
    );
  });
}
