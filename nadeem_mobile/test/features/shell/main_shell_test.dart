import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:nadeem_mobile/core/theme/nadeem_theme.dart';
import 'package:nadeem_mobile/features/shell/main_shell.dart';

void main() {
  testWidgets('MainShell renders body and bottom nav', (tester) async {
    await tester.pumpWidget(
      MaterialApp(
        theme: buildNadeemTheme(),
        home: MainShell(
          currentIndex: 0,
          onTabSelected: (_) {},
          body: const Text('HOME-BODY'),
        ),
      ),
    );
    expect(find.text('HOME-BODY'), findsOneWidget);
    expect(find.text('الرئيسية'), findsOneWidget);
    expect(find.text('المكتبة'), findsOneWidget);
    expect(find.text('السجل'), findsOneWidget);
    expect(find.text('الإعدادات'), findsOneWidget);
  });

  testWidgets('MainShell calls onTabSelected when nav item tapped', (tester) async {
    int tappedIndex = -1;
    await tester.pumpWidget(
      MaterialApp(
        theme: buildNadeemTheme(),
        home: MainShell(
          currentIndex: 0,
          onTabSelected: (i) => tappedIndex = i,
          body: const Text('BODY'),
        ),
      ),
    );
    await tester.tap(find.text('المكتبة'));
    expect(tappedIndex, 1);
  });
}
