# Nadeem Flutter Companion App — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the Nadeem Flutter companion app that authenticates a parent, provisions a new Nadeem device onto home Wi-Fi, and polls the backend until the device is confirmed online.

**Architecture:** Single-responsibility app with an auth layer (JWT stored in secure storage), a backend API client (user-session endpoints), a device-local HTTP client (192.168.4.1 while on SoftAP), and platform channel abstractions for iOS (NEHotspotConfiguration) and Android (WifiNetworkSpecifier) Wi-Fi joining. Navigation is screen-by-screen; state lives in Riverpod providers local to the onboarding flow.

**Tech Stack:** Flutter 3.x, Dart 3, Riverpod 2 (state), Dio (HTTP), flutter_secure_storage (token), wifi_iot (platform Wi-Fi join/scan), go_router (navigation), freezed + json_serializable (models), mocktail (unit tests).

---

## File Map

| Path | Responsibility |
|------|---------------|
| `pubspec.yaml` | Dependencies |
| `lib/main.dart` | App entry, provider scope |
| `lib/app.dart` | GoRouter config, MaterialApp |
| `lib/core/api/backend_client.dart` | Dio client for `/api/*` (auth, provisioning-tokens, devices) |
| `lib/core/api/device_client.dart` | Dio client for `http://192.168.4.1` (/info, /scan, /provision, /reset) |
| `lib/core/models/provision_token.dart` | `{provision_token, expires_at}` |
| `lib/core/models/device_info.dart` | `{device_id, fw, status}` |
| `lib/core/models/wifi_network.dart` | `{ssid, rssi, auth}` + `authLabel` helper |
| `lib/core/models/device.dart` | `{device_id, online_status, bootstrapped}` from `GET /api/devices/<id>` |
| `lib/features/auth/auth_service.dart` | Login, logout, JWT persist/retrieve |
| `lib/features/auth/login_screen.dart` | Email + password form, calls auth_service |
| `lib/features/onboarding/services/wifi_join_service.dart` | Platform-agnostic Wi-Fi join (iOS / Android) |
| `lib/features/onboarding/services/device_discovery_service.dart` | Probe `GET /info`, return `DeviceInfo` |
| `lib/features/onboarding/screens/onboarding_start_screen.dart` | "Power on Nadeem" instruction screen |
| `lib/features/onboarding/screens/wifi_join_screen.dart` | Detect/join `Nadeem-XXXX` SoftAP |
| `lib/features/onboarding/screens/network_select_screen.dart` | Show /scan results, user picks SSID + enters password |
| `lib/features/onboarding/screens/provision_screen.dart` | POST /provision, then poll backend, show progress |
| `lib/features/onboarding/screens/success_screen.dart` | "Nadeem is online" confirmation |
| `lib/features/dashboard/dashboard_screen.dart` | Device list + "Add Device" CTA |
| `android/app/src/main/AndroidManifest.xml` | ACCESS_WIFI_STATE, CHANGE_WIFI_STATE, ACCESS_FINE_LOCATION |
| `ios/Runner/Info.plist` | NEHotspotConfiguration usage description |
| `ios/Runner/Runner.entitlements` | `com.apple.developer.networking.wifi-info` |
| `test/core/api/backend_client_test.dart` | Backend client unit tests |
| `test/core/api/device_client_test.dart` | Device client unit tests |
| `test/core/models/wifi_network_test.dart` | authLabel helper tests |
| `test/features/auth/auth_service_test.dart` | Login + token storage tests |
| `test/features/onboarding/device_discovery_service_test.dart` | /info probe tests |
| `test/features/onboarding/provision_screen_test.dart` | Provision + poll integration tests |

---

## Task 1: Flutter Project Scaffold

**Files:**
- Create: `nadeem_mobile/` (run from repo root)
- Create: `nadeem_mobile/pubspec.yaml`
- Create: `nadeem_mobile/analysis_options.yaml`

- [ ] **Step 1: Create the Flutter project**

```bash
flutter create --org com.nadeem --project-name nadeem_mobile --platforms android,ios nadeem_mobile
cd nadeem_mobile
```

- [ ] **Step 2: Write `pubspec.yaml`**

Replace the generated `pubspec.yaml` with:

```yaml
name: nadeem_mobile
description: Nadeem device companion app — onboarding and provisioning.
publish_to: 'none'
version: 0.1.0+1

environment:
  sdk: '>=3.0.0 <4.0.0'

dependencies:
  flutter:
    sdk: flutter
  # HTTP
  dio: ^5.4.3
  # State management
  flutter_riverpod: ^2.5.1
  riverpod_annotation: ^2.3.5
  # Navigation
  go_router: ^14.2.7
  # Models
  freezed_annotation: ^2.4.1
  json_annotation: ^4.9.0
  # Secure storage
  flutter_secure_storage: ^9.2.2
  # Platform Wi-Fi
  wifi_iot: ^0.3.19

dev_dependencies:
  flutter_test:
    sdk: flutter
  # Code generation
  build_runner: ^2.4.9
  freezed: ^2.5.2
  json_serializable: ^6.8.0
  riverpod_generator: ^2.4.0
  # Tests
  mocktail: ^1.0.3

flutter:
  uses-material-design: true
```

- [ ] **Step 3: Write `analysis_options.yaml`**

```yaml
include: package:flutter/analysis_options.yaml

analyzer:
  errors:
    invalid_annotation_target: ignore

linter:
  rules:
    prefer_const_constructors: true
    prefer_const_literals_to_create_immutables: true
```

- [ ] **Step 4: Install dependencies**

```bash
flutter pub get
```

Expected: all packages resolve, no version conflicts.

- [ ] **Step 5: Commit**

```bash
git add nadeem_mobile/
git commit -m "feat(mobile): scaffold Flutter project with dependencies"
```

---

## Task 2: Core Models

**Files:**
- Create: `nadeem_mobile/lib/core/models/provision_token.dart`
- Create: `nadeem_mobile/lib/core/models/device_info.dart`
- Create: `nadeem_mobile/lib/core/models/wifi_network.dart`
- Create: `nadeem_mobile/lib/core/models/device.dart`
- Test: `nadeem_mobile/test/core/models/wifi_network_test.dart`

- [ ] **Step 1: Write failing test for `WifiNetwork.authLabel`**

