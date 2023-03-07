import 'package:flutter/material.dart';

import '../network/computers.dart';

class ComputerTab extends StatelessWidget {
  const ComputerTab({super.key, required this.computerInfo, required this.connectToComputer});

  final Computer computerInfo;

  final Future<void> Function(Computer) connectToComputer;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: () {
        connectToComputer(computerInfo);
      },
      child: Container(
        height: 140,
        width: 240,
        alignment: Alignment.center,
        margin: const EdgeInsets.only(top: 5, bottom: 5),
        decoration: BoxDecoration(
          border: Border.all(
            color: const Color(0xFFCE1ECE),
            width: 3,
          ),
          borderRadius: BorderRadius.circular(15),
          color: const Color(0xFFF9DCF9),
        ),
        child: makeCardDisplay(),
      ),
    );
  }

  Column makeCardDisplay() {
    String gardenStr = (computerInfo.gardenType) == GardenType.sadx ? "sadx" : "sa2";

    return Column(
      children: [
        Padding(
          padding: EdgeInsets.all(5),
          child: Image.asset(
            ("assets/chaogarden${gardenStr}logo.png"),
          ),
        ),
        Text(
          computerInfo.compName,
          maxLines: 2,
          style: const TextStyle(
            inherit: false,
            color: Colors.black,
            fontFamily: "Kimberley",
            fontSize: 20,
          ),
        )
      ],
    );
  }
}
