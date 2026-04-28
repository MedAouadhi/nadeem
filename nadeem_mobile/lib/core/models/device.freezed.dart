// coverage:ignore-file
// GENERATED CODE - DO NOT MODIFY BY HAND
// ignore_for_file: type=lint
// ignore_for_file: unused_element, deprecated_member_use, deprecated_member_use_from_same_package, use_function_type_syntax_for_parameters, unnecessary_const, avoid_init_to_null, invalid_override_different_default_values_named, prefer_expression_function_bodies, annotate_overrides, invalid_annotation_target, unnecessary_question_mark

part of 'device.dart';

// **************************************************************************
// FreezedGenerator
// **************************************************************************

T _$identity<T>(T value) => value;

final _privateConstructorUsedError = UnsupportedError(
    'It seems like you constructed your class using `MyClass._()`. This constructor is only meant to be used by freezed and you are not supposed to need it nor use it.\nPlease check the documentation here for more information: https://github.com/rrousselGit/freezed#adding-getters-and-methods-to-our-models');

Device _$DeviceFromJson(Map<String, dynamic> json) {
  return _Device.fromJson(json);
}

/// @nodoc
mixin _$Device {
  @JsonKey(name: 'device_id')
  String get deviceId => throw _privateConstructorUsedError;
  @JsonKey(name: 'online_status')
  bool get onlineStatus => throw _privateConstructorUsedError;
  @JsonKey(name: 'bootstrapped')
  bool get bootstrapped => throw _privateConstructorUsedError;
  @JsonKey(name: 'name')
  String? get name => throw _privateConstructorUsedError;

  /// Serializes this Device to a JSON map.
  Map<String, dynamic> toJson() => throw _privateConstructorUsedError;

  /// Create a copy of Device
  /// with the given fields replaced by the non-null parameter values.
  @JsonKey(includeFromJson: false, includeToJson: false)
  $DeviceCopyWith<Device> get copyWith => throw _privateConstructorUsedError;
}

/// @nodoc
abstract class $DeviceCopyWith<$Res> {
  factory $DeviceCopyWith(Device value, $Res Function(Device) then) =
      _$DeviceCopyWithImpl<$Res, Device>;
  @useResult
  $Res call(
      {@JsonKey(name: 'device_id') String deviceId,
      @JsonKey(name: 'online_status') bool onlineStatus,
      @JsonKey(name: 'bootstrapped') bool bootstrapped,
      @JsonKey(name: 'name') String? name});
}

/// @nodoc
class _$DeviceCopyWithImpl<$Res, $Val extends Device>
    implements $DeviceCopyWith<$Res> {
  _$DeviceCopyWithImpl(this._value, this._then);

  // ignore: unused_field
  final $Val _value;
  // ignore: unused_field
  final $Res Function($Val) _then;

  /// Create a copy of Device
  /// with the given fields replaced by the non-null parameter values.
  @pragma('vm:prefer-inline')
  @override
  $Res call({
    Object? deviceId = null,
    Object? onlineStatus = null,
    Object? bootstrapped = null,
    Object? name = freezed,
  }) {
    return _then(_value.copyWith(
      deviceId: null == deviceId
          ? _value.deviceId
          : deviceId // ignore: cast_nullable_to_non_nullable
              as String,
      onlineStatus: null == onlineStatus
          ? _value.onlineStatus
          : onlineStatus // ignore: cast_nullable_to_non_nullable
              as bool,
      bootstrapped: null == bootstrapped
          ? _value.bootstrapped
          : bootstrapped // ignore: cast_nullable_to_non_nullable
              as bool,
      name: freezed == name
          ? _value.name
          : name // ignore: cast_nullable_to_non_nullable
              as String?,
    ) as $Val);
  }
}

/// @nodoc
abstract class _$$DeviceImplCopyWith<$Res> implements $DeviceCopyWith<$Res> {
  factory _$$DeviceImplCopyWith(
          _$DeviceImpl value, $Res Function(_$DeviceImpl) then) =
      __$$DeviceImplCopyWithImpl<$Res>;
  @override
  @useResult
  $Res call(
      {@JsonKey(name: 'device_id') String deviceId,
      @JsonKey(name: 'online_status') bool onlineStatus,
      @JsonKey(name: 'bootstrapped') bool bootstrapped,
      @JsonKey(name: 'name') String? name});
}