```dart
// test/core/models/wifi_network_test.dart
import 'package:flutter_test/flutter_test.dart';
import 'package:nadeem_mobile/core/models/wifi_network.dart';

void main() {
  group('WifiNetwork.authLabel', () {
    test('auth 0 → Open', () {
      const net = WifiNetwork(ssid: 'x', rssi: -50, auth: 0);
      expect(net.authLabel, 'Open');
    });

    test('auth 3 → WPA2-PSK', () {
      const net = WifiNetwork(ssid: 'x', rssi: -50, auth: 3);
      expect(net.authLabel, 'WPA2-PSK');
    });

    test('auth 9 → OWE', () {
      const net = WifiNetwork(ssid: 'x', rssi: -50, auth: 9);
      expect(net.authLabel, 'OWE');
    });

    test('unknown auth → Unknown', () {
      const net = WifiNetwork(ssid: 'x', rssi: -50, auth: 99);
      expect(net.authLabel, 'Unknown');
    });

    test('isOpen true when auth 0', () {
      const net = WifiNetwork(ssid: 'x', rssi: -50, auth: 0);
      expect(net.isOpen, isTrue);
    });

    test('isOpen false when auth 3', () {
      const net = WifiNetwork(ssid: 'x', rssi: -50, auth: 3);
      expect(net.isOpen, isFalse);
    });
  });
}
```

- [ ] **Step 2: Run test — expect FAIL (class not found)**

```bash
cd nadeem_mobile && flutter test test/core/models/wifi_network_test.dart
```

Expected: compilation error `Target of URI doesn't exist`.

- [ ] **Step 3: Write `wifi_network.dart`**

```dart
// lib/core/models/wifi_network.dart
import 'package:freezed_annotation/freezed_annotation.dart';

part 'wifi_network.freezed.dart';
part 'wifi_network.g.dart';

@freezed
class WifiNetwork with _$WifiNetwork {
  const factory WifiNetwork({
    required String ssid,
    required int rssi,
    required int auth,
  }) = _WifiNetwork;

  factory WifiNetwork.fromJson(Map<String, dynamic> json) =>
      _$WifiNetworkFromJson(json);

  const WifiNetwork._();

  static const _authLabels = {
    0: 'Open',
    1: 'WEP',
    2: 'WPA-PSK',
    3: 'WPA2-PSK',
    4: 'WPA/WPA2-PSK',
    5: 'Enterprise',
    6: 'WPA3-PSK',
    7: 'WPA2/WPA3-PSK',
    8: 'WAPI-PSK',
    9: 'OWE',
  };

  String get authLabel => _authLabels[auth] ?? 'Unknown';
  bool get isOpen => auth == 0;
}
```

- [ ] **Step 4: Write `device_info.dart`**

```dart
// lib/core/models/device_info.dart
import 'package:freezed_annotation/freezed_annotation.dart';

part 'device_info.freezed.dart';
part 'device_info.g.dart';

@freezed
class DeviceInfo with _$DeviceInfo {
  const factory DeviceInfo({
    @JsonKey(name: 'device_id') required String deviceId,
    required String fw,
    required String status,
  }) = _DeviceInfo;

  factory DeviceInfo.fromJson(Map<String, dynamic> json) =>
      _$DeviceInfoFromJson(json);
}
```

- [ ] **Step 5: Write `provision_token.dart`**

```dart
// lib/core/models/provision_token.dart
import 'package:freezed_annotation/freezed_annotation.dart';

part 'provision_token.freezed.dart';
part 'provision_token.g.dart';

@freezed
class ProvisionToken with _$ProvisionToken {
  const factory ProvisionToken({
    @JsonKey(name: 'provision_token') required String token,
    @JsonKey(name: 'expires_at') required DateTime expiresAt,
  }) = _ProvisionToken;

  factory ProvisionToken.fromJson(Map<String, dynamic> json) =>
      _$ProvisionTokenFromJson(json);
}
```

- [ ] **Step 6: Write `device.dart`**

```dart
// lib/core/models/device.dart
import 'package:freezed_annotation/freezed_annotation.dart';

part 'device.freezed.dart';
part 'device.g.dart';

@freezed
class Device with _$Device {
  const factory Device({
    @JsonKey(name: 'device_id') required String deviceId,
    @JsonKey(name: 'online_status') required bool onlineStatus,
    @JsonKey(name: 'bootstrapped') @Default(false) bool bootstrapped,
    @JsonKey(name: 'name') String? name,
  }) = _Device;

  factory Device.fromJson(Map<String, dynamic> json) =>
      _$DeviceFromJson(json);
}
```

- [ ] **Step 7: Run code generation**

```bash
flutter pub run build_runner build --delete-conflicting-outputs
```

Expected: generates `*.freezed.dart` and `*.g.dart` for all four models.

- [ ] **Step 8: Run tests — expect PASS**

```bash
flutter test test/core/models/wifi_network_test.dart
```

Expected: 6 tests pass.

- [ ] **Step 9: Commit**

```bash
git add lib/core/models/ test/core/models/
git commit -m "feat(mobile): add core models (DeviceInfo, WifiNetwork, ProvisionToken, Device)"
```

---

## Task 3: Backend API Client

**Files:**
- Create: `nadeem_mobile/lib/core/api/backend_client.dart`
- Test: `nadeem_mobile/test/core/api/backend_client_test.dart`

The backend client wraps:
- `POST /api/auth/login` → returns JWT
- `POST /api/provisioning-tokens` → returns `ProvisionToken`
- `GET /api/devices/<device_id>` → returns `Device` (for polling bootstrap)

- [ ] **Step 1: Write failing tests**

```dart
// test/core/api/backend_client_test.dart
import 'package:dio/dio.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/models/device.dart';
import 'package:nadeem_mobile/core/models/provision_token.dart';

class MockDio extends Mock implements Dio {}
class MockResponse<T> extends Mock implements Response<T> {}

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

    test('throws on 401', () async {
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
}
```

- [ ] **Step 2: Run tests — expect FAIL**

```bash
flutter test test/core/api/backend_client_test.dart
```

Expected: compilation errors — `BackendClient`, `BackendAuthException` not found.

- [ ] **Step 3: Write `backend_client.dart`**

