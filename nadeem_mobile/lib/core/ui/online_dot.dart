import 'package:flutter/material.dart';
import 'package:nadeem_mobile/core/theme/nadeem_colors.dart';

class OnlineDot extends StatefulWidget {
  const OnlineDot({
    super.key,
    this.color = NadeemColors.onlineDot,
    this.size = 12,
  });

  final Color color;
  final double size;

  @override
  State<OnlineDot> createState() => _OnlineDotState();
}

class _OnlineDotState extends State<OnlineDot>
    with SingleTickerProviderStateMixin {
  late final AnimationController _ctrl;

  @override
  void initState() {
    super.initState();
    _ctrl = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 1500),
    )..repeat();
  }

  @override
  void dispose() {
    _ctrl.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: widget.size,
      height: widget.size,
      child: Stack(
        alignment: Alignment.center,
        children: [
          AnimatedBuilder(
            animation: _ctrl,
            builder: (_, __) => Transform.scale(
              scale: 1 + _ctrl.value,
              child: Opacity(
                opacity: 0.75 * (1 - _ctrl.value),
                child: Container(
                  decoration: BoxDecoration(
                    color: widget.color,
                    shape: BoxShape.circle,
                  ),
                ),
              ),
            ),
          ),
          Container(
            decoration: BoxDecoration(
              color: widget.color,
              shape: BoxShape.circle,
            ),
          ),
        ],
      ),
    );
  }
}
