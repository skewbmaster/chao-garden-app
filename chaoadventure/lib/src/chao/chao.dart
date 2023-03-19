import 'dart:math';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:vector_math/vector_math.dart';

import '../queue.dart';
import 'chaobehaviours.dart';

class Chao {
  Uint8List data;
  int initialHash;

  Vector2 position = Vector2.zero();
  String sprite = "assets/chaosit.png";
  Queue<ChaoBehaviour> behaviours = Queue();
  bool showingStats = false;
  bool showStatsUnder = false;

  Random randomiser = Random();

  Chao(this.data, this.initialHash) {
    double initX = max(randomiser.nextDouble() * 350, 10);
    double initY = max(randomiser.nextDouble() * 650, 40);

    position = Vector2(initX, initY);
  }

  void displayChaoStats() {
    showingStats = !showingStats;

    if (position.y < 360) {
      showStatsUnder = true;
    } else {
      showStatsUnder = false;
    }
  }

  void runBehaviours() {
    if (behaviours.getSize() < 2) {
      if (randomiser.nextBool()) {
        double randX = max(randomiser.nextDouble() * 330, 10);
        double randY = max(randomiser.nextDouble() * 650, 40);
        behaviours.push(
          MoveBehaviour(
            type: BehaviourType.moving,
            timer: 0.0,
            startLocation: position,
            goToLocation: Vector2(randX, randY),
          ),
        );
      } else {
        behaviours.push(
          ChaoBehaviour(type: BehaviourType.idle, timer: randomiser.nextInt(480) + 300),
        );
      }
    }

    ChaoBehaviour currentBehaviour = behaviours.getFirst() as ChaoBehaviour;

    switch (currentBehaviour.type) {
      case BehaviourType.idle:
        sprite = "assets/chaosit.png";
        currentBehaviour.timer--;
        if (currentBehaviour.timer < 1) {
          behaviours.pop();
        }
        return;

      case BehaviourType.moving:
        sprite = "assets/chaoidle.png";
        MoveBehaviour moving = currentBehaviour as MoveBehaviour;

        if (moving.timer == 0) {
          moving.updateStartPos(position);
        }

        currentBehaviour.timer += moving.timestep;

        if (moving.timer >= 1) {
          position = moving.goToLocation;
          behaviours.pop();
          return;
        }
        position = moving.startLocation + (moving.goToLocation - moving.startLocation) * currentBehaviour.timer;
    }
  }

  String getName() {
    StringBuffer nameBuilder = StringBuffer();

    for (int i = 0x12; i < 0x18; i++) {
      if (data[i] == 0) {
        break;
      } else if (data[i] == 95) {
        nameBuilder.writeCharCode(0x20);
      } else {
        nameBuilder.write(chaoCharDecode[data[i]]);
      }
    }

    return nameBuilder.toString() == "" ? "NoName" : nameBuilder.toString();
  }

  int getStatProgress(StatType stat) {
    return data[0x20 + (stat as int)];
  }

  Grade getGrade(StatType stat) {
    return Grade.S;
  }

  int getStatLevel(StatType stat) {
    return data[0x30 + (stat as int)];
  }

  int getStatPoints(StatType stat) {
    return (data[0x38 + (stat as int)] << 8) | data[0x39 + (stat as int)];
  }
}

enum Grade { E, D, C, B, A, S }

enum StatType { swim, fly, run, power, stamina, luck, intelligence }

