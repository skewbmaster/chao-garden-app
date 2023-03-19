import 'dart:convert';
import 'dart:ffi';
import 'dart:typed_data';

import 'package:chaoadventure/src/network/skoobhash.dart';
import 'package:flutter/material.dart';
import 'package:logging/logging.dart';

import 'dart:async';
import 'dart:io';

import '../chao/chao.dart';
import 'computers.dart';

class Network {
  Logger _log;
  InternetAddress receivedIP = InternetAddress.anyIPv4;

  List<Computer> availableComputers = <Computer>[];
  Computer? connectedComputer;

  late bool listening;
  late bool connectedWithComputer;
  late bool failureInComms;
  late bool wantToSend;
  late bool sentConfirmMessage;
  late bool transferWindowShown;
  late String newSendMessage;
  late Chao? chaoDataToSend;

  final Function(Chao) addInChao;
  final bool Function(int) checkChaoDuplicate;

  late void Function() updateTransferWindow;
  late bool hasUpdateAvailableComputersFunc;

  Network(this._log, this.addInChao, this.checkChaoDuplicate) {
    //_log = logger;
    listening = false;
    hasUpdateAvailableComputersFunc = false;
    connectedWithComputer = false;
    failureInComms = false;
    wantToSend = false;
    sentConfirmMessage = true;
    transferWindowShown = false;
    chaoDataToSend = null;
    newSendMessage = "CHAOADV KEEPALIVE\u0000";
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
      Computer newComputer = Computer(
        packet.address,
        msgSplit[2],
        gardenRequesting,
      );

      availableComputers.add(newComputer);

      _log.info("New computer detected called ${msgSplit[2]}, they are playing on ${msgSplit[1]}");

      if (hasUpdateAvailableComputersFunc) {
        updateTransferWindow();
      }
    }
  }

  Future<void> connectToComputer(Computer computer) async {
    Future<Socket> socket = Socket.connect(computer.address.address, 8889);
    _log.info("Trying to connect socket connection with ${computer.address.address}:8889 - ${computer.compName}");
    // _log.info("Big error with connecting ${error.toString()}");
    connectedComputer = computer;
    socket.then(runConnection).onError((error, stackTrace) => errorConnecting);
  }

  void runConnection(Socket socket) {
    _log.info("Connected to ${socket.remoteAddress}:${socket.remotePort}");

    connectedWithComputer = true;
    //connectedComputer = availableComputers.firstWhere((element) => element.address == socket.address, orElse: () => null);

    socket.listen(
      connectionDataHandler,
      onDone: () {
        _log.info("Done with connection");
        socket.close();
        connectedWithComputer = false;
        connectedComputer = null;
        chaoDataToSend = null;
        newSendMessage = "";
        if (transferWindowShown) {
          updateTransferWindow();
        }
      },
      onError: errorConnecting,
      cancelOnError: false,
    );

    socket.write("CHAOADV CONNECT\u0000");
    socket.write("CHAOADV KEEPALIVE\u0000");
    Timer.periodic(const Duration(milliseconds: 800), (timer) {
      try {
        if (wantToSend) {
          socket.write(newSendMessage);
          if (chaoDataToSend != null && newSendMessage.contains("SENDCHAO")) {
            socket.add(chaoDataToSend!.data);
            chaoDataToSend = null;
          }

          wantToSend = false;
          sentConfirmMessage = true;
          newSendMessage = "CHAOADV KEEPALIVE\u0000";
        } else {
          socket.write("CHAOADV KEEPALIVE\u0000");
        }

        if (failureInComms) {
          throw Error();
        }
      } catch (e) {
        _log.info("Error interacting with server: $e");
        timer.cancel();
        socket.close();
        connectedComputer = null;
        connectedWithComputer = false;
        failureInComms = false;
        if (transferWindowShown) {
          updateTransferWindow();
        }
      }
    });
  }

  void connectionDataHandler(Uint8List stream) {
    String data = String.fromCharCodes(stream).trim();

    _log.info(data);

    List<String> splitData = data.split(' ');

    if (splitData[0] != "CHAOGARDEN") {
      failureInComms = true;
    }

    if (splitData[1] == "ACCEPT") {
      _log.info("Computer accepted connection");
      if (transferWindowShown) {
        updateTransferWindow();
      }
    } else if (splitData[1] == "KEEPALIVE") {
      _log.info("Computer sent keepalive message");
    } else if (splitData[1] == "SENDCHAO") {
      int lastPartOffset = data.indexOf(' ', 21) + 1;
      _log.info(lastPartOffset);
      Uint8List chaodata = Uint8List.fromList(stream.getRange(lastPartOffset, stream.length).toList());
      int? hash = int.tryParse(splitData[2], radix: 16);
      int selfhash = SkoobHash.skoobHashOnData(chaodata, SkoobHash.INITSEED);

      if (checkChaoDuplicate(hash!)) {
        _log.info("Already sent this chao");
        newSendMessage = "CHAOADV CHAOBAD\u0000";
        return;
      }

      if (hash == selfhash) {
        _log.info("Successfully received chao data");
        Chao receivedChao = Chao(chaodata, selfhash);
        addInChao(receivedChao);
        newSendMessage = "CHAOADV CHAOGOOD\u0000";
        sentConfirmMessage = false;
      } else if (sentConfirmMessage) {
        newSendMessage = "CHAOADV CHAOBAD\u0000";
        _log.info("Selfhash: ${selfhash.toRadixString(16)}");
      }

      wantToSend = true;
      //_log.info(splitData[3].length);
      //_log.info("Phone hash: ${SkoobHash.skoobHashOnData(splitData[3], SkoobHash.INITSEED).toRadixString(16).toUpperCase()}");
    } else if (splitData[1] == "REQUESTACCEPT" && chaoDataToSend != null) {
      String hashOfData = SkoobHash.skoobHashOnData(chaoDataToSend!.data, SkoobHash.INITSEED).toRadixString(16).toUpperCase();
      newSendMessage = "CHAOADV SENDCHAO $hashOfData ";
      wantToSend = true;
      _log.info("Request to send accepted");
    }
  }

  void requestToSendChao(Chao chao) {
    chaoDataToSend = chao;
    newSendMessage = "CHAOADV REQTOSEND\u0000";
    wantToSend = true;
    _log.info("Sending chao ${chao.getName()}");
  }

  FutureOr<void> errorConnecting(Object error, StackTrace stackTrace) {
    _log.info("Big error with connecting ${error.toString()}");
  }

  void assignTransferWindowFunction(final void Function() updateFunc) {
    updateTransferWindow = updateFunc;
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