import 'package:flutter/material.dart';
import 'package:logging/logging.dart';
import 'package:flutter_launcher_icons/android.dart';
import 'package:keep_screen_on/keep_screen_on.dart';

import 'dart:io';
import 'dart:async';
import 'dart:ui';

import 'src/styles/palette.dart';
import 'src/network/network.dart';
import 'src/windows/transferwindow.dart';

Logger _log = Logger('main.dart');

int ringCount = 0;

void main() {
  Logger.root.onRecord.listen((record) {
    debugPrint('${record.level.name}: ${record.time}: '
        '${record.loggerName}: '
        '${record.message}');
  });

  WidgetsFlutterBinding.ensureInitialized();

  runApp(const MyApp());

  ringCount = 1750;

  _log.info(
      "Width: ${window.physicalSize.width} - Height: ${window.physicalSize.height} - PixelRatio: ${window.devicePixelRatio}");

  if (Platform.isAndroid || Platform.isIOS) KeepScreenOn.turnOn();
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    /*return Container(
      home: const MyHomePage(title: "Chao Adventure"),
      height: 100,
      alignment: Alignment.bottomCenter,
      color: const Color(0xAA0080D4),

    );*/

    return MaterialApp(
      title: 'Chao Adventure',
      theme: ThemeData.from(
        colorScheme: ColorScheme.fromSeed(
          seedColor: const Color(0xFF0030D0),
          background: const Color(0xFF0044DD),
        ),
        textTheme: const TextTheme(
          headlineMedium: TextStyle(fontFamily: "Kimberley"),
        ),
      ),
      home: const MyHomePage(title: 'Chao Adventure'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({
    super.key,
    required this.title,
  });

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int _counter = 0;

  bool transferWindowButtonPressed = false;

  String addressString = "No address";

  late Network network;

  _MyHomePageState() {
    network = Network(_log, updateReceivedIP);
  }

  void openTransferWindow() {
    /*if (!network.listening) {
      network.mainListen();
      _log.info("Network field initialised");
    }*/

    setState(() {
      Navigator.of(context).push(TransferWindowPopup<void>(network));
    });

    /*showDialog(
        context: context,
        builder: (_) => const AlertDialog(
              content: TransferWindow(),
            ));*/
  }

  void updateReceivedIP(String newIP) {
    setState(() {
      addressString = newIP;
    });
  }

  /*void _incrementCounter() {
    setState(() {
      _counter++;
    });
  }*/

  void pressRings() {
    _log.info("True rings");
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        toolbarHeight: 100,
        actions: const <Widget>[],
        title: Row(
          children: <Widget>[
            GestureDetector(
              onTap: () {
                setState(() {
                  ringCount += 10;
                });
              },
              child: Image.asset(
                "assets/spinningring.gif",
                height: 64,
                width: 64,
                isAntiAlias: true,
                fit: BoxFit.scaleDown,
              ),
            ),
            Padding(
              padding: const EdgeInsets.only(left: 10),
              child: Text(
                "${ringCount}",
                style: const TextStyle(
                  fontFamily: "Blippo",
                  fontSize: 32,
                ),
              ),
            ),
          ],
        ),
        /*Text(
          "Rings: ${ringCount}",
          style: const TextStyle(
            fontSize: 32,
            fontWeight: FontWeight.bold,
          ),
        ),*/
        //foregroundColor: const Color(0xFFFFC812),
        backgroundColor: const Color(0x00000000),
        elevation: 0,
      ),
      /*floatingActionButton: FloatingActionButton(
        onPressed: _incrementCounter,
        tooltip: 'Increment',
        child: const Icon(Icons.add),
      ),*/
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.end,
          children: <Widget>[
            /*const Text(
              'Fuck you:',
            ),
            Text(
              addressString,
              style: Theme.of(context).textTheme.headlineMedium,
            ),*/
            BottomButtons(openTransferWindow: openTransferWindow),
          ],
        ),
      ),
      backgroundColor: const Color(0xFF00FF00),
    );
  }

  @override
  void dispose() {
    network.close();
    super.dispose();
  }
}

/*
IconButton(
              onPressed: pressRings,
              icon: const Icon(
                Icons.circle_outlined,
                size: 40,
              ),
              color: const Color(0xFFFFC400),
            ),
*/

class BottomButtons extends StatelessWidget {
  const BottomButtons({super.key, required this.openTransferWindow});

  final String title = "BottomButtons";

  final Function openTransferWindow;

  @override
  Widget build(BuildContext context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.end,
      children: [
        Padding(
          padding: EdgeInsets.only(
              bottom: MediaQuery.of(context).size.height / 54.0, right: MediaQuery.of(context).size.height / 54.0),
          child: Ink(
            decoration: const ShapeDecoration(
                shape: /*CircleBorder()*/ RoundedRectangleBorder(
                  side: BorderSide(),
                  borderRadius: BorderRadius.all(Radius.circular(20)),
                ),
                color: Color(0xFF21DBF3)),
            child: IconButton(
              splashRadius: 28,
              highlightColor: const Color(0x00176CEB),
              splashColor: const Color(0x00176CEB),
              color: const Color(0xFF176CEB),
              iconSize: 70,
              onPressed: () {
                _log.info("pressed");
                openTransferWindow();
              },
              icon: const Icon(Icons.cable_sharp),
            ),
          ),
        ),
      ],
    );
  }
}

class TopAppBar extends StatelessWidget {
  const TopAppBar({super.key});

  void pressRings() {
    _log.info("True rings");
  }

  @override
  Widget build(BuildContext context) {
    return AppBar(
      // Here we take the value from the MyHomePage object that was created by
      // the App.build method, and use it to set our appbar title.
      actions: const <Widget>[],
      title: Row(
        children: <Widget>[
          IconButton(
            onPressed: pressRings,
            icon: const Icon(
              Icons.circle_outlined,
              size: 40,
            ),
            color: const Color(0xFFFFC400),
          ),
        ],
      ),
      /*Text(
          "Rings: ${ringCount}",
          style: const TextStyle(
            fontSize: 32,
            fontWeight: FontWeight.bold,
          ),
        ),*/
      foregroundColor: const Color(0xFFFFC812),
      backgroundColor: const Color(0x00000000),
      elevation: 0,
    );
  }
}
