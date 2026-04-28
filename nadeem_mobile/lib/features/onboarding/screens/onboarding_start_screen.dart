import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/ui/gradient_button.dart';

class OnboardingStartScreen extends ConsumerStatefulWidget {
  const OnboardingStartScreen({super.key});

  @override
  ConsumerState<OnboardingStartScreen> createState() => _State();
}

class _State extends ConsumerState<OnboardingStartScreen> {
  bool _loading = false;
  String? _error;

  Future<void> _continue() async {
    setState(() {
      _loading = true;
      _error = null;
    });
    try {
      final token = await ref.read(authServiceProvider).getToken();
      if (token == null) {
        if (mounted) context.go('/login');
        return;
      }
      final pt =
          await ref.read(backendClientProvider).mintProvisionToken(token);
      if (mounted) context.push('/onboarding/wifi-join', extra: pt.token);
    } catch (e) {
      setState(() => _error = 'تعذّر الحصول على رمز التوصيل: $e');
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('إضافة جهاز نديم')),
      body: SafeArea(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              const _IconCircle(icon: Icons.speaker_rounded),
              const SizedBox(height: 32),
              const Text(
                'شغّل جهاز نديم',
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
                'تأكد من تشغيل جهاز نديم الجديد.\nيجب أن تومض المصابيح باللون الأزرق.',
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
              const SizedBox(height: 40),
              GradientButton(
                label: 'متابعة',
                icon: Icons.arrow_forward_rounded,
                loading: _loading,
                onPressed: _loading ? null : _continue,
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
