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
      return resp.data!['access'] as String;
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

  Future<List<Device>> listDevices(String jwt) async {
    final resp = await _dio.get<List<dynamic>>(
      '/api/devices',
      options: Options(headers: {'Authorization': 'Bearer $jwt'}),
    );
    return (resp.data as List)
        .map((e) => Device.fromJson(e as Map<String, dynamic>))
        .toList();
  }
}