```dart
// lib/core/api/backend_client.dart
import 'package:dio/dio.dart';
import 'package:nadeem_mobile/core/models/device.dart';
import 'package:nadeem_mobile/core/models/provision_token.dart';

class BackendAuthException implements Exception {
  final String message;
  const BackendAuthException(this.message);

  @override
  String toString() => 'BackendAuthException: $message';
}

class BackendClient {
  final Dio _dio;

  BackendClient(String baseUrl)
      : _dio = Dio(BaseOptions(
          baseUrl: baseUrl,
          connectTimeout: const Duration(seconds: 10),
          receiveTimeout: const Duration(seconds: 10),
        ));

  BackendClient.withDio(this._dio);

  Future<String> login(String email, String password) async {
    try {
      final resp = await _dio.post<Map<String, dynamic>>(
        '/api/auth/login',
        data: {'email': email, 'password': password},
      );
      return resp.data!['token'] as String;
    } on DioException catch (e) {
      if (e.response?.statusCode == 401) {
        throw const BackendAuthException('Invalid credentials');
      }
      rethrow;
    }
  }

  Future<ProvisionToken> mintProvisionToken(String jwt) async {
    final resp = await _dio.post<Map<String, dynamic>>(
      '/api/provisioning-tokens',
      options: Options(headers: {'Authorization': 'Bearer $jwt'}),
    );
    return ProvisionToken.fromJson(resp.data!);
  }

  Future<Device> getDevice(String deviceId, String jwt) async {
    final resp = await _dio.get<Map<String, dynamic>>(
      '/api/devices/$deviceId',
      options: Options(headers: {'Authorization': 'Bearer $jwt'}),
    );
    return Device.fromJson(resp.data!);
  }
}
```

- [ ] **Step 4: Run tests — expect PASS**

```bash
flutter test test/core/api/backend_client_test.dart
```

Expected: 4 tests pass.

- [ ] **Step 5: Commit**

```bash
git add lib/core/api/backend_client.dart test/core/api/backend_client_test.dart
git commit -m "feat(mobile): add BackendClient (login, mintProvisionToken, getDevice)"
```

---

## Task 4: Device Local API Client

**Files:**
- Create: `nadeem_mobile/lib/core/api/device_client.dart`
- Test: `nadeem_mobile/test/core/api/device_client_test.dart`

The device client wraps `http://192.168.4.1:80` — the SoftAP HTTP server. 2-second read timeout on `/scan` (real Wi-Fi scan blocks). On `/provision`, the TCP connection resets after ~500 ms; that's expected — treat `DioExceptionType.connectionError` after a 200 as success.

- [ ] **Step 1: Write failing tests**

```dart
// test/core/api/device_client_test.dart
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

    test('throws DeviceNotFoundError on non-200', () async {
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
    test('returns true on 200 {ok:true}', () async {
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
      // Device sends 200 then reboots, closing TCP — expect success not throw
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

    test('throws on 400 missing ssid', () async {
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
```

- [ ] **Step 2: Run tests — expect FAIL**

```bash
flutter test test/core/api/device_client_test.dart
```

Expected: compilation errors — `DeviceClient`, error types not found.

- [ ] **Step 3: Write `device_client.dart`**

```dart
// lib/core/api/device_client.dart
import 'package:dio/dio.dart';
import 'package:nadeem_mobile/core/models/device_info.dart';
import 'package:nadeem_mobile/core/models/wifi_network.dart';

class DeviceNotReachableError implements Exception {}

class ProvisionValidationError implements Exception {
  final String message;
  const ProvisionValidationError(this.message);
}

class DeviceClient {
  static const _deviceBaseUrl = 'http://192.168.4.1';

  final Dio _dio;

  DeviceClient()
      : _dio = Dio(BaseOptions(
          baseUrl: _deviceBaseUrl,
          connectTimeout: const Duration(seconds: 5),
          receiveTimeout: const Duration(seconds: 4),
        ));

  DeviceClient.withDio(this._dio);

  Future<DeviceInfo> fetchInfo() async {
    try {
      final resp = await _dio.get<Map<String, dynamic>>('/info');
      return DeviceInfo.fromJson(resp.data!);
    } on DioException {
      throw DeviceNotReachableError();
    }
  }

  Future<List<WifiNetwork>> scanNetworks() async {
    final resp = await _dio.get<List<dynamic>>('/scan');
    return (resp.data as List)
        .map((e) => WifiNetwork.fromJson(e as Map<String, dynamic>))
        .toList();
  }

  Future<bool> provision({
    required String ssid,
    required String password,
    required String provisionToken,
  }) async {
    try {
      await _dio.post<Map<String, dynamic>>(
        '/provision',
        data: Uri(queryParameters: {
          'ssid': ssid,
          if (password.isNotEmpty) 'password': password,
          'provision_token': provisionToken,
        }).query,
        options: Options(
          contentType: 'application/x-www-form-urlencoded',
        ),
      );
      return true;
    } on DioException catch (e) {
      // Device reboots ~500 ms after 200; TCP reset is expected.
      if (e.type == DioExceptionType.connectionError &&
          e.response?.statusCode == 200) {
        return true;
      }
      if (e.response?.statusCode == 400) {
        throw ProvisionValidationError(
            e.response?.data?.toString() ?? 'Validation error');
      }
      rethrow;
    }
  }

  Future<void> factoryReset() async {
    try {
      await _dio.post<Map<String, dynamic>>('/reset');
    } on DioException catch (e) {
      // Same reboot pattern as /provision — TCP reset is normal.
      if (e.type == DioExceptionType.connectionError) return;
      rethrow;
    }
  }
}
```

- [ ] **Step 4: Run tests — expect PASS**

```bash
flutter test test/core/api/device_client_test.dart
```

Expected: 5 tests pass.

- [ ] **Step 5: Commit**

```bash
git add lib/core/api/device_client.dart test/core/api/device_client_test.dart
git commit -m "feat(mobile): add DeviceClient for local SoftAP HTTP API"
```

---

## Task 5: Auth Service

**Files:**
- Create: `nadeem_mobile/lib/features/auth/auth_service.dart`
- Test: `nadeem_mobile/test/features/auth/auth_service_test.dart`

Stores JWT in flutter_secure_storage; exposes `login`, `logout`, `getToken`.

- [ ] **Step 1: Write failing tests**

```dart
// test/features/auth/auth_service_test.dart
import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
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
```

- [ ] **Step 2: Run tests — expect FAIL**

```bash
flutter test test/features/auth/auth_service_test.dart
```

Expected: `AuthService` not found.

- [ ] **Step 3: Write `auth_service.dart`**

```dart
// lib/features/auth/auth_service.dart
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';

class AuthService {
  static const jwtKey = 'nadeem_jwt';

  final BackendClient _backend;
  final FlutterSecureStorage _storage;

  AuthService(this._backend, this._storage);

  Future<void> login(String email, String password) async {
    final token = await _backend.login(email, password);
    await _storage.write(key: jwtKey, value: token);
  }

  Future<String?> getToken() => _storage.read(key: jwtKey);

  Future<void> logout() => _storage.delete(key: jwtKey);

  Future<bool> isLoggedIn() async => (await getToken()) != null;
}
```