Map<int, String> chaoCharDecode = {
  1: "!",
  2: "\"",
  3: "#",
  4: "\$",
  5: "%",
  6: "&",
  7: "\\'",
  8: "(",
  9: ")",
  10: "*",
  11: "+",
  12: ",",
  13: "-",
  14: ".",
  15: "/",
  16: "0",
  17: "1",
  18: "2",
  19: "3",
  20: "4",
  21: "5",
  22: "6",
  23: "7",
  24: "8",
  25: "9",
  26: ":",
  27: ";",
  28: "<",
  29: "=",
  30: ">",
  31: "?",
  32: "@",
  33: "A",
  34: "B",
  35: "C",
  36: "D",
  37: "E",
  38: "F",
  39: "G",
  40: "H",
  41: "I",
  42: "J",
  43: "K",
  44: "L",
  45: "M",
  46: "N",
  47: "O",
  48: "P",
  49: "Q",
  50: "R",
  51: "S",
  52: "T",
  53: "U",
  54: "V",
  55: "W",
  56: "X",
  57: "Y",
  58: "Z",
  59: "[",
  60: "¥",
  61: "]",
  62: "^",
  63: "_",
  64: "'",
  65: "a",
  66: "b",
  67: "c",
  68: "d",
  69: "e",
  70: "f",
  71: "g",
  72: "h",
  73: "i",
  74: "j",
  75: "k",
  76: "l",
  77: "m",
  78: "n",
  79: "o",
  80: "p",
  81: "q",
  82: "r",
  83: "s",
  84: "t",
  85: "u",
  86: "v",
  87: "w",
  88: "x",
  89: "y",
  90: "z",
  91: "{",
  92: "|",
  93: "}",
  94: "~",
  95: "\u0020",
  96: "À",
  97: "Á",
  98: "Â",
  99: "Ã",
  100: "Ä",
  101: "Å",
  102: "Æ",
  103: "Ç",
  104: "È",
  105: "É",
  106: "Ê",
  107: "Ë",
  108: "Ì",
  109: "Í",
  110: "Î",
  111: "Ï",
  112: "Ð",
  113: "Ñ",
  114: "Ò",
  115: "Ó",
  116: "Ô",
  117: "Õ",
  118: "Ö",
  119: "¿",
  120: "Ø",
  121: "Ù",
  122: "Ú",
  123: "Û",
  124: "Ü",
  125: "Ý",
  126: "Þ",
  127: "ß",
  128: "à",
  129: "á",
  130: "â",
  131: "ã",
  132: "ä",
  133: "å",
  134: "æ",
  135: "ç",
  136: "è",
  137: "é",
  138: "ê",
  139: "ë",
  140: "ì",
  141: "í",
  142: "î",
  143: "ï",
  144: "ð",
  145: "ñ",
  146: "ò",
  147: "ó",
  148: "ô",
  149: "õ",
  150: "ö",
  151: "¡",
  152: "ø",
  153: "ù",
  154: "ú",
  155: "û",
  156: "ü",
  157: "ý",
  158: "þ",
  159: "ÿ",
  160: "ァ",
  161: "ア",
  162: "ィ",
  163: "イ",
  164: "ゥ",
  165: "ウ",
  166: "エ",
  167: "エ",
  168: "ォ",
  169: "オ",
  170: "カ",
  171: "ガ",
  172: "キ",
  173: "ギ",
  174: "ク",
  175: "グ",
  176: "ケ",
  177: "ゲ",
  178: "コ",
  179: "ゴ",
  180: "サ",
  181: "ザ",
  182: "シ",
  183: "ジ",
  184: "ス",
  185: "ズ",
  186: "セ",
  187: "ゼ",
  188: "ソ",
  189: "ゾ",
  190: "タ",
  191: "ダ",
  192: "チ",
  193: "ヂ",
  194: "ツ",
  195: "ッ",
  196: "ヅ",
  197: "テ",
  198: "デ",
  199: "ト",
  200: "ド",
  201: "ナ",
  202: "ニ",
  203: "ヌ",
  204: "ネ",
  205: "ノ",
  206: "ハ",
  207: "バ",
  208: "パ",
  209: "ヒ",
  210: "ビ",
  211: "ピ",
  212: "フ",
  213: "ブ",
  214: "プ",
  215: "ヘ",
  216: "ベ",
  217: "ペ",
  218: "ホ",
  219: "ボ",
  220: "ポ",
  221: "マ",
  222: "ミ",
  223: "ム",
  224: "メ",
  225: "モ",
  226: "ャ",
  227: "ヤ",
  228: "ュ",
  229: "ユ",
  230: "ョ",
  231: "ヨ",
  232: "ラ",
  233: "リ",
  234: "ル",
  235: "レ",
  236: "ロ",
  237: "ヮ",
  238: "ワ",
  239: "ﾞ",
  240: "ﾟ",
  241: "ヲ",
  242: "ン",
  243: "。",
  244: "、",
  245: "〒",
  246: "・",
  247: "☆",
  248: "♀",
  249: "♂",
  250: "♪",
  251: "…",
  252: "「",
  253: "」",
  254: "ヴ",
  255: " "
};
