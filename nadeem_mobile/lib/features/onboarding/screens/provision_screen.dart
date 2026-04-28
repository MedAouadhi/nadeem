import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/ui/gradient_button.dart';
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
      if (jwt == null) {
        context.go('/login');
        return;
      }
      await ref.read(provisionServiceProvider).provisionAndWait(
            ssid: widget.ssid,
            password: widget.password,
            provisionToken: widget.provisionToken,
            deviceId: widget.deviceId,
            jwt: jwt,
          );
      if (mounted) context.go('/onboarding/success', extra: widget.deviceId);
    } on ProvisionTimeoutError {
      if (mounted)
        setState(() => _error =
            'لم يتصل الجهاز بالشبكة خلال 60 ثانية.\nتحقق من كلمة مرور Wi-Fi ثم أعد المحاولة.');
    } catch (e) {
      if (mounted)
        setState(() => _error = 'تعذّر إتمام الإعداد. حاول مجدداً.');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('جاري إعداد الجهاز')),
      body: SafeArea(
        child: Center(
          child: Padding(
            padding: const EdgeInsets.all(24),
            child: _error != null
                ? Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    children: [
                      Center(
                        child: Container(
                          width: 128,
                          height: 128,
                          decoration: const BoxDecoration(
                            color: NadeemColors.errorContainer,
                            shape: BoxShape.circle,
                          ),
                          child: const Icon(
                            Icons.error_outline_rounded,
                            size: 64,
                            color: NadeemColors.error,
                          ),
                        ),
                      ),
                      const SizedBox(height: 24),
                      Text(
                        _error!,
                        textAlign: TextAlign.center,
                        style: const TextStyle(
                          fontSize: 16,
                          color: NadeemColors.onSurfaceVariant,
                        ),
                      ),
                      const SizedBox(height: 32),
                      GradientButton(
                        label: 'العودة للرئيسية',
                        icon: Icons.home_rounded,
                        onPressed: () => context.go('/dashboard'),
                      ),
                    ],
                  )
                : Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      const CircularProgressIndicator(
                        color: NadeemColors.primary,
                        strokeWidth: 3,
                      ),
                      const SizedBox(height: 24),
                      Text(
                        'إرسال الإعدادات إلى: ${widget.ssid}',
                        style: const TextStyle(
                          fontWeight: FontWeight.w600,
                          color: NadeemColors.onSurface,
                        ),
                      ),
                      const SizedBox(height: 8),
                      const Text(
                        'في انتظار اتصال الجهاز بالخادم...',
                        style: TextStyle(color: NadeemColors.onSurfaceVariant),
                      ),
                    ],
                  ),
          ),
        ),
      ),
    );
  }
}
