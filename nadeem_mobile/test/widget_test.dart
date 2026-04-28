import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_test/flutter_test.dart';

import 'package:nadeem_mobile/app.dart';

void main() {
  testWidgets('NadeemApp renders without crashing', (WidgetTester tester) async {
    await tester.pumpWidget(const ProviderScope(child: NadeemApp()));
    // Allow the router to settle on the initial /login route.
    await tester.pump();
    expect(find.byType(NadeemApp), findsOneWidget);
  });
}
