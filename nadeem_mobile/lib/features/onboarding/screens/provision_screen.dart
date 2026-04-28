// lib/features/onboarding/screens/provision_screen.dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/features/onboarding/services/provision_service.dart';

class ProvisionScreen extends ConsumerStatefulWidget {
  final String ssid;
  final String password;
  final String provisionToken;
  final String deviceId;

  const ProvisionScreen({
    super.key,
    required this.ssid,
    required this.password,
    required this.provisionToken,
    required this.deviceId,
  });

  @override
  ConsumerState<ProvisionScreen> createState() => _State();
}

class _State extends ConsumerState<ProvisionScreen> {
  String? _error;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) => _run());
  }

  Future<void> _run() async {
    try {
      final jwt = await ref.read(authServiceProvider).getToken();
      if (jwt == null) { context.go('/login'); return; }
      await ref.read(provisionServiceProvider).provisionAndWait(
        ssid: widget.ssid,
        password: widget.password,
        provisionToken: widget.provisionToken,
        deviceId: widget.deviceId,
        jwt: jwt,
      );
      if (mounted) context.go('/onboarding/success', extra: widget.deviceId);
    } on ProvisionTimeoutError {
      setState(() => _error = 'لم يتصل الجهاز بالشبكة خلال 60 ثانية.\nتحقق من كلمة مرور Wi-Fi ثم أعد المحاولة.');
    } catch (e) {
      setState(() => _error = 'خطأ: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('جاري إعداد الجهاز')),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: _error != null
              ? Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    const Icon(Icons.error_outline, size: 64, color: Colors.red),
                    const SizedBox(height: 16),
                    Text(_error!, textAlign: TextAlign.center, style: const TextStyle(fontSize: 16)),
                    const SizedBox(height: 24),
                    ElevatedButton(
                      onPressed: () => context.go('/dashboard'),
                      child: const Text('العودة للرئيسية'),
                    ),
                  ],
                )
              : Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    const CircularProgressIndicator(),
                    const SizedBox(height: 24),
                    Text('إرسال الإعدادات إلى: ${widget.ssid}'),
                    const SizedBox(height: 8),
                    const Text('في انتظار اتصال الجهاز بالخادم...', style: TextStyle(color: Colors.grey)),
                  ],
                ),
        ),
      ),
    );
  }
}
