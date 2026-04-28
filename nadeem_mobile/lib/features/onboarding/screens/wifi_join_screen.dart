import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/features/onboarding/services/device_discovery_service.dart';

class WifiJoinScreen extends ConsumerStatefulWidget {
  final String provisionToken;
  const WifiJoinScreen({super.key, required this.provisionToken});

  @override
  ConsumerState<WifiJoinScreen> createState() => _State();
}

class _State extends ConsumerState<WifiJoinScreen> {
  bool _loading = false;
  String? _error;

  Future<void> _connectAndProbe() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    try {
      final wifiService = ref.read(wifiJoinServiceProvider);
      final discoveryService = ref.read(deviceDiscoveryServiceProvider);

      final joined = await wifiService.joinSoftAP('Nadeem-');
      if (!joined && mounted) {
        setState(() => _error =
            'تعذّر الاتصال بالجهاز. يرجى الانضمام يدوياً من إعدادات Wi-Fi.');
        setState(() => _loading = false);
        return;
      }

      final info = await discoveryService.discover();
      if (mounted) {
        context.push('/onboarding/network-select', extra: {
          'provisionToken': widget.provisionToken,
          'deviceId': info.deviceId,
        });
      }
    } on DeviceDiscoveryTimeoutError {
      setState(() => _error =
          'لم يتم العثور على جهاز نديم. تأكد من الاتصال بشبكة Nadeem-XXXX.');
    } catch (e) {
      setState(() => _error = 'خطأ: $e');
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('الاتصال بالجهاز')),
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Icon(Icons.wifi, size: 80, color: Colors.blue),
            const SizedBox(height: 24),
            const Text(
              'سيتم الاتصال بشبكة الجهاز (Nadeem-XXXX) تلقائياً.\nقد يُطلب منك السماح بذلك.',
              textAlign: TextAlign.center,
              style: TextStyle(fontSize: 16),
            ),
            if (_error != null) ...[
              const SizedBox(height: 16),
              Text(
                _error!,
                style: const TextStyle(color: Colors.red),
                textAlign: TextAlign.center,
              ),
            ],
            const SizedBox(height: 32),
            ElevatedButton(
              onPressed: _loading ? null : _connectAndProbe,
              child: _loading
                  ? const Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        CircularProgressIndicator(),
                        SizedBox(height: 8),
                        Text('جاري البحث عن الجهاز...'),
                      ],
                    )
                  : const Text('اتصل الآن'),
            ),
          ],
        ),
      ),
    );
  }
}
