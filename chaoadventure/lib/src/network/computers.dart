import 'dart:io';

enum GardenType {
  sadx,
  sa2;
}

class Computer {
  final InternetAddress address;
  GardenType gardenType;

  Computer(this.address, this.gardenType);
}
