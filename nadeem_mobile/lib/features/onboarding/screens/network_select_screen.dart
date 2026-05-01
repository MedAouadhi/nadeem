import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/models/wifi_network.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/ui/gradient_button.dart';

final _scanProvider =
    FutureProvider.autoDispose<List<WifiNetwork>>((ref) async {
  return ref.read(deviceClientProvider).scanNetworks();
});

class NetworkSelectScreen extends ConsumerStatefulWidget {
  final String provisionToken;
  final String deviceId;

  const NetworkSelectScreen({
    super.key,
    required this.provisionToken,
    required this.deviceId,
  });

  @override
  ConsumerState<NetworkSelectScreen> createState() => _State();
}

class _State extends ConsumerState<NetworkSelectScreen> {
  WifiNetwork? _selected;
  final _passCtrl = TextEditingController();

  @override
  void dispose() {
    _passCtrl.dispose();
    super.dispose();
  }

  void _selectNetwork(WifiNetwork net) {
    setState(() {
      _selected = net;
      _passCtrl.clear();
    });
  }

  void _proceed() {
    if (_selected == null) return;
    if (!_selected!.isOpen && _passCtrl.text.isEmpty) return;

    context.push('/onboarding/provision', extra: {
      'ssid': _selected!.ssid,
      'password': _passCtrl.text,
      'provisionToken': widget.provisionToken,
      'deviceId': widget.deviceId,
    });
  }

  @override
  Widget build(BuildContext context) {
    final scanAsync = ref.watch(_scanProvider);

    return Scaffold(
      appBar: AppBar(title: const Text('اختر شبكة Wi-Fi')),
      body: scanAsync.when(
        loading: () => const Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              CircularProgressIndicator(color: NadeemColors.primary),
              SizedBox(height: 16),
              Text(
                'جاري البحث عن الشبكات...',
                style: TextStyle(color: NadeemColors.onSurfaceVariant),
              ),
            ],
          ),
        ),
        error: (e, _) => Center(
          child: Text(
            'خطأ: $e',
            style: const TextStyle(color: NadeemColors.error),
          ),
        ),
        data: (networks) => Column(
          children: [
            Expanded(
              child: ListView.separated(
                padding: const EdgeInsets.all(16),
                itemCount: networks.length,
                separatorBuilder: (_, __) => const SizedBox(height: 8),
                itemBuilder: (_, i) {
                  final net = networks[i];
                  final isSelected = _selected?.ssid == net.ssid;
                  return Column(
                    children: [
                      InkWell(
                        onTap: () => _selectNetwork(net),
                        borderRadius: BorderRadius.circular(16),
                        child: AnimatedContainer(
                          duration: const Duration(milliseconds: 200),
                          padding: const EdgeInsets.all(16),
                          decoration: BoxDecoration(
                            color: isSelected
                                ? NadeemColors.primaryContainer
                                    .withOpacity(0.2)
                                : NadeemColors.surfaceContainerHighest,
                            borderRadius: BorderRadius.circular(16),
                            border: isSelected
                                ? Border.all(
                                    color: NadeemColors.primary,
                                    width: 1.5,
                                  )
                                : null,
                          ),
                          child: Row(
                            children: [
                              Container(
                                width: 40,
                                height: 40,
                                decoration: BoxDecoration(
                                  color: isSelected
                                      ? NadeemColors.primary.withOpacity(0.1)
                                      : NadeemColors.surfaceContainer,
                                  shape: BoxShape.circle,
                                ),
                                child: Icon(
                                  Icons.wifi_rounded,
                                  color: isSelected
                                      ? NadeemColors.primary
                                      : NadeemColors.outline,
                                  size: 20,
                                ),
                              ),
                              const SizedBox(width: 12),
                              Expanded(
                                child: Column(
                                  crossAxisAlignment:
                                      CrossAxisAlignment.start,
                                  children: [
                                    Text(
                                      net.ssid,
                                      style: TextStyle(
                                        fontWeight: FontWeight.w700,
                                        color: isSelected
                                            ? NadeemColors.primary
                                            : NadeemColors.onSurface,
                                      ),
                                    ),
                                    Text(
                                      '${net.authLabel} · ${net.rssi} dBm',
                                      style: const TextStyle(
                                        fontSize: 12,
                                        color: NadeemColors.onSurfaceVariant,
                                      ),
                                    ),
                                  ],
                                ),
                              ),
                            ],
                          ),
                        ),
                      ),
                      if (isSelected && !net.isOpen) ...[
                        const SizedBox(height: 8),
                        TextFormField(
                          controller: _passCtrl,
                          decoration: const InputDecoration(
                            labelText: 'كلمة المرور',
                            prefixIcon: Icon(
                              Icons.lock_outline,
                              color: NadeemColors.primary,
                            ),
                          ),
                          obscureText: true,
                          textDirection: TextDirection.ltr,
                        ),
                      ],
                    ],
                  );
                },
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(24),
              child: GradientButton(
                label: 'متابعة',
                icon: Icons.arrow_forward_rounded,
                onPressed: _selected != null ? _proceed : null,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
