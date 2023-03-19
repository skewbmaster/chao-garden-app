import 'dart:async';

import 'package:flutter/material.dart';
import '../styles/textstyles.dart';
import 'chao.dart';

class ChaoOverWorldDisplay extends StatefulWidget {
  const ChaoOverWorldDisplay({super.key, required this.chaoList});

  final List<Chao> chaoList;

  @override
  State<ChaoOverWorldDisplay> createState() => ChaoOverWorldState(this.chaoList);
}

class ChaoOverWorldState extends State<ChaoOverWorldDisplay> {
  List<Chao> chaoList;

  Timer? timer;

  ChaoOverWorldState(this.chaoList) {
    timer = Timer.periodic(const Duration(microseconds: 16666), (timer) => updateAllChao());
  }

  void updateAllChao() {
    for (Chao chao in chaoList) {
      chao.runBehaviours();
    }
    setState(() {});
  }

  List<Widget> buildChaoWithDisplay(Chao chao, int index) {
    List<Widget> items = [];

    if (!chao.showingStats) {
      items.add(Text(
        chao.getName(),
        style: ChaoTextStyles.chaoNames,
      ));
    } else if (!chao.showStatsUnder) {
      items.add(Card());
    }

    items.add(
      GestureDetector(
        onTap: () => setState(() => chao.displayChaoStats()),
        child: Image.asset(
          height: 57,
          width: 69,
          chao.sprite,
        ),
      ),
    );

    if (chao.showingStats && chao.showStatsUnder) {
      items.add(Card());
    }

    return items;
  }

  @override
  Widget build(BuildContext context) {
    List<Positioned> chaoPositioned = [];

    for (int i = 0; i < chaoList.length; i++) {
      chaoPositioned.add(
        Positioned(
          top: chaoList[i].position.y,
          left: chaoList[i].position.x,
          child: Column(
            children: buildChaoWithDisplay(chaoList[i], i),
          ),
        ),
      );
    }

    return Stack(children: chaoPositioned);
  }
}