- [ ] **Step 4: Run tests — expect PASS**

```bash
flutter test test/features/auth/auth_service_test.dart
```

Expected: 4 tests pass.

- [ ] **Step 5: Commit**

```bash
git add lib/features/auth/auth_service.dart test/features/auth/auth_service_test.dart
git commit -m "feat(mobile): add AuthService with JWT secure storage"
```

---

## Task 6: Platform Wi-Fi Join Service

**Files:**
- Create: `nadeem_mobile/lib/features/onboarding/services/wifi_join_service.dart`
- Test: `nadeem_mobile/test/features/onboarding/wifi_join_service_test.dart`

Thin wrapper around `wifi_iot` that abstracts the platform difference. On iOS, `wifi_iot` uses `NEHotspotConfiguration`. On Android, it uses `WifiNetworkSpecifier`. The caller passes the SSID to join (e.g. `Nadeem-A3F1`).

- [ ] **Step 1: Write failing tests**

```dart
// test/features/onboarding/wifi_join_service_test.dart
import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:wifi_iot/wifi_iot.dart';
import 'package:nadeem_mobile/features/onboarding/services/wifi_join_service.dart';

class MockWifiForIoT extends Mock implements WiFiForIoTPlugin {}

// wifi_iot uses a global singleton; this test validates the join/disconnect logic
// by confirming the service calls the expected wifi_iot methods.
void main() {
  late MockWifiForIoT mockWifi;
  late WifiJoinService service;

  setUp(() {
    mockWifi = MockWifiForIoT();
    service = WifiJoinService(plugin: mockWifi);
  });

  test('joinSoftAP calls connectToNetwork with correct SSID', () async {
    when(() => mockWifi.connectToNetwork('Nadeem-A3F1', security: NetworkSecurity.NONE))
        .thenAnswer((_) async => true);

    final result = await service.joinSoftAP('Nadeem-A3F1');
    expect(result, isTrue);
  });

  test('joinSoftAP returns false on failure', () async {
    when(() => mockWifi.connectToNetwork('Nadeem-A3F1', security: NetworkSecurity.NONE))
        .thenAnswer((_) async => false);

    final result = await service.joinSoftAP('Nadeem-A3F1');
    expect(result, isFalse);
  });

  test('disconnect calls disconnect on plugin', () async {
    when(() => mockWifi.disconnect()).thenAnswer((_) async => true);
    await service.disconnect();
    verify(() => mockWifi.disconnect()).called(1);
  });
}
```

- [ ] **Step 2: Run tests — expect FAIL**

```bash
flutter test test/features/onboarding/wifi_join_service_test.dart
```

Expected: `WifiJoinService` not found.

- [ ] **Step 3: Write `wifi_join_service.dart`**

```dart
// lib/features/onboarding/services/wifi_join_service.dart
import 'package:wifi_iot/wifi_iot.dart';

class WifiJoinService {
  final WiFiForIoTPlugin _plugin;

  WifiJoinService({WiFiForIoTPlugin? plugin})
      : _plugin = plugin ?? WiFiForIoTPlugin();

  Future<bool> joinSoftAP(String ssid) async {
    return _plugin.connectToNetwork(ssid, security: NetworkSecurity.NONE);
  }

  Future<void> disconnect() async {
    await _plugin.disconnect();
  }
}
```

- [ ] **Step 4: Run tests — expect PASS**

```bash
flutter test test/features/onboarding/wifi_join_service_test.dart
```

Expected: 3 tests pass.

- [ ] **Step 5: Configure Android permissions**

In `android/app/src/main/AndroidManifest.xml`, inside `<manifest>` before `<application>`:

```xml
<uses-permission android:name="android.permission.ACCESS_WIFI_STATE"/>
<uses-permission android:name="android.permission.CHANGE_WIFI_STATE"/>
<uses-permission android:name="android.permission.ACCESS_FINE_LOCATION"/>
<uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION"/>
<uses-permission android:name="android.permission.CHANGE_NETWORK_STATE"/>
<uses-permission android:name="android.permission.INTERNET"/>
```

- [ ] **Step 6: Configure iOS permissions**

In `ios/Runner/Info.plist`, add inside the root `<dict>`:

```xml
<key>NSLocationWhenInUseUsageDescription</key>
<string>Needed to detect the Nadeem device Wi-Fi network</string>
<key>NSLocalNetworkUsageDescription</key>
<string>Needed to communicate with the Nadeem device on your local network</string>
```

In `ios/Runner/Runner.entitlements` (create if absent):

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.developer.networking.wifi-info</key>
    <true/>
</dict>
</plist>
```

- [ ] **Step 7: Commit**

```bash
git add lib/features/onboarding/services/wifi_join_service.dart \
        test/features/onboarding/wifi_join_service_test.dart \
        android/app/src/main/AndroidManifest.xml \
        ios/Runner/Info.plist ios/Runner/Runner.entitlements
git commit -m "feat(mobile): add WifiJoinService with platform permissions"
```

---

## Task 7: Device Discovery Service

**Files:**
- Create: `nadeem_mobile/lib/features/onboarding/services/device_discovery_service.dart`
- Test: `nadeem_mobile/test/features/onboarding/device_discovery_service_test.dart`

Repeatedly probes `GET /info` until it gets a valid `DeviceInfo` with `"fw"` present, or times out. Used to confirm the app is connected to the SoftAP.

- [ ] **Step 1: Write failing tests**

```dart
// test/features/onboarding/device_discovery_service_test.dart
import 'dart:async';
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
```

- [ ] **Step 2: Run tests — expect FAIL**

```bash
flutter test test/features/onboarding/device_discovery_service_test.dart
```

Expected: `DeviceDiscoveryService`, `DeviceDiscoveryTimeoutError` not found.

- [ ] **Step 3: Write `device_discovery_service.dart`**

```dart
// lib/features/onboarding/services/device_discovery_service.dart
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
```

- [ ] **Step 4: Run tests — expect PASS**

```bash
flutter test test/features/onboarding/device_discovery_service_test.dart
```

Expected: 3 tests pass.

- [ ] **Step 5: Commit**

```bash
git add lib/features/onboarding/services/device_discovery_service.dart \
        test/features/onboarding/device_discovery_service_test.dart
