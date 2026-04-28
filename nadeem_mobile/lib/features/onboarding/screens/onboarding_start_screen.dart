import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/providers.dart';

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
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Icon(Icons.speaker, size: 80, color: Colors.blue),
            const SizedBox(height: 24),
            const Text(
              'تأكد من تشغيل جهاز نديم الجديد.\nيجب أن تومض المصابيح باللون الأزرق.',
              textAlign: TextAlign.center,
              style: TextStyle(fontSize: 18),
            ),
            if (_error != null) ...[
              const SizedBox(height: 16),
              Text(_error!, style: const TextStyle(color: Colors.red)),
            ],
            const SizedBox(height: 32),
            ElevatedButton(
              onPressed: _loading ? null : _continue,
              child: _loading
                  ? const CircularProgressIndicator()
                  : const Text('متابعة'),
            ),
          ],
        ),
      ),
    );
  }
}
