// test/features/onboarding/provision_screen_test.dart
import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/api/device_client.dart';
import 'package:nadeem_mobile/core/models/device.dart';
import 'package:nadeem_mobile/features/onboarding/services/provision_service.dart';

class MockDeviceClient extends Mock implements DeviceClient {}
class MockBackendClient extends Mock implements BackendClient {}

void main() {
  late MockDeviceClient mockDevice;
  late MockBackendClient mockBackend;
  late ProvisionService service;

  setUp(() {
    mockDevice = MockDeviceClient();
    mockBackend = MockBackendClient();
    service = ProvisionService(
      mockDevice,
      mockBackend,
      pollInterval: const Duration(milliseconds: 50),
      pollTimeout: const Duration(milliseconds: 500),
    );
  });

  test('provision sends credentials then polls until bootstrapped', () async {
    when(() => mockDevice.provision(
      ssid: 'HomeWifi',
      password: 'hunter2',
      provisionToken: 'tok_xyz',
    )).thenAnswer((_) async => true);

    var callCount = 0;
    when(() => mockBackend.getDevice('aabbccddeeff', 'jwt_abc'))
        .thenAnswer((_) async {
      callCount++;
      return Device(
        deviceId: 'aabbccddeeff',
        onlineStatus: callCount >= 3,
        bootstrapped: callCount >= 3,
      );
    });

    final device = await service.provisionAndWait(
      ssid: 'HomeWifi',
      password: 'hunter2',
      provisionToken: 'tok_xyz',
      deviceId: 'aabbccddeeff',
      jwt: 'jwt_abc',
    );

    expect(device.bootstrapped, isTrue);
    expect(callCount, 3);
  });

  test('throws ProvisionTimeoutError when device never bootstraps', () async {
    when(() => mockDevice.provision(
      ssid: 'HomeWifi',
      password: 'hunter2',
      provisionToken: 'tok_xyz',
    )).thenAnswer((_) async => true);

    when(() => mockBackend.getDevice('aabbccddeeff', 'jwt_abc'))
        .thenAnswer((_) async => const Device(
          deviceId: 'aabbccddeeff',
          onlineStatus: false,
          bootstrapped: false,
        ));

    await expectLater(
      () => service.provisionAndWait(
        ssid: 'HomeWifi',
        password: 'hunter2',
        provisionToken: 'tok_xyz',
        deviceId: 'aabbccddeeff',
        jwt: 'jwt_abc',
      ),
      throwsA(isA<ProvisionTimeoutError>()),
    );
  });
}
