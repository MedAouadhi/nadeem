import 'package:flutter/material.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mocktail/mocktail.dart';
import 'package:nadeem_mobile/core/api/backend_client.dart';
import 'package:nadeem_mobile/core/models/device.dart';
import 'package:nadeem_mobile/core/providers.dart';
import 'package:nadeem_mobile/core/theme/nadeem_theme.dart';
import 'package:nadeem_mobile/features/auth/auth_service.dart';
import 'package:nadeem_mobile/features/dashboard/dashboard_screen.dart';

class _MockBackend extends Mock implements BackendClient {}

class _MockAuth extends Mock implements AuthService {}

void main() {
  late _MockBackend backend;
  late _MockAuth auth;

  setUp(() {
    backend = _MockBackend();
    auth = _MockAuth();
  });

  Widget buildSubject() => ProviderScope(
        overrides: [
          backendClientProvider.overrideWithValue(backend),
          authServiceProvider.overrideWithValue(auth),
        ],
        child: MaterialApp(
          theme: buildNadeemTheme(),
          locale: const Locale('ar'),
          supportedLocales: const [Locale('ar')],
          localizationsDelegates: const [
            GlobalMaterialLocalizations.delegate,
            GlobalWidgetsLocalizations.delegate,
            GlobalCupertinoLocalizations.delegate,
          ],
          home: const Directionality(
            textDirection: TextDirection.rtl,
            child: DashboardScreen(),
          ),
        ),
      );

  testWidgets('Dashboard shows greeting and stat cards', (tester) async {
    when(() => auth.getToken()).thenAnswer((_) async => 'tok');
    when(() => backend.listDevices('tok')).thenAnswer((_) async => [
          const Device(deviceId: 'd1', name: 'سمسم 1', onlineStatus: true),
        ]);

    await tester.pumpWidget(buildSubject());
    // pump enough frames to let the FutureProvider resolve; OnlineDot has an
    // infinite animation so pumpAndSettle would time out.
    await tester.pump();
    await tester.pump(const Duration(seconds: 1));

    expect(find.text('مرحباً، أهالي نديم!'), findsOneWidget);
    expect(find.text('ساعات الاستماع'), findsOneWidget);
    expect(find.text('سمسم مملوك'), findsOneWidget);
    expect(find.text('حالة الجهاز'), findsOneWidget);
    expect(find.text('سمسم 1'), findsOneWidget);
  });

  testWidgets('Dashboard shows empty state when no devices', (tester) async {
    when(() => auth.getToken()).thenAnswer((_) async => 'tok');
    when(() => backend.listDevices('tok')).thenAnswer((_) async => []);

    await tester.pumpWidget(buildSubject());
    // pump enough frames to let the FutureProvider resolve; OnlineDot has an
    // infinite animation so pumpAndSettle would time out.
    await tester.pump();
    await tester.pump(const Duration(seconds: 1));

    expect(find.text('لا توجد أجهزة مرتبطة بعد'), findsOneWidget);
  });
}
