import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/ui/blur_orb.dart';
import 'package:nadeem_mobile/core/ui/gradient_button.dart';

class LoginScreen extends ConsumerStatefulWidget {
  const LoginScreen({super.key});

  @override
  ConsumerState<LoginScreen> createState() => _LoginScreenState();
}

class _LoginScreenState extends ConsumerState<LoginScreen> {
  final _formKey = GlobalKey<FormState>();
  final _emailCtrl = TextEditingController();
  final _passCtrl = TextEditingController();
  bool _loading = false;
  String? _error;

  @override
  void dispose() {
    _emailCtrl.dispose();
    _passCtrl.dispose();
    super.dispose();
  }

  Future<void> _submit() async {
    if (!_formKey.currentState!.validate()) return;
    setState(() {
      _loading = true;
      _error = null;
    });
    try {
      await ref.read(authServiceProvider).login(
            _emailCtrl.text.trim(),
            _passCtrl.text,
          );
      if (mounted) context.go('/dashboard');
    } on BackendAuthException {
      setState(() => _error = 'البريد الإلكتروني أو كلمة المرور غير صحيحة');
    } catch (_) {
      setState(() => _error = 'تعذّر الاتصال. حاول مجدداً');
    } finally {
      if (mounted) setState(() => _loading = false);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Stack(
        children: [
          const Positioned(
            bottom: -120,
            left: -120,
            child: BlurOrb(size: 360, color: NadeemColors.primaryContainer),
          ),
          const Positioned(
            top: -120,
            right: -120,
            child: BlurOrb(size: 320, color: NadeemColors.secondaryContainer),
          ),
          SafeArea(
            child: Center(
              child: SingleChildScrollView(
                padding: const EdgeInsets.all(24),
                child: Form(
                  key: _formKey,
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      Container(
                        width: 128,
                        height: 128,
                        decoration: const BoxDecoration(
                          color: NadeemColors.surfaceContainerHighest,
                          shape: BoxShape.circle,
                          boxShadow: [
                            BoxShadow(
                              color: Color(0x1A00668A),
                              blurRadius: 30,
                              offset: Offset(0, 10),
                            ),
                          ],
                        ),
                        child: const Icon(
                          Icons.school_rounded,
                          size: 64,
                          color: NadeemColors.primary,
                        ),
                      ),
                      const SizedBox(height: 32),
                      const Text(
                        'أهلاً بك في نديم',
                        style: TextStyle(
                          fontFamily: 'BalooBhaijaan2',
                          fontSize: 36,
                          fontWeight: FontWeight.w800,
                          color: NadeemColors.primary,
                        ),
                      ),
                      const SizedBox(height: 8),
                      const Text(
                        'مغامرات تعليمية ممتعة تنتظرك',
                        style: TextStyle(
                          fontSize: 16,
                          color: NadeemColors.onSurfaceVariant,
                        ),
                      ),
                      const SizedBox(height: 32),
                      Container(
                        padding: const EdgeInsets.all(24),
                        decoration: BoxDecoration(
                          color: NadeemColors.surfaceContainerLow,
                          borderRadius: BorderRadius.circular(24),
                        ),
                        child: Column(
                          children: [
                            TextFormField(
                              controller: _emailCtrl,
                              keyboardType: TextInputType.emailAddress,
                              textDirection: TextDirection.ltr,
                              decoration: const InputDecoration(
                                labelText: 'البريد الإلكتروني',
                                prefixIcon: Icon(
                                  Icons.mail_outline,
                                  color: NadeemColors.primary,
                                ),
                              ),
                              validator: (v) =>
                                  (v == null || !v.contains('@'))
                                      ? 'بريد إلكتروني غير صالح'
                                      : null,
                            ),
                            const SizedBox(height: 16),
                            TextFormField(
                              controller: _passCtrl,
                              obscureText: true,
                              textDirection: TextDirection.ltr,
                              decoration: const InputDecoration(
                                labelText: 'كلمة المرور',
                                prefixIcon: Icon(
                                  Icons.lock_outline,
                                  color: NadeemColors.primary,
                                ),
                              ),
                              validator: (v) =>
                                  (v == null || v.isEmpty)
                                      ? 'أدخل كلمة المرور'
                                      : null,
                            ),
                            if (_error != null) ...[
                              const SizedBox(height: 12),
                              Text(
                                _error!,
                                style: const TextStyle(
                                  color: NadeemColors.error,
                                ),
                              ),
                            ],
                            const SizedBox(height: 24),
                            GradientButton(
                              label: 'تسجيل الدخول',
                              icon: Icons.login_rounded,
                              loading: _loading,
                              onPressed: _submit,
                            ),
                          ],
                        ),
                      ),
                      const SizedBox(height: 24),
                      Row(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          const Text(
                            'ليس لديك حساب؟ ',
                            style: TextStyle(
                              color: NadeemColors.onSurfaceVariant,
                            ),
                          ),
                          GestureDetector(
                            onTap: () {},
                            child: const Text(
                              'إنشاء حساب جديد',
                              style: TextStyle(
                                color: NadeemColors.secondary,
                                fontWeight: FontWeight.w700,
                              ),
                            ),
                          ),
                        ],
                      ),
                    ],
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}
