import 'package:freezed_annotation/freezed_annotation.dart';

part 'device.freezed.dart';
part 'device.g.dart';

@freezed
class Device with _$Device {
  const factory Device({
    @JsonKey(name: 'device_id') required String deviceId,
    @JsonKey(name: 'online_status') required bool onlineStatus,
    @JsonKey(name: 'bootstrapped') @Default(false) bool bootstrapped,
    @JsonKey(name: 'name') String? name,
  }) = _Device;

  factory Device.fromJson(Map<String, dynamic> json) =>
      _$DeviceFromJson(json);
}
