import 'package:flutter/material.dart';

class TransferWindowPopup<T> extends PopupRoute<T> {
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
  Widget buildPage(BuildContext context, Animation<double> animation, Animation<double> secondaryAnimation) {
    return TransferWindow();
  }
}

class TransferWindow extends StatefulWidget {
  const TransferWindow({Key? key}) : super(key: key);

  @override
  State<TransferWindow> createState() => _TransferWindowState();
}

class _TransferWindowState extends State<TransferWindow> {
  @override
  Widget build(BuildContext context) {
    return Center(
      child: UnconstrainedBox(
        child: Container(
          padding: EdgeInsets.symmetric(vertical: 200, horizontal: 70),
          decoration: BoxDecoration(borderRadius: BorderRadius.circular(10), color: Colors.greenAccent),
          child: Column(
            children: [
              DefaultTextStyle(
                style: TextStyle(fontFamily: "Kimberley", fontSize: 30, color: Colors.pink[700]),
                child: Text(
                  "onjah",
                ),
              )
            ],
          ),
        ),
      ),
    );
  }
}

//PageView.builder(
          //reverse: true,