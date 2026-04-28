import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/models/wifi_network.dart';
import 'package:nadeem_mobile/core/providers.dart';

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
              CircularProgressIndicator(),
              SizedBox(height: 16),
              Text('جاري البحث عن الشبكات...'),
            ],
          ),
        ),
        error: (e, _) => Center(child: Text('خطأ: $e')),
        data: (networks) => Column(
          children: [
            Expanded(
              child: ListView.builder(
                itemCount: networks.length,
                itemBuilder: (_, i) {
                  final net = networks[i];
                  final isSelected = _selected?.ssid == net.ssid;
                  return Column(
                    children: [
                      ListTile(
                        title: Text(net.ssid),
                        subtitle:
                            Text('${net.authLabel} · ${net.rssi} dBm'),
                        leading: Icon(
                          Icons.wifi,
                          color: isSelected
                              ? Theme.of(context).colorScheme.primary
                              : null,
                        ),
                        selected: isSelected,
                        onTap: () => _selectNetwork(net),
                      ),
                      if (isSelected && !net.isOpen)
                        Padding(
                          padding: const EdgeInsets.symmetric(
                              horizontal: 16, vertical: 4),
                          child: TextFormField(
                            controller: _passCtrl,
                            decoration: const InputDecoration(
                                labelText: 'كلمة المرور'),
                            obscureText: true,
                            textDirection: TextDirection.ltr,
                          ),
                        ),
                    ],
                  );
                },
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(16),
              child: ElevatedButton(
                onPressed: _selected != null ? _proceed : null,
                child: const Text('متابعة'),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
