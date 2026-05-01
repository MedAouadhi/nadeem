import 'package:flutter/material.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:nadeem_mobile/core/theme/nadeem_theme.dart';
import 'package:nadeem_mobile/features/auth/login_screen.dart';

void main() {
  Widget buildSubject() => ProviderScope(
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
            child: LoginScreen(),
          ),
        ),
      );

  testWidgets('Login screen renders headline and fields', (tester) async {
    await tester.pumpWidget(buildSubject());
    await tester.pump();
    expect(find.text('أهلاً بك في نديم'), findsOneWidget);
    expect(find.text('البريد الإلكتروني'), findsOneWidget);
    expect(find.text('كلمة المرور'), findsOneWidget);
    expect(find.text('تسجيل الدخول'), findsOneWidget);
    expect(find.text('إنشاء حساب جديد'), findsOneWidget);
  });

  testWidgets('Login shows validation errors on empty submit', (tester) async {
    await tester.pumpWidget(buildSubject());
    await tester.pump();
    await tester.tap(find.text('تسجيل الدخول'));
    await tester.pump();
    expect(find.text('بريد إلكتروني غير صالح'), findsOneWidget);
    expect(find.text('أدخل كلمة المرور'), findsOneWidget);
  });
}
