import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/features/auth/login_screen.dart';
import 'package:nadeem_mobile/features/dashboard/dashboard_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/onboarding_start_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/wifi_join_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/network_select_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/provision_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/success_screen.dart';

final _router = GoRouter(
  initialLocation: '/login',
  routes: [
    GoRoute(path: '/login', builder: (_, __) => const LoginScreen()),
    GoRoute(path: '/dashboard', builder: (_, __) => const DashboardScreen()),
    GoRoute(
      path: '/onboarding',
      builder: (_, __) => const OnboardingStartScreen(),
    ),
    GoRoute(
      path: '/onboarding/wifi-join',
      builder: (_, state) {
        final provisionToken = state.extra as String;
        return WifiJoinScreen(provisionToken: provisionToken);
      },
    ),
    GoRoute(
      path: '/onboarding/network-select',
      builder: (_, state) {
        final extra = state.extra as Map<String, String>;
        return NetworkSelectScreen(
          provisionToken: extra['provisionToken']!,
          deviceId: extra['deviceId']!,
        );
      },
    ),
    GoRoute(
      path: '/onboarding/provision',
      builder: (_, state) {
        final extra = state.extra as Map<String, String>;
        return ProvisionScreen(
          ssid: extra['ssid']!,
          password: extra['password']!,
          provisionToken: extra['provisionToken']!,
          deviceId: extra['deviceId']!,
        );
      },
    ),
    GoRoute(
      path: '/onboarding/success',
      builder: (_, state) {
        final deviceId = state.extra as String;
        return SuccessScreen(deviceId: deviceId);
      },
    ),
  ],
);

class NadeemApp extends StatelessWidget {
  const NadeemApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp.router(
      title: 'Nadeem',
      routerConfig: _router,
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: const Color(0xFF4A90E2)),
        useMaterial3: true,
      ),
    );
  }
}