git commit -m "feat(mobile): add DeviceDiscoveryService with retry/timeout"
```

---

## Task 8: App Entry, Router, and Providers

**Files:**
- Create: `nadeem_mobile/lib/main.dart`
- Create: `nadeem_mobile/lib/app.dart`
- Create: `nadeem_mobile/lib/core/providers.dart`

Set up the `ProviderScope`, `GoRouter`, and top-level Riverpod providers for `BackendClient`, `DeviceClient`, `AuthService`, `WifiJoinService`, `DeviceDiscoveryService`.

- [ ] **Step 1: Write `lib/core/providers.dart`**

```dart
// lib/core/providers.dart
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/api/device_client.dart';
import 'package:nadeem_mobile/features/auth/auth_service.dart';
import 'package:nadeem_mobile/features/onboarding/services/device_discovery_service.dart';
import 'package:nadeem_mobile/features/onboarding/services/wifi_join_service.dart';

// Replace with your real backend URL before shipping.
const _kBackendUrl = 'https://api.nadeem.example';

final backendClientProvider = Provider<BackendClient>(
  (_) => BackendClient(_kBackendUrl),
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
```

- [ ] **Step 2: Write `lib/app.dart`**

```dart
// lib/app.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/features/auth/login_screen.dart';
import 'package:nadeem_mobile/features/dashboard/dashboard_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/onboarding_start_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/wifi_join_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/network_select_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/provision_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/success_screen.dart';

final _router = GoRouter(
  initialLocation: '/login',
  routes: [
    GoRoute(path: '/login', builder: (_, __) => const LoginScreen()),
    GoRoute(path: '/dashboard', builder: (_, __) => const DashboardScreen()),
    GoRoute(
      path: '/onboarding',
      builder: (_, __) => const OnboardingStartScreen(),
    ),
    GoRoute(
      path: '/onboarding/wifi-join',
      builder: (_, state) {
        final provisionToken = state.extra as String;
        return WifiJoinScreen(provisionToken: provisionToken);
      },
    ),
    GoRoute(
      path: '/onboarding/network-select',
      builder: (_, state) {
        final extra = state.extra as Map<String, String>;
        return NetworkSelectScreen(
          provisionToken: extra['provisionToken']!,
          deviceId: extra['deviceId']!,
        );
      },
    ),
    GoRoute(
      path: '/onboarding/provision',
      builder: (_, state) {
        final extra = state.extra as Map<String, String>;
        return ProvisionScreen(
          ssid: extra['ssid']!,
          password: extra['password']!,
          provisionToken: extra['provisionToken']!,
          deviceId: extra['deviceId']!,
        );
      },
    ),
    GoRoute(
      path: '/onboarding/success',
      builder: (_, state) {
        final deviceId = state.extra as String;
        return SuccessScreen(deviceId: deviceId);
      },
    ),
  ],
);

class NadeemApp extends ConsumerWidget {
  const NadeemApp({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    return MaterialApp.router(
      title: 'Nadeem',
      routerConfig: _router,
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: const Color(0xFF4A90E2)),
        useMaterial3: true,
      ),
    );
  }
}
```

- [ ] **Step 3: Write `lib/main.dart`**

```dart
// lib/main.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:nadeem_mobile/app.dart';

void main() {
  runApp(const ProviderScope(child: NadeemApp()));
}
```

- [ ] **Step 4: Verify compilation**

```bash
flutter build apk --debug 2>&1 | head -30
```

Expected: compiles without errors (screens can be stub widgets for now).

- [ ] **Step 5: Commit**

```bash
git add lib/main.dart lib/app.dart lib/core/providers.dart
git commit -m "feat(mobile): wire app entry, GoRouter, and top-level providers"
```

---

## Task 9: Login Screen

**Files:**
- Create: `nadeem_mobile/lib/features/auth/login_screen.dart`

- [ ] **Step 1: Write `login_screen.dart`**

```dart
// lib/features/auth/login_screen.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/providers.dart';

class LoginScreen extends ConsumerStatefulWidget {
  const LoginScreen({super.key});

  @override
  ConsumerState<LoginScreen> createState() => _LoginScreenState();
}

class _LoginScreenState extends ConsumerState<LoginScreen> {
  final _formKey = GlobalKey<FormState>();
  final _emailCtrl = TextEditingController();
  final _passCtrl = TextEditingController();
  bool _loading = false;
  String? _error;

  @override
  void dispose() {
    _emailCtrl.dispose();
    _passCtrl.dispose();
    super.dispose();
  }

