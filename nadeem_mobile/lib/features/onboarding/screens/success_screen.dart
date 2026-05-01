import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/ui/gradient_button.dart';

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
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                const Center(
                  child: _SuccessCircle(),
                ),
                const SizedBox(height: 32),
                const Text(
                  'نديم متصل الآن!',
                  textAlign: TextAlign.center,
                  style: TextStyle(
                    fontFamily: 'BalooBhaijaan2',
                    fontSize: 32,
                    fontWeight: FontWeight.w800,
                    color: NadeemColors.primary,
                  ),
                ),
                const SizedBox(height: 12),
                Text(
                  'معرّف الجهاز: $deviceId',
                  textAlign: TextAlign.center,
                  style: const TextStyle(
                    color: NadeemColors.onSurfaceVariant,
                    fontSize: 14,
                  ),
                ),
                const SizedBox(height: 48),
                GradientButton(
                  label: 'العودة للرئيسية',
                  icon: Icons.home_rounded,
                  onPressed: () => context.go('/dashboard'),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}

class _SuccessCircle extends StatelessWidget {
  const _SuccessCircle();

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 128,
      height: 128,
      decoration: BoxDecoration(
        color: NadeemColors.onlineDot.withOpacity(0.1),
        shape: BoxShape.circle,
        boxShadow: const [
          BoxShadow(
            color: Color(0x2010B981),
            blurRadius: 30,
            offset: Offset(0, 10),
          ),
        ],
      ),
      child: const Icon(
        Icons.check_circle_rounded,
        size: 72,
        color: NadeemColors.onlineDot,
      ),
    );
  }
}
