import 'package:flutter/material.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';

class NadeemBottomNav extends StatelessWidget {
  const NadeemBottomNav({
    super.key,
    required this.currentIndex,
    required this.onTap,
  });

  final int currentIndex;
  final ValueChanged<int> onTap;

  static const _items = [
    (icon: Icons.home_rounded, label: 'الرئيسية'),
    (icon: Icons.auto_stories_rounded, label: 'المكتبة'),
    (icon: Icons.forum_rounded, label: 'السجل'),
    (icon: Icons.settings_rounded, label: 'الإعدادات'),
  ];

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.fromLTRB(16, 8, 16, 24),
      decoration: const BoxDecoration(
        color: Color(0xB3FAFAF5),
        borderRadius: BorderRadius.vertical(top: Radius.circular(48)),
        boxShadow: [
          BoxShadow(
            color: Color(0x0F00668A),
            blurRadius: 30,
            offset: Offset(0, -4),
          ),
        ],
      ),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceAround,
        children: List.generate(_items.length, (i) {
          final active = i == currentIndex;
          final item = _items[i];
          return InkWell(
            onTap: () => onTap(i),
            borderRadius: BorderRadius.circular(9999),
            child: AnimatedContainer(
              duration: const Duration(milliseconds: 250),
              padding: EdgeInsets.all(active ? 12 : 8),
              decoration: BoxDecoration(
                gradient: active ? NadeemColors.primaryGradient : null,
                borderRadius: BorderRadius.circular(9999),
              ),
              child: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Icon(
                    item.icon,
                    size: active ? 26 : 24,
                    color: active
                        ? NadeemColors.onPrimary
                        : NadeemColors.onSurface.withOpacity(0.6),
                  ),
                  const SizedBox(height: 4),
                  Text(
                    item.label,
                    style: TextStyle(
                      fontSize: 10,
                      fontWeight: FontWeight.w700,
                      color: active
                          ? NadeemColors.onPrimary
                          : NadeemColors.onSurface.withOpacity(0.6),
                    ),
                  ),
                ],
              ),
            ),
          );
        }),
      ),
    );
  }
}
