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
