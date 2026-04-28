import 'package:dio/dio.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';

class MockDio extends Mock implements Dio {}

void main() {
  late MockDio mockDio;
  late BackendClient client;

  setUp(() {
    mockDio = MockDio();
    client = BackendClient.withDio(mockDio);
  });

  group('login', () {
    test('returns token on 200', () async {
      when(() => mockDio.post<Map<String, dynamic>>(
        '/api/auth/login',
        data: any(named: 'data'),
      )).thenAnswer((_) async => Response(
        data: {'token': 'jwt_abc'},
        statusCode: 200,
        requestOptions: RequestOptions(path: '/api/auth/login'),
      ));

      final token = await client.login('user@example.com', 'pass');
      expect(token, 'jwt_abc');
    });

    test('throws BackendAuthException on 401', () async {
      when(() => mockDio.post<Map<String, dynamic>>(
        '/api/auth/login',
        data: any(named: 'data'),
      )).thenThrow(DioException(
        requestOptions: RequestOptions(path: '/api/auth/login'),
        response: Response(
          statusCode: 401,
          requestOptions: RequestOptions(path: '/api/auth/login'),
        ),
        type: DioExceptionType.badResponse,
      ));

      expect(
        () => client.login('user@example.com', 'wrong'),
        throwsA(isA<BackendAuthException>()),
      );
    });
  });

  group('mintProvisionToken', () {
    test('returns ProvisionToken on 200', () async {
      when(() => mockDio.post<Map<String, dynamic>>(
        '/api/provisioning-tokens',
        options: any(named: 'options'),
      )).thenAnswer((_) async => Response(
        data: {
          'provision_token': 'tok_xyz',
          'expires_at': '2026-04-28T22:00:00Z',
        },
        statusCode: 200,
        requestOptions: RequestOptions(path: '/api/provisioning-tokens'),
      ));

      final pt = await client.mintProvisionToken('jwt_abc');
      expect(pt.token, 'tok_xyz');
    });
  });

  group('getDevice', () {
    test('returns Device when bootstrapped', () async {
      when(() => mockDio.get<Map<String, dynamic>>(
        '/api/devices/aabbccddeeff',
        options: any(named: 'options'),
      )).thenAnswer((_) async => Response(
        data: {
          'device_id': 'aabbccddeeff',
          'online_status': true,
          'bootstrapped': true,
        },
        statusCode: 200,
        requestOptions: RequestOptions(path: '/api/devices/aabbccddeeff'),
      ));

      final dev = await client.getDevice('aabbccddeeff', 'jwt_abc');
      expect(dev.bootstrapped, isTrue);
    });
  });

  group('listDevices', () {
    test('returns list of Devices', () async {
      when(() => mockDio.get<List<dynamic>>(
        '/api/devices/',
        options: any(named: 'options'),
      )).thenAnswer((_) async => Response(
        data: [
          {'device_id': 'aabbccddeeff', 'online_status': true, 'bootstrapped': true},
          {'device_id': '112233445566', 'online_status': false, 'bootstrapped': true},
        ],
        statusCode: 200,
        requestOptions: RequestOptions(path: '/api/devices/'),
      ));

      final devices = await client.listDevices('jwt_abc');
      expect(devices.length, 2);
      expect(devices[0].deviceId, 'aabbccddeeff');
    });
  });
}
