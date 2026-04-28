import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/models/device.dart';
import 'package:nadeem_mobile/core/providers.dart';

final _devicesProvider = FutureProvider.autoDispose<List<Device>>((ref) async {
  final auth = ref.read(authServiceProvider);
  final token = await auth.getToken();
  if (token == null) return [];
  return ref.read(backendClientProvider).listDevices(token);
});

class DashboardScreen extends ConsumerWidget {
  const DashboardScreen({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final devicesAsync = ref.watch(_devicesProvider);

    return Scaffold(
      appBar: AppBar(title: const Text('أجهزة نديم')),
      body: devicesAsync.when(
        loading: () => const Center(child: CircularProgressIndicator()),
        error: (e, _) =>
            Center(child: Text('خطأ في تحميل الأجهزة: $e')),
        data: (devices) => devices.isEmpty
            ? const Center(
                child: Text('لا توجد أجهزة مرتبطة بعد'),
              )
            : ListView.builder(
                itemCount: devices.length,
                itemBuilder: (_, i) => ListTile(
                  title: Text(devices[i].name ?? devices[i].deviceId),
                  subtitle:
                      Text(devices[i].onlineStatus ? 'متصل' : 'غير متصل'),
                  leading: Icon(
                    Icons.speaker,
                    color: devices[i].onlineStatus
                        ? Colors.green
                        : Colors.grey,
                  ),
                ),
              ),
      ),
      floatingActionButton: FloatingActionButton.extended(
        onPressed: () => context.push('/onboarding'),
        icon: const Icon(Icons.add),
        label: const Text('إضافة جهاز'),
      ),
    );
  }
}
