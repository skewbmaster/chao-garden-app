import 'package:chaoadventure/src/network/network.dart';
import 'package:chaoadventure/src/styles/textstyles.dart';
import 'package:flutter/material.dart';

import '../chao/chao.dart';
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
  final List<Chao> chaoList;

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

  TransferWindowPopup(this.networkRef, this.chaoList);

  @override
  Widget buildPage(BuildContext context, Animation<double> animation, Animation<double> secondaryAnimation) {
    //_controller = AnimationController(vsync: , duration: Duration(),);
    return TransferWindow(networkRef: networkRef, chaoList: chaoList);
  }
}

class TransferWindow extends StatefulWidget {
  const TransferWindow({Key? key, required this.networkRef, required this.chaoList}) : super(key: key);

  final Network networkRef;
  final List<Chao> chaoList;

  @override
  State<TransferWindow> createState() => _TransferWindowState(networkRef, chaoList);
}

class _TransferWindowState extends State<TransferWindow> {
  void setStateHere() {
    setState(() {});
  }

  Network networkRef;
  bool tryingToConnect = false;

  List<Chao> chaoList;

  _TransferWindowState(this.networkRef, this.chaoList) {
    if (!networkRef.listening && !networkRef.connectedWithComputer) {
      networkRef.mainListen();
    }

    if (!networkRef.hasUpdateAvailableComputersFunc) {
      networkRef.assignTransferWindowFunction(setStateHere);
    }

    networkRef.transferWindowShown = true;
  }

  @override
  void dispose() {
    networkRef.disposeComputers();
    networkRef.transferWindowShown = false;

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
    if (networkRef.connectedWithComputer) {
      tryingToConnect = false;
      content = Column(
        mainAxisAlignment: MainAxisAlignment.start,
        children: makeChaoTabs(),
      );
    } else if (tryingToConnect) {
      content = Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          const Padding(
            padding: EdgeInsets.only(bottom: 20),
            child: Text(
              "Connecting",
              style: ChaoTextStyles.transferWindow,
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
          padding: const EdgeInsets.symmetric(vertical: 10, horizontal: 10),
          decoration: BoxDecoration(
            border: Border.all(width: 3),
            borderRadius: BorderRadius.circular(30),
            color: const Color(0x80005499),
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
            style: ChaoTextStyles.transferWindow,
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

  List<Widget> makeChaoTabs() {
    List<Widget> tabs = [];

    tabs.add(
      Text(
        "Connected to\n${networkRef.connectedComputer?.compName}\nTransfer Back:",
        style: ChaoTextStyles.transferWindow,
        textAlign: TextAlign.center,
      ),
    );

    for (int i = 0; i < chaoList.length; i++) {
      tabs.add(
        TextButton(
          onPressed: () => networkRef.requestToSendChao(chaoList[0]),
          child: Text(chaoList[i].getName()),
        ),
      );
    }

    return tabs;
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
