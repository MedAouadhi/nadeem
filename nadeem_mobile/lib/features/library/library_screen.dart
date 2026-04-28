import 'package:flutter/material.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';
import 'package:nadeem_mobile/core/ui/nadeem_app_bar.dart';

class LibraryScreen extends StatelessWidget {
  const LibraryScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return const Scaffold(
      appBar: NadeemAppBar(title: 'مكتبة سمسم'),
      body: Center(
        child: Padding(
          padding: EdgeInsets.all(32),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              Icon(Icons.auto_stories_rounded,
                  size: 80, color: NadeemColors.primaryContainer),
              SizedBox(height: 24),
              Text(
                'قريباً',
                style: TextStyle(
                  fontFamily: 'BalooBhaijaan2',
                  fontWeight: FontWeight.w800,
                  fontSize: 32,
                  color: NadeemColors.primary,
                ),
              ),
              SizedBox(height: 8),
              Text(
                'استكشف الشخصيات والقصص قريباً',
                style: TextStyle(color: NadeemColors.onSurfaceVariant),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
