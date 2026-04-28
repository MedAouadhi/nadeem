import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/models/device.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/ui/nadeem_app_bar.dart';
import 'package:nadeem_mobile/core/ui/online_dot.dart';
import 'package:nadeem_mobile/core/ui/stat_card.dart';

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
    final anyOnline = devicesAsync.maybeWhen(
      data: (d) => d.any((x) => x.onlineStatus),
      orElse: () => false,
    );

    return Scaffold(
      appBar: const NadeemAppBar(),
      floatingActionButton: FloatingActionButton.extended(
        onPressed: () => context.push('/onboarding'),
        backgroundColor: NadeemColors.primary,
        foregroundColor: NadeemColors.onPrimary,
        icon: const Icon(Icons.add),
        label: const Text('إضافة جهاز'),
      ),
      body: ListView(
        padding: const EdgeInsets.fromLTRB(24, 8, 24, 120),
        children: [
          const Text(
            'مرحباً، أهالي نديم!',
            style: TextStyle(
              fontFamily: 'BalooBhaijaan2',
              fontSize: 36,
              fontWeight: FontWeight.w800,
              color: NadeemColors.primary,
              height: 1.1,
            ),
          ),
          const SizedBox(height: 8),
          const Text(
            'إليك ملخص نشاط طفلك اليوم.',
            style: TextStyle(
              fontSize: 16,
              color: NadeemColors.onSurfaceVariant,
              fontWeight: FontWeight.w500,
            ),
          ),
          const SizedBox(height: 32),
          const StatCard(
            icon: Icons.headphones_rounded,
            label: 'ساعات الاستماع',
            accent: NadeemColors.primary,
            value: '14',
          ),
          const SizedBox(height: 16),
          const StatCard(
            icon: Icons.smart_toy_rounded,
            label: 'سمسم مملوك',
            accent: NadeemColors.secondary,
            value: '8',
          ),
          const SizedBox(height: 16),
          StatCard(
            icon: Icons.router_rounded,
            label: 'حالة الجهاز',
            accent: NadeemColors.tertiary,
            trailing: Padding(
              padding: const EdgeInsets.only(top: 4),
              child: Row(
                children: [
                  const OnlineDot(),
                  const SizedBox(width: 8),
                  Text(
                    anyOnline ? 'متصل' : 'غير متصل',
                    style: const TextStyle(
                      fontWeight: FontWeight.w700,
                      fontSize: 18,
                      color: NadeemColors.onSurface,
                    ),
                  ),
                ],
              ),
            ),
          ),
          const SizedBox(height: 32),
          const Text(
            'أجهزتك',
            style: TextStyle(
              fontFamily: 'BalooBhaijaan2',
              fontSize: 22,
              fontWeight: FontWeight.w700,
              color: NadeemColors.onSurface,
            ),
          ),
          const SizedBox(height: 12),
          devicesAsync.when(
            loading: () =>
                const Center(child: CircularProgressIndicator()),
            error: (e, _) => Text(
              'خطأ في تحميل الأجهزة: $e',
              style: const TextStyle(color: NadeemColors.error),
            ),
            data: (devices) => devices.isEmpty
                ? const _EmptyDevices()
                : Column(
                    children: [
                      for (final d in devices) ...[
                        _DeviceRow(device: d),
                        const SizedBox(height: 12),
                      ],
                    ],
                  ),
          ),
        ],
      ),
    );
  }
}

class _DeviceRow extends StatelessWidget {
  const _DeviceRow({required this.device});

  final Device device;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: NadeemColors.surfaceContainerHighest,
        borderRadius: BorderRadius.circular(16),
      ),
      child: Row(
        children: [
          Container(
            width: 48,
            height: 48,
            decoration: BoxDecoration(
              color: NadeemColors.primaryContainer.withOpacity(0.2),
              shape: BoxShape.circle,
            ),
            child: const Icon(
              Icons.speaker_rounded,
              color: NadeemColors.primary,
            ),
          ),
          const SizedBox(width: 16),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  device.name ?? device.deviceId,
                  style: const TextStyle(
                    fontWeight: FontWeight.w700,
                    fontSize: 16,
                    color: NadeemColors.onSurface,
                  ),
                ),
                const SizedBox(height: 4),
                Row(
                  children: [
                    if (device.onlineStatus) const OnlineDot(size: 8),
                    if (device.onlineStatus) const SizedBox(width: 6),
                    Text(
                      device.onlineStatus ? 'متصل' : 'غير متصل',
                      style: TextStyle(
                        color: device.onlineStatus
                            ? NadeemColors.onlineDot
                            : NadeemColors.outline,
                        fontWeight: FontWeight.w600,
                        fontSize: 13,
                      ),
                    ),
                  ],
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}

class _EmptyDevices extends StatelessWidget {
  const _EmptyDevices();

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(24),
      decoration: BoxDecoration(
        color: NadeemColors.surfaceContainerLow,
        borderRadius: BorderRadius.circular(16),
      ),
      child: const Text(
        'لا توجد أجهزة مرتبطة بعد',
        textAlign: TextAlign.center,
        style: TextStyle(color: NadeemColors.onSurfaceVariant),
      ),
    );
  }
}
