import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/providers.dart';

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
      body: SafeArea(
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Form(
            key: _formKey,
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Text(
                  'نديم',
                  style: Theme.of(context).textTheme.displayMedium,
                  textAlign: TextAlign.center,
                ),
                const SizedBox(height: 32),
                TextFormField(
                  controller: _emailCtrl,
                  decoration:
                      const InputDecoration(labelText: 'البريد الإلكتروني'),
                  keyboardType: TextInputType.emailAddress,
                  textDirection: TextDirection.ltr,
                  validator: (v) =>
                      (v == null || !v.contains('@'))
                          ? 'بريد إلكتروني غير صالح'
                          : null,
                ),
                const SizedBox(height: 16),
                TextFormField(
                  controller: _passCtrl,
                  decoration:
                      const InputDecoration(labelText: 'كلمة المرور'),
                  obscureText: true,
                  textDirection: TextDirection.ltr,
                  validator: (v) =>
                      (v == null || v.isEmpty) ? 'أدخل كلمة المرور' : null,
                ),
                if (_error != null) ...[
                  const SizedBox(height: 12),
                  Text(
                    _error!,
                    style: TextStyle(
                        color: Theme.of(context).colorScheme.error),
                  ),
                ],
                const SizedBox(height: 24),
                ElevatedButton(
                  onPressed: _loading ? null : _submit,
                  child: _loading
                      ? const CircularProgressIndicator()
                      : const Text('تسجيل الدخول'),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
