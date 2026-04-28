import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/features/auth/auth_service.dart';

class MockBackendClient extends Mock implements BackendClient {}
class MockSecureStorage extends Mock implements FlutterSecureStorage {}

void main() {
  late MockBackendClient mockClient;
  late MockSecureStorage mockStorage;
  late AuthService authService;

  setUp(() {
    mockClient = MockBackendClient();
    mockStorage = MockSecureStorage();
    authService = AuthService(mockClient, mockStorage);
  });

  test('login stores token in secure storage', () async {
    when(() => mockClient.login('user@x.com', 'pass'))
        .thenAnswer((_) async => 'jwt_abc');
    when(() => mockStorage.write(
          key: AuthService.jwtKey,
          value: 'jwt_abc',
        )).thenAnswer((_) async {});

    await authService.login('user@x.com', 'pass');

    verify(() => mockStorage.write(key: AuthService.jwtKey, value: 'jwt_abc'))
        .called(1);
  });

  test('getToken reads from secure storage', () async {
    when(() => mockStorage.read(key: AuthService.jwtKey))
        .thenAnswer((_) async => 'jwt_abc');

    final token = await authService.getToken();
    expect(token, 'jwt_abc');
  });

  test('logout deletes token from secure storage', () async {
    when(() => mockStorage.delete(key: AuthService.jwtKey))
        .thenAnswer((_) async {});

    await authService.logout();
    verify(() => mockStorage.delete(key: AuthService.jwtKey)).called(1);
  });

  test('isLoggedIn returns false when no token', () async {
    when(() => mockStorage.read(key: AuthService.jwtKey))
        .thenAnswer((_) async => null);

    expect(await authService.isLoggedIn(), isFalse);
  });
}
