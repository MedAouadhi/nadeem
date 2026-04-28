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
