import 'package:vector_math/vector_math.dart';

class ChaoBehaviour {
  final BehaviourType type;
  double timer = 0;

  ChaoBehaviour({required this.type, required this.timer});
}

class MoveBehaviour extends ChaoBehaviour {
  Vector2 startLocation;
  Vector2 goToLocation;

  late double timestep;

  MoveBehaviour({required super.type, required super.timer, required this.startLocation, required this.goToLocation}) {
    double distance = startLocation.distanceTo(goToLocation);
    timestep = 1 / (3 * distance);
  }

  void updateStartPos(Vector2 newpos) {
    newpos.copyInto(startLocation);
    timestep = 1 / (3 * startLocation.distanceTo(goToLocation));
  }
}

enum BehaviourType { idle, moving }
