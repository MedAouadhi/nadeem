import 'package:flutter/material.dart';

class NetworkSelectScreen extends StatelessWidget {
  final String provisionToken;
  final String deviceId;
  const NetworkSelectScreen({super.key, required this.provisionToken, required this.deviceId});
  @override
  Widget build(BuildContext context) => const Scaffold(body: Center(child: Text('Network Select')));
}
