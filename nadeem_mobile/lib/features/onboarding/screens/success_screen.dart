import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';

class SuccessScreen extends StatelessWidget {
  final String deviceId;
  const SuccessScreen({super.key, required this.deviceId});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Center(
          child: Padding(
            padding: const EdgeInsets.all(24),
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                const Icon(Icons.check_circle_outline, size: 100, color: Colors.green),
                const SizedBox(height: 24),
                const Text(
                  'نديم متصل الآن!',
                  style: TextStyle(fontSize: 28, fontWeight: FontWeight.bold),
                ),
                const SizedBox(height: 12),
                Text('معرّف الجهاز: $deviceId', style: const TextStyle(color: Colors.grey)),
                const SizedBox(height: 40),
                ElevatedButton(
                  onPressed: () => context.go('/dashboard'),
                  child: const Text('الذهاب للوحة التحكم'),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
