import 'package:shared_preferences/shared_preferences.dart';

class SettingsPersistence {
  final Future<SharedPreferences> instanceFuture = SharedPreferences.getInstance();

  Future<bool> getMusicOn() async {
    final prefs = await instanceFuture;
    return prefs.getBool('musicOn') ?? true;
  }

  Future<bool> getRings({required bool defaultValue}) async {
    final prefs = await instanceFuture;
    return prefs.getBool('mute') ?? defaultValue;
  }
}
