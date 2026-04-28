import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:wifi_iot/wifi_iot.dart';
import 'package:nadeem_mobile/features/onboarding/services/wifi_join_service.dart';

class MockWifiPlugin extends Mock implements WifiPlugin {}

void main() {
  late MockWifiPlugin mockWifi;
  late WifiJoinService service;

  setUp(() {
    mockWifi = MockWifiPlugin();
    service = WifiJoinService(plugin: mockWifi);
  });

  test('joinSoftAP calls connect with NONE security', () async {
    when(() => mockWifi.connect(
          'Nadeem-A3F1',
          security: NetworkSecurity.NONE,
        )).thenAnswer((_) async => true);

    final result = await service.joinSoftAP('Nadeem-A3F1');
    expect(result, isTrue);
  });

  test('joinSoftAP returns false on failure', () async {
    when(() => mockWifi.connect(
          'Nadeem-A3F1',
          security: NetworkSecurity.NONE,
        )).thenAnswer((_) async => false);

    final result = await service.joinSoftAP('Nadeem-A3F1');
    expect(result, isFalse);
  });

  test('disconnect calls disconnect on plugin', () async {
    when(() => mockWifi.disconnect()).thenAnswer((_) async => true);
    await service.disconnect();
    verify(() => mockWifi.disconnect()).called(1);
  });
}
