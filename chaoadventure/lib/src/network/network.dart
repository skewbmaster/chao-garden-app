import 'dart:convert';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:logging/logging.dart';

import 'dart:async';
import 'dart:io';

import 'computers.dart';

class Network {
  Logger _log;
  InternetAddress receivedIP = InternetAddress.anyIPv4;

  List<Computer> availableComputers = <Computer>[];

  late bool listening;
  late bool connectedWithComputer;

  final Function(String) updateReceivedIP;

  late void Function(Computer newComputer) updateAvailableComputers;
  late bool hasUpdateAvailableComputersFunc;

  Network(this._log, this.updateReceivedIP) {
    //_log = logger;
    listening = false;
    hasUpdateAvailableComputersFunc = false;
    connectedWithComputer = false;
  }

  void close() {}

  String getAddressString() {
    if (receivedIP != InternetAddress.anyIPv4) {
      return receivedIP.address;
    }
    return "No address";
  }

  Future<void> mainListen() async {
    Future<RawDatagramSocket> bindedReceiver = RawDatagramSocket.bind(InternetAddress.anyIPv4, 8888);

    listening = true;
    connectedWithComputer = false;

    bindedReceiver.then(runBindedReceiver);
  }

  void runBindedReceiver(RawDatagramSocket receiver) {
    _log.info("UDP Receiver was binded");
    _log.info("${receiver.address.address}:${receiver.port}");

    receiver.listen(
      (RawSocketEvent e) {
        if (!listening) {
          receiver.close();
          return;
        }

        Datagram? datagram = receiver.receive();
        if (datagram == null) return;

        handleIncomingPacket(datagram);
      },
      onDone: () {
        _log.info("Finished receiving");
        receiver.close();
      },
    );
  }

  void handleIncomingPacket(Datagram packet) {
    String msg = String.fromCharCodes(packet.data).trim();
    _log.info("Datagram from ${packet.address.address}:${packet.port}: $msg");

    List<String> msgSplit = msg.split(' ');

    if (msgSplit[0] != "CHAOGARDEN") {
      return;
    }

    _log.info("Received a packet from a chaogarden");

    GardenType gardenRequesting = msgSplit[1] == "SADX" ? GardenType.sadx : GardenType.sa2;

    if (!availableComputers.any((x) => (x.address.address == packet.address.address && x.gardenType == gardenRequesting)) ||
        availableComputers.isEmpty) {
      updateReceivedIP(msgSplit[2]);
      Computer newComputer = Computer(
        packet.address,
        msgSplit[2],
        gardenRequesting,
      );

      availableComputers.add(newComputer);

      _log.info("New computer detected called ${msgSplit[2]}, they are playing on ${msgSplit[1]}");

      if (hasUpdateAvailableComputersFunc) {
        updateAvailableComputers(newComputer);
      }
    }
  }

  Future<void> connectToComputer(Computer computer) async {
    Future<Socket> socket = Socket.connect(computer.address.address, 8889);
    _log.info("Trying to connect socket connection with ${computer.address.address}:8889 - ${computer.compName}");
    // _log.info("Big error with connecting ${error.toString()}");
    socket.then(runConnection).onError((error, stackTrace) => errorConnecting);
  }

  void runConnection(Socket socket) {
    _log.info("Connected to ${socket.remoteAddress}:${socket.remotePort}");

    connectedWithComputer = true;

    socket.listen(
      connectionDataHandler,
      onDone: () {
        _log.info("Done with connection");
        socket.close();
        connectedWithComputer = false;
      },
      onError: errorConnecting,
      cancelOnError: false,
    );

    socket.write("CHAOADV CONNECT\u0000");
    socket.write("CHAOADV KEEPALIVE\u0000");
    Timer.periodic(const Duration(milliseconds: 800), (timer) {
      try {
        socket.write("CHAOADV KEEPALIVE\u0000");
      } catch (e) {
        _log.info("Error sending keepalive message to server: $e");
        timer.cancel();
        socket.close();
        connectedWithComputer = false;
      }
    });
  }

  void connectionDataHandler(Uint8List stream) {
    String data = String.fromCharCodes(stream).trim();

    _log.info(data);

    if (data == "CHAOGARDEN ACCEPT") {}
  }

  FutureOr<void> errorConnecting(Object error, StackTrace stackTrace) {
    _log.info("Big error with connecting ${error.toString()}");
  }

  void assignTransferWindowFunction(final void Function(Computer) updateFunc) {
    updateAvailableComputers = updateFunc;
    hasUpdateAvailableComputersFunc = true;
  }

  void disposeComputers() {
    availableComputers.clear();
    hasUpdateAvailableComputersFunc = false;
    listening = false;
  }

  //Future<RawDatagramSocket> receiverFunc()
}

/*class ServerKeepalive extends StreamSubscription {
  @override
  Future asFuture<E>([E? futureValue]) {
    // TODO: implement asFuture
    throw UnimplementedError();
  }

  @override
  Future<void> cancel() {
    // TODO: implement cancel
    throw UnimplementedError();
  }

  @override
  // TODO: implement isPaused
  bool get isPaused => throw UnimplementedError();

  @override
  void onData(void Function(dynamic data)? handleData) {
    // TODO: implement onData
  }

  @override
  void onDone(void Function()? handleDone) {
    // TODO: implement onDone
  }

  @override
  void onError(Function? handleError) {
    // TODO: implement onError
  }

  @override
  void pause([Future<void>? resumeSignal]) {
    // TODO: implement pause
  }

  @override
  void resume() {
    // TODO: implement resume
  }

}*/