import 'package:flutter/material.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:go_router/go_router.dart';
import 'package:nadeem_mobile/core/theme/nadeem_theme.dart';
import 'package:nadeem_mobile/features/auth/login_screen.dart';
import 'package:nadeem_mobile/features/dashboard/dashboard_screen.dart';
import 'package:nadeem_mobile/features/history/history_screen.dart';
import 'package:nadeem_mobile/features/library/library_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/network_select_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/onboarding_start_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/provision_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/success_screen.dart';
import 'package:nadeem_mobile/features/onboarding/screens/wifi_join_screen.dart';
import 'package:nadeem_mobile/features/settings/settings_screen.dart';
import 'package:nadeem_mobile/features/shell/main_shell.dart';

final _router = GoRouter(
  initialLocation: '/login',
  routes: [
    GoRoute(path: '/login', builder: (_, __) => const LoginScreen()),
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
      builder: (_, state) =>
          SuccessScreen(deviceId: state.extra as String),
    ),
    StatefulShellRoute.indexedStack(
      builder: (_, __, navShell) => MainShell(
        currentIndex: navShell.currentIndex,
        onTabSelected: navShell.goBranch,
        body: navShell,
      ),
      branches: [
        StatefulShellBranch(routes: [
          GoRoute(
            path: '/dashboard',
            builder: (_, __) => const DashboardScreen(),
          ),
        ]),
        StatefulShellBranch(routes: [
          GoRoute(
            path: '/library',
            builder: (_, __) => const LibraryScreen(),
          ),
        ]),
        StatefulShellBranch(routes: [
          GoRoute(
            path: '/history',
            builder: (_, __) => const HistoryScreen(),
          ),
        ]),
        StatefulShellBranch(routes: [
          GoRoute(
            path: '/settings',
            builder: (_, __) => const SettingsScreen(),
          ),
        ]),
      ],
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
      theme: buildNadeemTheme(),
      locale: const Locale('ar'),
      supportedLocales: const [Locale('ar'), Locale('en')],
      localizationsDelegates: const [
        GlobalMaterialLocalizations.delegate,
        GlobalWidgetsLocalizations.delegate,
        GlobalCupertinoLocalizations.delegate,
      ],
      builder: (context, child) => Directionality(
        textDirection: TextDirection.rtl,
        child: child!,
      ),
    );
  }
}
