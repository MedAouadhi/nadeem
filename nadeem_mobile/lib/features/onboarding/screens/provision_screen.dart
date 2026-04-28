import 'package:flutter/material.dart';

class ProvisionScreen extends StatelessWidget {
  final String ssid;
  final String password;
  final String provisionToken;
  final String deviceId;
  const ProvisionScreen({super.key, required this.ssid, required this.password, required this.provisionToken, required this.deviceId});
  @override
  Widget build(BuildContext context) => const Scaffold(body: Center(child: Text('Provisioning')));
}