/// @nodoc
class __$$DeviceImplCopyWithImpl<$Res>
    extends _$DeviceCopyWithImpl<$Res, _$DeviceImpl>
    implements _$$DeviceImplCopyWith<$Res> {
  __$$DeviceImplCopyWithImpl(
      _$DeviceImpl _value, $Res Function(_$DeviceImpl) _then)
      : super(_value, _then);

  /// Create a copy of Device
  /// with the given fields replaced by the non-null parameter values.
  @pragma('vm:prefer-inline')
  @override
  $Res call({
    Object? deviceId = null,
    Object? onlineStatus = null,
    Object? bootstrapped = null,
    Object? name = freezed,
  }) {
    return _then(_$DeviceImpl(
      deviceId: null == deviceId
          ? _value.deviceId
          : deviceId // ignore: cast_nullable_to_non_nullable
              as String,
      onlineStatus: null == onlineStatus
          ? _value.onlineStatus
          : onlineStatus // ignore: cast_nullable_to_non_nullable
              as bool,
      bootstrapped: null == bootstrapped
          ? _value.bootstrapped
          : bootstrapped // ignore: cast_nullable_to_non_nullable
              as bool,
      name: freezed == name
          ? _value.name
          : name // ignore: cast_nullable_to_non_nullable
              as String?,
    ));
  }
}

/// @nodoc
@JsonSerializable()
class _$DeviceImpl implements _Device {
  const _$DeviceImpl(
      {@JsonKey(name: 'device_id') required this.deviceId,
      @JsonKey(name: 'online_status') required this.onlineStatus,
      @JsonKey(name: 'bootstrapped') this.bootstrapped = false,
      @JsonKey(name: 'name') this.name});

  factory _$DeviceImpl.fromJson(Map<String, dynamic> json) =>
      _$$DeviceImplFromJson(json);

  @override
  @JsonKey(name: 'device_id')
  final String deviceId;
  @override
  @JsonKey(name: 'online_status')
  final bool onlineStatus;
  @override
  @JsonKey(name: 'bootstrapped')
  final bool bootstrapped;
  @override
  @JsonKey(name: 'name')
  final String? name;

  @override
  String toString() {
    return 'Device(deviceId: $deviceId, onlineStatus: $onlineStatus, bootstrapped: $bootstrapped, name: $name)';
  }

  @override
  bool operator ==(Object other) {
    return identical(this, other) ||
        (other.runtimeType == runtimeType &&
            other is _$DeviceImpl &&
            (identical(other.deviceId, deviceId) ||
                other.deviceId == deviceId) &&
            (identical(other.onlineStatus, onlineStatus) ||
                other.onlineStatus == onlineStatus) &&
            (identical(other.bootstrapped, bootstrapped) ||
                other.bootstrapped == bootstrapped) &&
            (identical(other.name, name) || other.name == name));
  }

  @JsonKey(includeFromJson: false, includeToJson: false)
  @override
  int get hashCode =>
      Object.hash(runtimeType, deviceId, onlineStatus, bootstrapped, name);

  /// Create a copy of Device
  /// with the given fields replaced by the non-null parameter values.
  @JsonKey(includeFromJson: false, includeToJson: false)
  @override
  @pragma('vm:prefer-inline')
  _$$DeviceImplCopyWith<_$DeviceImpl> get copyWith =>
      __$$DeviceImplCopyWithImpl<_$DeviceImpl>(this, _$identity);

  @override
  Map<String, dynamic> toJson() {
    return _$$DeviceImplToJson(
      this,
    );
  }
}

abstract class _Device implements Device {
  const factory _Device(
      {@JsonKey(name: 'device_id') required final String deviceId,
      @JsonKey(name: 'online_status') required final bool onlineStatus,
      @JsonKey(name: 'bootstrapped') final bool bootstrapped,
      @JsonKey(name: 'name') final String? name}) = _$DeviceImpl;

  factory _Device.fromJson(Map<String, dynamic> json) = _$DeviceImpl.fromJson;

  @override
  @JsonKey(name: 'device_id')
  String get deviceId;
  @override
  @JsonKey(name: 'online_status')
  bool get onlineStatus;
  @override
  @JsonKey(name: 'bootstrapped')
  bool get bootstrapped;
  @override
  @JsonKey(name: 'name')
  String? get name;

  /// Create a copy of Device
  /// with the given fields replaced by the non-null parameter values.
  @override
  @JsonKey(includeFromJson: false, includeToJson: false)
  _$$DeviceImplCopyWith<_$DeviceImpl> get copyWith =>
      throw _privateConstructorUsedError;
}
