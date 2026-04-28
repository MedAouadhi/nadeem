import 'package:flutter/material.dart';

/// Material 3 design tokens from nadeem_mobile/design.txt.
class NadeemColors {
  NadeemColors._();

  // Primary (blue)
  static const primary = Color(0xFF00668A);
  static const primaryContainer = Color(0xFF4FB6E9);
  static const primaryFixed = Color(0xFFC4E7FF);
  static const primaryFixedDim = Color(0xFF7CD0FF);
  static const onPrimary = Color(0xFFFFFFFF);
  static const onPrimaryFixed = Color(0xFF001E2C);
  static const onPrimaryFixedVariant = Color(0xFF004C69);
  static const onPrimaryContainer = Color(0xFF00455F);
  static const inversePrimary = Color(0xFF7CD0FF);

  // Secondary (orange)
  static const secondary = Color(0xFF875200);
  static const secondaryContainer = Color(0xFFFDAA3F);
  static const secondaryFixed = Color(0xFFFFDDBA);
  static const secondaryFixedDim = Color(0xFFFFB866);
  static const onSecondary = Color(0xFFFFFFFF);
  static const onSecondaryFixed = Color(0xFF2B1700);
  static const onSecondaryFixedVariant = Color(0xFF673D00);
  static const onSecondaryContainer = Color(0xFF6D4100);

  // Tertiary (olive)
  static const tertiary = Color(0xFF686000);
  static const tertiaryContainer = Color(0xFFBAAE3B);
  static const tertiaryFixed = Color(0xFFF3E56C);
  static const tertiaryFixedDim = Color(0xFFD6C953);
  static const onTertiary = Color(0xFFFFFFFF);
  static const onTertiaryFixed = Color(0xFF1F1C00);
  static const onTertiaryFixedVariant = Color(0xFF4E4800);
  static const onTertiaryContainer = Color(0xFF474100);

  // Surface
  static const surface = Color(0xFFFAFAF5);
  static const surfaceBright = Color(0xFFFAFAF5);
  static const surfaceContainerLowest = Color(0xFFFFFFFF);
  static const surfaceContainerLow = Color(0xFFF4F4EF);
  static const surfaceContainer = Color(0xFFEEEEE9);
  static const surfaceContainerHigh = Color(0xFFE8E8E3);
  static const surfaceContainerHighest = Color(0xFFE3E3DE);
  static const surfaceDim = Color(0xFFDADAD5);
  static const surfaceVariant = Color(0xFFE3E3DE);
  static const surfaceTint = Color(0xFF00668A);

  // On-neutral
  static const onSurface = Color(0xFF1A1C19);
  static const onSurfaceVariant = Color(0xFF3E484F);
  static const background = Color(0xFFFAFAF5);
  static const onBackground = Color(0xFF1A1C19);
  static const inverseSurface = Color(0xFF2F312E);
  static const inverseOnSurface = Color(0xFFF1F1EC);
  static const outline = Color(0xFF6F7880);
  static const outlineVariant = Color(0xFFBEC8D0);

  // Error
  static const error = Color(0xFFBA1A1A);
  static const errorContainer = Color(0xFFFFDAD6);
  static const onError = Color(0xFFFFFFFF);
  static const onErrorContainer = Color(0xFF93000A);

  // Online status indicator (outside the M3 scheme)
  static const onlineDot = Color(0xFF10B981); // emerald-500

  // Brand gradient
  static const Gradient primaryGradient = LinearGradient(
    begin: Alignment.bottomLeft,
    end: Alignment.topRight,
    colors: [primary, primaryContainer],
  );
}