  Future<void> _submit() async {
    if (!_formKey.currentState!.validate()) return;
    setState(() { _loading = true; _error = null; });
    try {
      await ref.read(authServiceProvider).login(_emailCtrl.text.trim(), _passCtrl.text);
      if (mounted) context.go('/dashboard');
    } on BackendAuthException {
      setState(() => _error = 'البريد الإلكتروني أو كلمة المرور غير صحيحة');
    } catch (_) {
      setState(() => _error = 'تعذّر الاتصال. حاول مجدداً');
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Form(
            key: _formKey,
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Text('نديم', style: Theme.of(context).textTheme.displayMedium, textAlign: TextAlign.center),
                const SizedBox(height: 32),
                TextFormField(
                  controller: _emailCtrl,
                  decoration: const InputDecoration(labelText: 'البريد الإلكتروني'),
                  keyboardType: TextInputType.emailAddress,
                  textDirection: TextDirection.ltr,
                  validator: (v) => (v == null || !v.contains('@')) ? 'بريد إلكتروني غير صالح' : null,
                ),
                const SizedBox(height: 16),
                TextFormField(
                  controller: _passCtrl,
                  decoration: const InputDecoration(labelText: 'كلمة المرور'),
                  obscureText: true,
                  textDirection: TextDirection.ltr,
                  validator: (v) => (v == null || v.isEmpty) ? 'أدخل كلمة المرور' : null,
                ),
                if (_error != null) ...[
                  const SizedBox(height: 12),
                  Text(_error!, style: TextStyle(color: Theme.of(context).colorScheme.error)),
                ],
                const SizedBox(height: 24),
                ElevatedButton(
                  onPressed: _loading ? null : _submit,
                  child: _loading ? const CircularProgressIndicator() : const Text('تسجيل الدخول'),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
```

- [ ] **Step 2: Commit**

```bash
git add lib/features/auth/login_screen.dart
git commit -m "feat(mobile): add LoginScreen with Arabic UI"
```

---

## Task 10: Dashboard Screen

**Files:**
- Create: `nadeem_mobile/lib/features/dashboard/dashboard_screen.dart`

Shows a device list (via `GET /api/devices/`) and an "Add Device" button that launches the onboarding flow. After login this is the home screen.

- [ ] **Step 1: Write `dashboard_screen.dart`**

```dart
// lib/features/dashboard/dashboard_screen.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/models/device.dart';
import 'package:nadeem_mobile/core/providers.dart';

final _devicesProvider = FutureProvider.autoDispose<List<Device>>((ref) async {
  final auth = ref.read(authServiceProvider);
  final token = await auth.getToken();
  if (token == null) return [];
  return ref.read(backendClientProvider).listDevices(token);
});

class DashboardScreen extends ConsumerWidget {
  const DashboardScreen({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final devicesAsync = ref.watch(_devicesProvider);

    return Scaffold(
      appBar: AppBar(title: const Text('أجهزة نديم')),
      body: devicesAsync.when(
        loading: () => const Center(child: CircularProgressIndicator()),
        error: (e, _) => Center(child: Text('خطأ في تحميل الأجهزة: $e')),
        data: (devices) => devices.isEmpty
            ? const Center(child: Text('لا توجد أجهزة مرتبطة بعد'))
            : ListView.builder(
                itemCount: devices.length,
                itemBuilder: (_, i) => ListTile(
                  title: Text(devices[i].name ?? devices[i].deviceId),
                  subtitle: Text(devices[i].onlineStatus ? 'متصل' : 'غير متصل'),
                  leading: Icon(
                    Icons.speaker,
                    color: devices[i].onlineStatus ? Colors.green : Colors.grey,
                  ),
                ),
              ),
      ),
      floatingActionButton: FloatingActionButton.extended(
        onPressed: () => context.push('/onboarding'),
        icon: const Icon(Icons.add),
        label: const Text('إضافة جهاز'),
      ),
    );
  }
}
```

- [ ] **Step 2: Add `listDevices` to `BackendClient`**

In `lib/core/api/backend_client.dart`, add:

```dart
Future<List<Device>> listDevices(String jwt) async {
  final resp = await _dio.get<List<dynamic>>(
    '/api/devices/',
    options: Options(headers: {'Authorization': 'Bearer $jwt'}),
  );
  return (resp.data as List)
      .map((e) => Device.fromJson(e as Map<String, dynamic>))
      .toList();
}
```

- [ ] **Step 3: Commit**

```bash
git add lib/features/dashboard/dashboard_screen.dart lib/core/api/backend_client.dart
git commit -m "feat(mobile): add DashboardScreen and BackendClient.listDevices"
```

---

## Task 11: Onboarding Start Screen

**Files:**
- Create: `nadeem_mobile/lib/features/onboarding/screens/onboarding_start_screen.dart`

Instructs the user to power on the Nadeem. On "Continue", mints a `provision_token` from the backend and navigates to the Wi-Fi join screen.

- [ ] **Step 1: Write `onboarding_start_screen.dart`**

```dart
// lib/features/onboarding/screens/onboarding_start_screen.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';

class OnboardingStartScreen extends ConsumerStatefulWidget {
  const OnboardingStartScreen({super.key});

  @override
  ConsumerState<OnboardingStartScreen> createState() => _State();
}

class _State extends ConsumerState<OnboardingStartScreen> {
  bool _loading = false;
  String? _error;

  Future<void> _continue() async {
    setState(() { _loading = true; _error = null; });
    try {
      final token = await ref.read(authServiceProvider).getToken();
      if (token == null) { context.go('/login'); return; }
      final pt = await ref.read(backendClientProvider).mintProvisionToken(token);
      if (mounted) context.push('/onboarding/wifi-join', extra: pt.token);
    } catch (e) {
      setState(() => _error = 'تعذّر الحصول على رمز التوصيل: $e');
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('إضافة جهاز نديم')),
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Icon(Icons.speaker, size: 80, color: Colors.blue),
            const SizedBox(height: 24),
            const Text(
              'تأكد من تشغيل جهاز نديم الجديد.\nيجب أن تومض المصابيح باللون الأزرق.',
              textAlign: TextAlign.center,
              style: TextStyle(fontSize: 18),
            ),
            if (_error != null) ...[
              const SizedBox(height: 16),
              Text(_error!, style: const TextStyle(color: Colors.red)),
            ],
            const SizedBox(height: 32),
            ElevatedButton(
              onPressed: _loading ? null : _continue,
              child: _loading ? const CircularProgressIndicator() : const Text('متابعة'),
            ),
          ],
        ),
      ),
    );
  }
}
```

- [ ] **Step 2: Commit**

```bash
git add lib/features/onboarding/screens/onboarding_start_screen.dart
git commit -m "feat(mobile): add OnboardingStartScreen (mint provision token)"
```

---

## Task 12: Wi-Fi Join Screen

**Files:**
- Create: `nadeem_mobile/lib/features/onboarding/screens/wifi_join_screen.dart`

Tells the user to connect to `Nadeem-XXXX`. On "اتصل الآن", calls `WifiJoinService.joinSoftAP` (providing a known `Nadeem-` prefix) then probes `/info` to confirm. Passes `deviceId` downstream.

- [ ] **Step 1: Write `wifi_join_screen.dart`**

```dart
// lib/features/onboarding/screens/wifi_join_screen.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/features/onboarding/services/device_discovery_service.dart';

class WifiJoinScreen extends ConsumerStatefulWidget {
  final String provisionToken;
  const WifiJoinScreen({super.key, required this.provisionToken});

  @override
  ConsumerState<WifiJoinScreen> createState() => _State();
}

class _State extends ConsumerState<WifiJoinScreen> {
  bool _loading = false;
  String? _error;

  Future<void> _connectAndProbe() async {
    setState(() { _loading = true; _error = null; });
    try {
      final wifiService = ref.read(wifiJoinServiceProvider);
      final discoveryService = ref.read(deviceDiscoveryServiceProvider);

      // On Android the join is programmatic; on iOS the user joins manually
      // via Settings — joinSoftAP still triggers the system prompt on iOS.
      final joined = await wifiService.joinSoftAP('Nadeem-');
      if (!joined && mounted) {
        setState(() => _error = 'تعذّر الاتصال بالجهاز. يرجى الانضمام يدوياً من إعدادات Wi-Fi.');
        setState(() => _loading = false);
        return;
      }

      final info = await discoveryService.discover();
      if (mounted) {
        context.push('/onboarding/network-select', extra: {
          'provisionToken': widget.provisionToken,
          'deviceId': info.deviceId,
        });
      }
    } on DeviceDiscoveryTimeoutError {
      setState(() => _error = 'لم يتم العثور على جهاز نديم. تأكد من الاتصال بشبكة Nadeem-XXXX.');
    } catch (e) {
      setState(() => _error = 'خطأ: $e');
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('الاتصال بالجهاز')),
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Icon(Icons.wifi, size: 80, color: Colors.blue),
            const SizedBox(height: 24),
            const Text(
              'سيتم الاتصال بشبكة الجهاز (Nadeem-XXXX) تلقائياً.\nقد يُطلب منك السماح بذلك.',
              textAlign: TextAlign.center,
              style: TextStyle(fontSize: 16),
            ),
            if (_error != null) ...[
              const SizedBox(height: 16),
              Text(_error!, style: const TextStyle(color: Colors.red), textAlign: TextAlign.center),
            ],
            const SizedBox(height: 32),
            ElevatedButton(
              onPressed: _loading ? null : _connectAndProbe,
              child: _loading
                  ? const Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        CircularProgressIndicator(),
                        SizedBox(height: 8),
                        Text('جاري البحث عن الجهاز...'),
                      ],
                    )
                  : const Text('اتصل الآن'),
            ),
          ],
        ),
      ),
    );
  }
}
```

- [ ] **Step 2: Commit**

```bash
git add lib/features/onboarding/screens/wifi_join_screen.dart
git commit -m "feat(mobile): add WifiJoinScreen (join SoftAP + probe /info)"
```

---

## Task 13: Network Select Screen

**Files:**
- Create: `nadeem_mobile/lib/features/onboarding/screens/network_select_screen.dart`

Calls `GET /scan`, shows a list of nearby APs. User taps one; if not open, shows a password field. Navigates to `ProvisionScreen` with all required params.

- [ ] **Step 1: Write `network_select_screen.dart`**

```dart
// lib/features/onboarding/screens/network_select_screen.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/models/wifi_network.dart';
import 'package:nadeem_mobile/core/providers.dart';

