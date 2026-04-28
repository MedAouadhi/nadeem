import 'package:flutter/material.dart';
import 'nadeem_colors.dart';

ThemeData buildNadeemTheme() {
  const colorScheme = ColorScheme(
    brightness: Brightness.light,
    primary: NadeemColors.primary,
    onPrimary: NadeemColors.onPrimary,
    primaryContainer: NadeemColors.primaryContainer,
    onPrimaryContainer: NadeemColors.onPrimaryContainer,
    secondary: NadeemColors.secondary,
    onSecondary: NadeemColors.onSecondary,
    secondaryContainer: NadeemColors.secondaryContainer,
    onSecondaryContainer: NadeemColors.onSecondaryContainer,
    tertiary: NadeemColors.tertiary,
    onTertiary: NadeemColors.onTertiary,
    tertiaryContainer: NadeemColors.tertiaryContainer,
    onTertiaryContainer: NadeemColors.onTertiaryContainer,
    error: NadeemColors.error,
    onError: NadeemColors.onError,
    errorContainer: NadeemColors.errorContainer,
    onErrorContainer: NadeemColors.onErrorContainer,
    surface: NadeemColors.surface,
    onSurface: NadeemColors.onSurface,
    surfaceContainerLowest: NadeemColors.surfaceContainerLowest,
    surfaceContainerLow: NadeemColors.surfaceContainerLow,
    surfaceContainer: NadeemColors.surfaceContainer,
    surfaceContainerHigh: NadeemColors.surfaceContainerHigh,
    surfaceContainerHighest: NadeemColors.surfaceContainerHighest,
    surfaceDim: NadeemColors.surfaceDim,
    surfaceBright: NadeemColors.surfaceBright,
    onSurfaceVariant: NadeemColors.onSurfaceVariant,
    outline: NadeemColors.outline,
    outlineVariant: NadeemColors.outlineVariant,
    inverseSurface: NadeemColors.inverseSurface,
    onInverseSurface: NadeemColors.inverseOnSurface,
    inversePrimary: NadeemColors.inversePrimary,
    surfaceTint: NadeemColors.surfaceTint,
  );

  const headline = 'BalooBhaijaan2';
  const body = 'Cairo';

  final textTheme = const TextTheme(
    displayLarge: TextStyle(fontFamily: headline, fontWeight: FontWeight.w800),
    displayMedium: TextStyle(fontFamily: headline, fontWeight: FontWeight.w800),
    displaySmall: TextStyle(fontFamily: headline, fontWeight: FontWeight.w700),
    headlineLarge: TextStyle(fontFamily: headline, fontWeight: FontWeight.w700),
    headlineMedium: TextStyle(fontFamily: headline, fontWeight: FontWeight.w700),
    headlineSmall: TextStyle(fontFamily: headline, fontWeight: FontWeight.w700),
    titleLarge: TextStyle(fontFamily: headline, fontWeight: FontWeight.w700),
    titleMedium: TextStyle(fontFamily: body, fontWeight: FontWeight.w600),
    titleSmall: TextStyle(fontFamily: body, fontWeight: FontWeight.w600),
    bodyLarge: TextStyle(fontFamily: body),
    bodyMedium: TextStyle(fontFamily: body),
    bodySmall: TextStyle(fontFamily: body),
    labelLarge: TextStyle(fontFamily: body, fontWeight: FontWeight.w700),
    labelMedium: TextStyle(fontFamily: body, fontWeight: FontWeight.w600),
    labelSmall: TextStyle(fontFamily: body, fontWeight: FontWeight.w600),
  ).apply(
    bodyColor: NadeemColors.onSurface,
    displayColor: NadeemColors.onSurface,
  );

  return ThemeData(
    useMaterial3: true,
    colorScheme: colorScheme,
    fontFamily: body,
    textTheme: textTheme,
    scaffoldBackgroundColor: NadeemColors.surface,
    appBarTheme: const AppBarTheme(
      backgroundColor: NadeemColors.surface,
      foregroundColor: NadeemColors.primary,
      elevation: 0,
      centerTitle: true,
      titleTextStyle: TextStyle(
        fontFamily: headline,
        fontSize: 28,
        fontWeight: FontWeight.w800,
        color: NadeemColors.primary,
      ),
    ),
    inputDecorationTheme: InputDecorationTheme(
      filled: true,
      fillColor: NadeemColors.surfaceContainerHighest,
      border: OutlineInputBorder(
        borderRadius: BorderRadius.circular(16),
        borderSide: BorderSide.none,
      ),
      enabledBorder: OutlineInputBorder(
        borderRadius: BorderRadius.circular(16),
        borderSide: BorderSide.none,
      ),
      focusedBorder: OutlineInputBorder(
        borderRadius: BorderRadius.circular(16),
        borderSide: const BorderSide(color: NadeemColors.primary, width: 2),
      ),
      contentPadding: const EdgeInsets.symmetric(horizontal: 16, vertical: 16),
    ),
    cardTheme: CardTheme(
      color: NadeemColors.surfaceContainerHighest,
      elevation: 0,
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
    ),
    elevatedButtonTheme: ElevatedButtonThemeData(
      style: ElevatedButton.styleFrom(
        backgroundColor: NadeemColors.primary,
        foregroundColor: NadeemColors.onPrimary,
        textStyle: const TextStyle(
          fontFamily: body,
          fontWeight: FontWeight.w700,
          fontSize: 16,
        ),
        padding: const EdgeInsets.symmetric(vertical: 16, horizontal: 24),
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(9999),
        ),
        elevation: 0,
      ),
    ),
  );
}
