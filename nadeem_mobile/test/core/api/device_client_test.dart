import 'package:dio/dio.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:nadeem_mobile/core/api/device_client.dart';
import 'package:nadeem_mobile/core/models/device_info.dart';
import 'package:nadeem_mobile/core/models/wifi_network.dart';

class MockDio extends Mock implements Dio {}

void main() {
  late MockDio mockDio;
  late DeviceClient client;

  setUp(() {
    mockDio = MockDio();
    client = DeviceClient.withDio(mockDio);
  });

  group('fetchInfo', () {
    test('returns DeviceInfo on 200', () async {
      when(() => mockDio.get<Map<String, dynamic>>('/info')).thenAnswer(
        (_) async => Response(
          data: {
            'device_id': 'aabbccddeeff',
            'fw': '0.1.0',
            'status': 'onboarding',
          },
          statusCode: 200,
          requestOptions: RequestOptions(path: '/info'),
        ),
      );

      final info = await client.fetchInfo();
      expect(info.deviceId, 'aabbccddeeff');
      expect(info.status, 'onboarding');
    });

    test('throws DeviceNotReachableError on connection failure', () async {
      when(() => mockDio.get<Map<String, dynamic>>('/info')).thenThrow(
        DioException(
          requestOptions: RequestOptions(path: '/info'),
          type: DioExceptionType.connectionTimeout,
        ),
      );

      expect(() => client.fetchInfo(), throwsA(isA<DeviceNotReachableError>()));
    });
  });

  group('scanNetworks', () {
    test('returns list of WifiNetwork', () async {
      when(() => mockDio.get<List<dynamic>>('/scan')).thenAnswer(
        (_) async => Response(
          data: [
            {'ssid': 'HomeWifi', 'rssi': -48, 'auth': 3},
            {'ssid': 'Neighbour5G', 'rssi': -71, 'auth': 4},
          ],
          statusCode: 200,
          requestOptions: RequestOptions(path: '/scan'),
        ),
      );

      final networks = await client.scanNetworks();
      expect(networks.length, 2);
      expect(networks[0].ssid, 'HomeWifi');
      expect(networks[1].authLabel, 'WPA/WPA2-PSK');
    });
  });

  group('provision', () {
    test('returns true on 200 ok:true', () async {
      when(() => mockDio.post<Map<String, dynamic>>(
        '/provision',
        data: any(named: 'data'),
        options: any(named: 'options'),
      )).thenAnswer((_) async => Response(
        data: {'ok': true},
        statusCode: 200,
        requestOptions: RequestOptions(path: '/provision'),
      ));

      final ok = await client.provision(
        ssid: 'HomeWifi',
        password: 'hunter2',
        provisionToken: 'tok_xyz',
      );
      expect(ok, isTrue);
    });

    test('treats TCP reset after 200 as success', () async {
      when(() => mockDio.post<Map<String, dynamic>>(
        '/provision',
        data: any(named: 'data'),
        options: any(named: 'options'),
      )).thenThrow(DioException(
        requestOptions: RequestOptions(path: '/provision'),
        type: DioExceptionType.connectionError,
        response: Response(
          statusCode: 200,
          data: {'ok': true},
          requestOptions: RequestOptions(path: '/provision'),
        ),
      ));

      final ok = await client.provision(
        ssid: 'HomeWifi',
        password: 'hunter2',
        provisionToken: 'tok_xyz',
      );
      expect(ok, isTrue);
    });

    test('throws ProvisionValidationError on 400', () async {
      when(() => mockDio.post<Map<String, dynamic>>(
        '/provision',
        data: any(named: 'data'),
        options: any(named: 'options'),
      )).thenThrow(DioException(
        requestOptions: RequestOptions(path: '/provision'),
        response: Response(
          statusCode: 400,
          data: 'missing ssid',
          requestOptions: RequestOptions(path: '/provision'),
        ),
        type: DioExceptionType.badResponse,
      ));

      expect(
        () => client.provision(ssid: '', password: '', provisionToken: 'tok'),
        throwsA(isA<ProvisionValidationError>()),
      );
    });
  });
}
