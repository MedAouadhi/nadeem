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
      // Device reboots ~500ms after 200; TCP reset is expected after success.
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
      if (e.type == DioExceptionType.connectionError) return;
      rethrow;
    }
  }
}
