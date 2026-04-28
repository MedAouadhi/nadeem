import 'package:flutter/material.dart';

class SuccessScreen extends StatelessWidget {
  final String deviceId;
  const SuccessScreen({super.key, required this.deviceId});
  @override
  Widget build(BuildContext context) => const Scaffold(body: Center(child: Text('Success')));
}
