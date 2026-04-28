import 'package:flutter/material.dart';
import 'package:nadeem_mobile/core/ui/nadeem_bottom_nav.dart';

class MainShell extends StatelessWidget {
  const MainShell({
    super.key,
    required this.body,
    required this.currentIndex,
    required this.onTabSelected,
  });

  final Widget body;
  final int currentIndex;
  final ValueChanged<int> onTabSelected;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      extendBody: true,
      body: body,
      bottomNavigationBar: NadeemBottomNav(
        currentIndex: currentIndex,
        onTap: onTabSelected,
      ),
    );
  }
}
