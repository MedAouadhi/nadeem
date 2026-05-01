// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'device.dart';

// **************************************************************************
// JsonSerializableGenerator
// **************************************************************************

_$DeviceImpl _$$DeviceImplFromJson(Map<String, dynamic> json) => _$DeviceImpl(
      deviceId: json['device_id'] as String,
      onlineStatus: json['online_status'] as bool,
      bootstrapped: json['bootstrapped'] as bool? ?? false,
      name: json['name'] as String?,
    );

Map<String, dynamic> _$$DeviceImplToJson(_$DeviceImpl instance) =>
    <String, dynamic>{
      'device_id': instance.deviceId,
      'online_status': instance.onlineStatus,
      'bootstrapped': instance.bootstrapped,
      'name': instance.name,
    };
