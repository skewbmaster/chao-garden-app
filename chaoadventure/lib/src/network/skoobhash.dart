import 'package:flutter/material.dart';

class SkoobHash {
  static const MAGICPRIME = 0x01000193;
  static const INITSEED = 0x23;

  static int skoobHashOnData(String data, int hashSeed) {
    for (int i = 0; i < data.length; i++) {
      hashSeed *= MAGICPRIME;
      hashSeed &= 0xFFFFFFFF;
      hashSeed ^= data.codeUnitAt(i);
    }
    return hashSeed;
  }
}
