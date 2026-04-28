import 'package:flutter/material.dart';

class WifiJoinScreen extends StatelessWidget {
  final String provisionToken;
  const WifiJoinScreen({super.key, required this.provisionToken});
  @override
  Widget build(BuildContext context) => const Scaffold(body: Center(child: Text('WiFi Join')));
}
