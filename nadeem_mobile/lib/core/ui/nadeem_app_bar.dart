import 'package:flutter/material.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';

class NadeemAppBar extends StatelessWidget implements PreferredSizeWidget {
  const NadeemAppBar({
    super.key,
    this.title = 'نديم',
    this.onMenuTap,
    this.avatarUrl,
    this.onAvatarTap,
  });

  final String title;
  final VoidCallback? onMenuTap;
  final String? avatarUrl;
  final VoidCallback? onAvatarTap;

  @override
  Size get preferredSize => const Size.fromHeight(72);

  @override
  Widget build(BuildContext context) {
    return SafeArea(
      bottom: false,
      child: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 24, vertical: 16),
        child: Row(
          children: [
            IconButton(
              onPressed: onMenuTap ??
                  () => Scaffold.maybeOf(context)?.openDrawer(),
              icon: const Icon(Icons.menu, size: 28, color: NadeemColors.primary),
            ),
            Expanded(
              child: Text(
                title,
                textAlign: TextAlign.center,
                style: Theme.of(context).appBarTheme.titleTextStyle,
              ),
            ),
            GestureDetector(
              onTap: onAvatarTap,
              child: CircleAvatar(
                radius: 20,
                backgroundColor: NadeemColors.surfaceContainerHighest,
                backgroundImage:
                    avatarUrl != null ? NetworkImage(avatarUrl!) : null,
                child: avatarUrl == null
                    ? const Icon(Icons.person, color: NadeemColors.primary)
                    : null,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
