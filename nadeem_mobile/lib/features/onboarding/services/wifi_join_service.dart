import 'package:wifi_iot/wifi_iot.dart';

/// Thin wrapper around [WiFiForIoTPlugin] that makes the Wi-Fi API injectable
/// and mockable in tests.
abstract class WifiPlugin {
  Future<bool> connect(String ssid, {NetworkSecurity security});
  Future<bool> disconnect();
}

/// Default implementation that delegates to the static [WiFiForIoTPlugin] API.
class DefaultWifiPlugin implements WifiPlugin {
  const DefaultWifiPlugin();

  @override
  Future<bool> connect(
    String ssid, {
    NetworkSecurity security = NetworkSecurity.NONE,
  }) =>
      WiFiForIoTPlugin.connect(ssid, security: security);

  @override
  Future<bool> disconnect() => WiFiForIoTPlugin.disconnect();
}

class WifiJoinService {
  final WifiPlugin _plugin;

  WifiJoinService({WifiPlugin? plugin})
      : _plugin = plugin ?? const DefaultWifiPlugin();

  Future<bool> joinSoftAP(String ssid) async {
    return _plugin.connect(ssid, security: NetworkSecurity.NONE);
  }

  Future<void> disconnect() async {
    await _plugin.disconnect();
  }
}