final _scanProvider =
    FutureProvider.autoDispose<List<WifiNetwork>>((ref) async {
  return ref.read(deviceClientProvider).scanNetworks();
});

class NetworkSelectScreen extends ConsumerStatefulWidget {
  final String provisionToken;
  final String deviceId;

  const NetworkSelectScreen({
    super.key,
    required this.provisionToken,
    required this.deviceId,
  });

  @override
  ConsumerState<NetworkSelectScreen> createState() => _State();
}

class _State extends ConsumerState<NetworkSelectScreen> {
  WifiNetwork? _selected;
  final _passCtrl = TextEditingController();

  @override
  void dispose() {
    _passCtrl.dispose();
    super.dispose();
  }

  void _selectNetwork(WifiNetwork net) {
    setState(() {
      _selected = net;
      _passCtrl.clear();
    });
  }

  void _proceed() {
    if (_selected == null) return;
    if (!_selected!.isOpen && _passCtrl.text.isEmpty) return;

    context.push('/onboarding/provision', extra: {
      'ssid': _selected!.ssid,
      'password': _passCtrl.text,
      'provisionToken': widget.provisionToken,
      'deviceId': widget.deviceId,
    });
  }

  @override
  Widget build(BuildContext context) {
    final scanAsync = ref.watch(_scanProvider);

    return Scaffold(
      appBar: AppBar(title: const Text('اختر شبكة Wi-Fi')),
      body: scanAsync.when(
        loading: () => const Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              CircularProgressIndicator(),
              SizedBox(height: 16),
              Text('جاري البحث عن الشبكات...'),
            ],
          ),
        ),
        error: (e, _) => Center(child: Text('خطأ: $e')),
        data: (networks) => Column(
          children: [
            Expanded(
              child: ListView.builder(
                itemCount: networks.length,
                itemBuilder: (_, i) {
                  final net = networks[i];
                  final isSelected = _selected?.ssid == net.ssid;
                  return Column(
                    children: [
                      ListTile(
                        title: Text(net.ssid),
                        subtitle: Text('${net.authLabel} · ${net.rssi} dBm'),
                        leading: Icon(
                          Icons.wifi,
                          color: isSelected ? Theme.of(context).colorScheme.primary : null,
                        ),
                        selected: isSelected,
                        onTap: () => _selectNetwork(net),
                      ),
                      if (isSelected && !net.isOpen)
                        Padding(
                          padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 4),
                          child: TextFormField(
                            controller: _passCtrl,
                            decoration: const InputDecoration(labelText: 'كلمة المرور'),
                            obscureText: true,
                            textDirection: TextDirection.ltr,
                          ),
                        ),
                    ],
                  );
                },
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(16),
              child: ElevatedButton(
                onPressed: _selected != null ? _proceed : null,
                child: const Text('متابعة'),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
```

- [ ] **Step 2: Commit**

```bash
git add lib/features/onboarding/screens/network_select_screen.dart
git commit -m "feat(mobile): add NetworkSelectScreen (GET /scan results)"
```

---

## Task 14: Provision Screen + Backend Polling

**Files:**
- Create: `nadeem_mobile/lib/features/onboarding/screens/provision_screen.dart`
- Test: `nadeem_mobile/test/features/onboarding/provision_screen_test.dart`

Calls `POST /provision` on the device, then polls `GET /api/devices/<device_id>` every 3 seconds (max 60 s) until `bootstrapped=true`. Shows progress. Navigates to `SuccessScreen` or shows a timeout error.

- [ ] **Step 1: Write failing test for the provision+poll logic**

```dart
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

    expect(
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
```

- [ ] **Step 2: Run tests — expect FAIL**

```bash
flutter test test/features/onboarding/provision_screen_test.dart
```

Expected: `ProvisionService`, `ProvisionTimeoutError` not found.

- [ ] **Step 3: Write `provision_service.dart`**

```dart
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
      await Future.delayed(_pollInterval);
      try {
        final device = await _backendClient.getDevice(deviceId, jwt);
        if (device.bootstrapped) return device;
      } catch (_) {
        // backend may be momentarily unreachable — keep polling
      }
    }

    throw ProvisionTimeoutError();
  }
}
```

- [ ] **Step 4: Register provider in `providers.dart`**

In `lib/core/providers.dart`, add:

```dart
import 'package:nadeem_mobile/features/onboarding/services/provision_service.dart';

final provisionServiceProvider = Provider<ProvisionService>((ref) {
  return ProvisionService(
    ref.read(deviceClientProvider),
    ref.read(backendClientProvider),
  );
});
```

- [ ] **Step 5: Run tests — expect PASS**

```bash
flutter test test/features/onboarding/provision_screen_test.dart
```

Expected: 2 tests pass.

- [ ] **Step 6: Write `provision_screen.dart`**

```dart
// lib/features/onboarding/screens/provision_screen.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/features/onboarding/services/provision_service.dart';

class ProvisionScreen extends ConsumerStatefulWidget {
  final String ssid;
  final String password;
  final String provisionToken;
  final String deviceId;

  const ProvisionScreen({
    super.key,
    required this.ssid,
    required this.password,
    required this.provisionToken,
    required this.deviceId,
  });

  @override
  ConsumerState<ProvisionScreen> createState() => _State();
}

class _State extends ConsumerState<ProvisionScreen> {
  String? _error;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) => _run());
  }

