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
