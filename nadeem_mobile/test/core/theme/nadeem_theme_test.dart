import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/theme/nadeem_theme.dart';

void main() {
  group('buildNadeemTheme', () {
    final theme = buildNadeemTheme();

    test('uses Material 3', () {
      expect(theme.useMaterial3, isTrue);
    });

    test('primary color matches design token', () {
      expect(theme.colorScheme.primary, NadeemColors.primary);
      expect(theme.colorScheme.onPrimary, NadeemColors.onPrimary);
      expect(theme.colorScheme.surface, NadeemColors.surface);
    });

    test('default body font is Cairo', () {
      expect(theme.textTheme.bodyMedium?.fontFamily, 'Cairo');
    });

    test('headline font is Baloo Bhaijaan 2', () {
      expect(theme.textTheme.headlineLarge?.fontFamily, 'BalooBhaijaan2');
      expect(theme.textTheme.displayLarge?.fontFamily, 'BalooBhaijaan2');
    });

    test('scaffold background is the surface token', () {
      expect(theme.scaffoldBackgroundColor, NadeemColors.surface);
    });
  });
}
