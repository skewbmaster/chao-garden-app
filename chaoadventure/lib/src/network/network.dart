import 'package:flutter/material.dart';
import 'package:logging/logging.dart';

import 'dart:async';
import 'dart:io';

import 'computers.dart';

class Network {
  Logger _log;
  InternetAddress receivedIP = InternetAddress.anyIPv4;

  var availableComputers = <Computer>[];

  final Function(String) updateReceivedIP;

  Network(this._log, this.updateReceivedIP) {
    //_log = logger;
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

    bindedReceiver.then(runBindedReceiver);
  }

  void runBindedReceiver(RawDatagramSocket receiver) {
    _log.info("UDP Receiver was binded");
    _log.info("${receiver.address.address}:${receiver.port}");
    receiver.listen((RawSocketEvent e) {
      Datagram? datagram = receiver.receive();
      if (datagram == null) return;

      handleIncomingPacket(datagram);
    });
  }

  void handleIncomingPacket(Datagram packet) {
    String msg = String.fromCharCodes(packet.data).trim();
    _log.info("Datagram from ${packet.address.address}:${packet.port}: $msg");

    if (msg.substring(0, 10) == "CHAOGARDEN") {
      _log.info("Received a packet from a chaogarden");

      if (availableComputers.any((x) => x.address.address != packet.address.address)) {
        receivedIP = packet.address;
        updateReceivedIP(packet.address.address);
        availableComputers.add(Computer(packet.address, GardenType.sa2));
      }
    }
  }

  //Future<RawDatagramSocket> receiverFunc()
}
