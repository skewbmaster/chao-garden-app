import 'dart:io';

enum GardenType {
  sadx,
  sa2;
}

class Computer {
  final InternetAddress address;
  String compName;
  GardenType gardenType;

  Computer(this.address, this.compName, this.gardenType);
}
