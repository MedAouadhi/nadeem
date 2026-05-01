import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/ui/gradient_button.dart';
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
        setState(() {
          _error =
              'تعذّر الاتصال بالجهاز. يرجى الانضمام يدوياً من إعدادات Wi-Fi.';
          _loading = false;
        });
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
      setState(() =>
          _error = 'لم يتم العثور على جهاز نديم. تأكد من الاتصال بشبكة Nadeem-XXXX.');
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
      body: SafeArea(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              const _IconCircle(icon: Icons.wifi_rounded),
              const SizedBox(height: 32),
              const Text(
                'الاتصال بالجهاز',
                textAlign: TextAlign.center,
                style: TextStyle(
                  fontFamily: 'BalooBhaijaan2',
                  fontSize: 28,
                  fontWeight: FontWeight.w800,
                  color: NadeemColors.primary,
                ),
              ),
              const SizedBox(height: 12),
              const Text(
                'سيتم الاتصال بشبكة الجهاز (Nadeem-XXXX) تلقائياً.\nقد يُطلب منك السماح بذلك.',
                textAlign: TextAlign.center,
                style: TextStyle(
                  fontSize: 16,
                  color: NadeemColors.onSurfaceVariant,
                ),
              ),
              if (_error != null) ...[
                const SizedBox(height: 16),
                Text(
                  _error!,
                  textAlign: TextAlign.center,
                  style: const TextStyle(color: NadeemColors.error),
                ),
              ],
              if (_loading) ...[
                const SizedBox(height: 16),
                const Center(
                  child: Text(
                    'جاري البحث عن الجهاز...',
                    style: TextStyle(color: NadeemColors.onSurfaceVariant),
                  ),
                ),
              ],
              const SizedBox(height: 40),
              GradientButton(
                label: 'اتصل الآن',
                icon: Icons.wifi_find_rounded,
                loading: _loading,
                onPressed: _loading ? null : _connectAndProbe,
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _IconCircle extends StatelessWidget {
  const _IconCircle({required this.icon});

  final IconData icon;

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Container(
        width: 128,
        height: 128,
        decoration: const BoxDecoration(
          color: NadeemColors.surfaceContainerLow,
          shape: BoxShape.circle,
          boxShadow: [
            BoxShadow(
              color: Color(0x1A00668A),
              blurRadius: 30,
              offset: Offset(0, 10),
            ),
          ],
        ),
        child: Icon(icon, size: 64, color: NadeemColors.primary),
      ),
    );
  }
}