  Future<void> _run() async {
    try {
      final jwt = await ref.read(authServiceProvider).getToken();
      if (jwt == null) { context.go('/login'); return; }
      await ref.read(provisionServiceProvider).provisionAndWait(
        ssid: widget.ssid,
        password: widget.password,
        provisionToken: widget.provisionToken,
        deviceId: widget.deviceId,
        jwt: jwt,
      );
      if (mounted) context.go('/onboarding/success', extra: widget.deviceId);
    } on ProvisionTimeoutError {
      setState(() => _error = 'لم يتصل الجهاز بالشبكة خلال 60 ثانية.\nتحقق من كلمة مرور Wi-Fi ثم أعد المحاولة.');
    } catch (e) {
      setState(() => _error = 'خطأ: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('جاري إعداد الجهاز')),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: _error != null
              ? Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    const Icon(Icons.error_outline, size: 64, color: Colors.red),
                    const SizedBox(height: 16),
                    Text(_error!, textAlign: TextAlign.center, style: const TextStyle(fontSize: 16)),
                    const SizedBox(height: 24),
                    ElevatedButton(
                      onPressed: () => context.go('/dashboard'),
                      child: const Text('العودة للرئيسية'),
                    ),
                  ],
                )
              : Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    const CircularProgressIndicator(),
                    const SizedBox(height: 24),
                    Text('إرسال الإعدادات إلى: ${widget.ssid}'),
                    const SizedBox(height: 8),
                    const Text('في انتظار اتصال الجهاز بالخادم...', style: TextStyle(color: Colors.grey)),
                  ],
                ),
        ),
      ),
    );
  }
}
```

- [ ] **Step 7: Commit**

```bash
git add lib/features/onboarding/services/provision_service.dart \
        lib/features/onboarding/screens/provision_screen.dart \
        lib/core/providers.dart \
        test/features/onboarding/provision_screen_test.dart
git commit -m "feat(mobile): add ProvisionService and ProvisionScreen (provision + poll)"
```

---

## Task 15: Success Screen

**Files:**
- Create: `nadeem_mobile/lib/features/onboarding/screens/success_screen.dart`

- [ ] **Step 1: Write `success_screen.dart`**

```dart
// lib/features/onboarding/screens/success_screen.dart
import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';

class SuccessScreen extends StatelessWidget {
  final String deviceId;
  const SuccessScreen({super.key, required this.deviceId});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Center(
          child: Padding(
            padding: const EdgeInsets.all(24),
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                const Icon(Icons.check_circle_outline, size: 100, color: Colors.green),
                const SizedBox(height: 24),
                const Text(
                  'نديم متصل الآن!',
                  style: TextStyle(fontSize: 28, fontWeight: FontWeight.bold),
                ),
                const SizedBox(height: 12),
                Text('معرّف الجهاز: $deviceId', style: const TextStyle(color: Colors.grey)),
                const SizedBox(height: 40),
                ElevatedButton(
                  onPressed: () => context.go('/dashboard'),
                  child: const Text('الذهاب للوحة التحكم'),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
```

- [ ] **Step 2: Commit**

```bash
git add lib/features/onboarding/screens/success_screen.dart
git commit -m "feat(mobile): add SuccessScreen"
```

---

## Task 16: Full Test Run and Checklist

- [ ] **Step 1: Run all tests**

```bash
cd nadeem_mobile && flutter test
```

Expected: all tests pass with 0 failures.

- [ ] **Step 2: Run static analysis**

```bash
flutter analyze
```

Expected: no errors, 0 issues.

- [ ] **Step 3: Build debug APK to verify compilation**

```bash
flutter build apk --debug
```

Expected: `Build at build/app/outputs/flutter-apk/app-debug.apk` line in output.

- [ ] **Step 4: Manual onboarding checklist (device required)**

Run the app on a physical device. Walk through the happy path from the spec:

- [ ] `GET /info` returns 200 and parses when joined to `Nadeem-XXXX`
- [ ] `GET /scan` shows the home network with sane RSSI / auth
- [ ] `POST /provision` with valid SSID/password succeeds; AP disappears within 5 s
- [ ] Backend reports device "online" + "bootstrapped" within 60 s
- [ ] `POST /reset` brings the device back to onboarding mode
- [ ] SSID with `&`, `=`, spaces, and non-ASCII chars provisions successfully
- [ ] App tolerates dropped TCP after `/provision` (no crash)
- [ ] Repeated `/scan` within 2 s does not lock up the device

- [ ] **Step 5: Final commit**

```bash
git add -A
git commit -m "chore(mobile): verify all tests pass and app compiles"
```

---

## Spec Coverage Self-Review

| Requirement (from mobile.md / wiki) | Covered in task |
|--------------------------------------|-----------------|
| `POST /api/provisioning-tokens` | Task 3, 11 |
| `GET /info` probe | Task 4, 7 |
| `GET /scan` | Task 4, 13 |
| `POST /provision` form-encoded | Task 4, 14 |
| TCP reset after provision = success | Task 4 (test) |
| Poll backend until bootstrapped | Task 14 |
| `POST /reset` | Task 4 |
| iOS `NEHotspotConfiguration` | Task 6 (wifi_iot) |
| Android `WifiNetworkSpecifier` | Task 6 (wifi_iot) |
| Arabic UI, RTL | Tasks 9–15 |
| SSID special chars URL-encoded | Task 4 (`Uri(queryParameters:...)`) |
| 400 "missing ssid" error shown | Task 4 (test) |
| 60 s device-never-appeared error | Task 14 |
| Wrong password → re-provision | Task 14 (timeout → error screen → dashboard) |
| Dashboard device list | Task 10 |
