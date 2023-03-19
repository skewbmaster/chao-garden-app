import 'package:flutter/foundation.dart';

import 'persistence/settings_persistence.dart';

class SettingsController {
  final SettingsPersistence _persistence;

  ValueNotifier<bool> muted = ValueNotifier(false);

  SettingsController({required SettingsPersistence persistence}) : _persistence = persistence;
}
