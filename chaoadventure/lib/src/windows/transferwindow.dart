import 'package:chaoadventure/src/network/network.dart';
import 'package:flutter/material.dart';

import 'computertabs.dart';
import '../network/computers.dart';

class TransferWindowPopup<T> extends PopupRoute<T> {
  //late AnimationController _controller;

  @override
  bool get barrierDismissible => true;

  @override
  Color? get barrierColor => Colors.black.withAlpha(50);

  @override
  String? get barrierLabel => null;

  @override
  Duration get transitionDuration => const Duration(milliseconds: 300);

  @override
  bool get opaque => false;

  @override
  Curve get barrierCurve => Curves.easeInSine;

  final Network networkRef;

  //@override
  //Animation<double> get animation => CurvedAnimation(parent: super., curve: Curves.easeInSine);

  /*PageRouteBuilder<void> _getPageRoute() {
    return PageRouteBuilder(
      pageBuilder: buildPage,
      transitionDuration: ,
      transitionsBuilder: (context, animation, secondaryAnimation, child) {
        return SlideTransition(
          position: Tween<Offset>(
            begin: const Offset(0.0, 1.0),
            end: const Offset(0.0, 0.0),
          ).animate(animation),
        );
      },
    );
  }*/

  /*@override
  Widget buildTransitions(BuildContext context, Animation<double> animation, Animation<double> secondaryAnimation, Widget child) {
    // TODO: implement buildTransitions
    _controller = AnimationController(
      duration: const Duration(milliseconds: 900),
      vsync: Navigator.of(context).overlay!,
    );

    //return super.buildTransitions(context, animation, secondaryAnimation, child);
    return SlideTransition(
      position: Tween<Offset>(
        begin: const Offset(0.0, 0.0),
        end: const Offset(150.0, 300.0),
      ).animate(CurvedAnimation(
        parent: _controller,
        curve: Curves.easeInSine,
      )),
      child: child,
    );
  }*/

  TransferWindowPopup(this.networkRef);

  @override
  Widget buildPage(BuildContext context, Animation<double> animation, Animation<double> secondaryAnimation) {
    //_controller = AnimationController(vsync: , duration: Duration(),);
    return TransferWindow(networkRef: networkRef);
  }
}

class TransferWindow extends StatefulWidget {
  const TransferWindow({Key? key, required this.networkRef}) : super(key: key);

  final Network networkRef;

  @override
  State<TransferWindow> createState() => _TransferWindowState(networkRef);
}

class _TransferWindowState extends State<TransferWindow> {
  void setCurrentComputers(Computer newComputer) {
    setState(() {});
  }

  Network networkRef;
  bool tryingToConnect = false;

  _TransferWindowState(this.networkRef) {
    if (!networkRef.listening && !networkRef.connectedWithComputer) {
      networkRef.mainListen();
    }

    if (!networkRef.hasUpdateAvailableComputersFunc) {
      networkRef.assignTransferWindowFunction(setCurrentComputers);
    }
  }

  @override
  void dispose() {
    networkRef.disposeComputers();

    super.dispose();
  }

  Widget loadingCircle = const SizedBox(
    width: 64,
    height: 64,
    child: CircularProgressIndicator(
      color: Colors.white,
      strokeWidth: 4,
    ),
  );

  Future<void> connectToComputerCallback(Computer computer) async {
    tryingToConnect = true;
    networkRef.disposeComputers();
    networkRef.connectToComputer(computer);
    setState(() {});
  }

  @override
  Widget build(BuildContext context) {
    Widget content;
    if (tryingToConnect || networkRef.connectedWithComputer) {
      content = Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          const Padding(
            padding: EdgeInsets.only(bottom: 20),
            child: Text(
              "Connecting",
              style: TextStyle(
                inherit: false,
                fontSize: 30,
                fontFamily: 'Kimberley',
              ),
            ),
          ),
          loadingCircle
        ],
      );
    } else {
      content = selectionView();
    }

    return Center(
      child: UnconstrainedBox(
        child: Container(
          width: 280,
          height: 476,
          padding: EdgeInsets.symmetric(vertical: 10, horizontal: 10),
          decoration: BoxDecoration(
            border: Border.all(width: 3),
            borderRadius: BorderRadius.circular(30),
            color: Color(0x80005499),
          ),
          child: content,
        ),
      ),
    );
  }

  Widget selectionView() {
    List<Widget> computerTabs = <Widget>[];
    if (networkRef.availableComputers.isEmpty) {
      computerTabs.add(
        const Padding(
          padding: EdgeInsets.only(bottom: 20),
          child: Text(
            "Searching for\nComputers",
            textAlign: TextAlign.center,
            style: TextStyle(
              inherit: false,
              fontSize: 30,
              fontFamily: 'Kimberley',
            ),
          ),
        ),
      );
      computerTabs.add(loadingCircle);
    }
    for (int i = 0; i < networkRef.availableComputers.length; i++) {
      computerTabs.add(ComputerTab(
        computerInfo: networkRef.availableComputers[i],
        connectToComputer: connectToComputerCallback,
      ));
    }

    return Column(
      mainAxisAlignment: (networkRef.availableComputers.isEmpty) ? MainAxisAlignment.center : MainAxisAlignment.start,
      mainAxisSize: MainAxisSize.min,
      children: computerTabs,
    );
  }
}

//PageView.builder(
//reverse: true,

/*[
              DefaultTextStyle(
                style: TextStyle(fontFamily: "Kimberley", fontSize: 30, color: Colors.pink[700]),
                child: Text(
                  "onjah",
                ),
              )
            ],*/