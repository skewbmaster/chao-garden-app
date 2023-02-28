import 'package:flutter/material.dart';
import 'package:logging/logging.dart';
import 'package:flutter_launcher_icons/android.dart';
import 'package:keep_screen_on/keep_screen_on.dart';

import 'dart:io';
import 'dart:async';
import 'dart:ui';

import 'src/styles/palette.dart';
import 'src/network/network.dart';

Logger _log = Logger('main.dart');

int ringCount = 0;

late Network network;

void main() {
  Logger.root.onRecord.listen((record) {
    debugPrint('${record.level.name}: ${record.time}: '
        '${record.loggerName}: '
        '${record.message}');
  });

  WidgetsFlutterBinding.ensureInitialized();

  runApp(const MyApp());

  ringCount = 400;

  _log.info(
      "Width: ${window.physicalSize.width} - Height: ${window.physicalSize.height} - PixelRatio: ${window.devicePixelRatio}");

  if (Platform.isAndroid || Platform.isIOS) KeepScreenOn.turnOn();
}

void pressRings() {
  _log.info("True rings");
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
      )),
      home: const MyHomePage(title: 'Chao Adventure'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int _counter = 0;

  String addressString = "No address";

  void updateReceivedIP(String newIP) {
    setState(() {
      addressString = newIP;
    });
  }

  void _incrementCounter() {
    setState(() {
      // This call to setState tells the Flutter framework that something has
      // changed in this State, which causes it to rerun the build method below
      // so that the display can reflect the updated values. If we changed
      // _counter without calling setState(), then the build method would not be
      // called again, and so nothing would appear to happen.
      _counter++;
    });
  }

  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        actions: const <Widget>[],
        title: Row(
          children: const <Widget>[
            IconButton(
              onPressed: pressRings,
              icon: Icon(
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
      ),

      /*floatingActionButton: FloatingActionButton(
        onPressed: _incrementCounter,
        tooltip: 'Increment',
        child: const Icon(Icons.add),
      ),*/
      body: Center(
        // Center is a layout widget. It takes a single child and positions it
        // in the middle of the parent.
        child: Column(
          // Column is also a layout widget. It takes a list of children and
          // arranges them vertically. By default, it sizes itself to fit its
          // children horizontally, and tries to be as tall as its parent.
          //
          // Invoke "debug painting" (press "p" in the console, choose the
          // "Toggle Debug Paint" action from the Flutter Inspector in Android
          // Studio, or the "Toggle Debug Paint" command in Visual Studio Code)
          // to see the wireframe for each widget.
          //
          // Column has various properties to control how it sizes itself and
          // how it positions its children. Here we use mainAxisAlignment to
          // center the children vertically; the main axis here is the vertical
          // axis because Columns are vertical (the cross axis would be
          // horizontal).
          mainAxisAlignment: MainAxisAlignment.end,
          children: <Widget>[
            const Text(
              'Fuck you:',
            ),
            Text(
              '$addressString${MediaQuery.of(context).size.height}',
              style: Theme.of(context).textTheme.headlineMedium,
            ),
            Container(
              alignment: Alignment.bottomCenter,
              height: 90,
              width: 360,
              color: const Color(0xFFFFFFFF),
              child: const BottomButtons(),
            ),
          ],
        ),
      ), // This trailing comma makes auto-formatting nicer for build methods.
      backgroundColor: const Color(0xFF00FF00),
    );
  }

  @override
  void dispose() {
    network.close();
    super.dispose();
  }
}

class BottomButtons extends StatelessWidget {
  const BottomButtons({super.key});

  final String title = "BottomButtons";

  @override
  Widget build(BuildContext context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.end,
      children: [
        Ink(
          child: IconButton(
            splashRadius: 40,
            splashColor: Colors.blue,
            color: Color(0xFFEB17C7),
            iconSize: 80,
            onPressed: () {
              _log.info("pressed");
            },
            icon: const Icon(Icons.cable_rounded),
          ),
          decoration: const ShapeDecoration(shape: CircleBorder(), color: Colors.blue),
        ),
      ],
    );
  }
}
