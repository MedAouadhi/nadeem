// coverage:ignore-file
// GENERATED CODE - DO NOT MODIFY BY HAND
// ignore_for_file: type=lint
// ignore_for_file: unused_element, deprecated_member_use, deprecated_member_use_from_same_package, use_function_type_syntax_for_parameters, unnecessary_const, avoid_init_to_null, invalid_override_different_default_values_named, prefer_expression_function_bodies, annotate_overrides, invalid_annotation_target, unnecessary_question_mark

part of 'provision_token.dart';

// **************************************************************************
// FreezedGenerator
// **************************************************************************

T _$identity<T>(T value) => value;

final _privateConstructorUsedError = UnsupportedError(
    'It seems like you constructed your class using `MyClass._()`. This constructor is only meant to be used by freezed and you are not supposed to need it nor use it.\nPlease check the documentation here for more information: https://github.com/rrousselGit/freezed#adding-getters-and-methods-to-our-models');

ProvisionToken _$ProvisionTokenFromJson(Map<String, dynamic> json) {
  return _ProvisionToken.fromJson(json);
}

/// @nodoc
mixin _$ProvisionToken {
  @JsonKey(name: 'provision_token')
  String get token => throw _privateConstructorUsedError;
  @JsonKey(name: 'expires_at')
  DateTime get expiresAt => throw _privateConstructorUsedError;

  /// Serializes this ProvisionToken to a JSON map.
  Map<String, dynamic> toJson() => throw _privateConstructorUsedError;

  /// Create a copy of ProvisionToken
  /// with the given fields replaced by the non-null parameter values.
  @JsonKey(includeFromJson: false, includeToJson: false)
  $ProvisionTokenCopyWith<ProvisionToken> get copyWith =>
      throw _privateConstructorUsedError;
}

/// @nodoc
abstract class $ProvisionTokenCopyWith<$Res> {
  factory $ProvisionTokenCopyWith(
          ProvisionToken value, $Res Function(ProvisionToken) then) =
      _$ProvisionTokenCopyWithImpl<$Res, ProvisionToken>;
  @useResult
  $Res call(
      {@JsonKey(name: 'provision_token') String token,
      @JsonKey(name: 'expires_at') DateTime expiresAt});
}

/// @nodoc
class _$ProvisionTokenCopyWithImpl<$Res, $Val extends ProvisionToken>
    implements $ProvisionTokenCopyWith<$Res> {
  _$ProvisionTokenCopyWithImpl(this._value, this._then);

  // ignore: unused_field
  final $Val _value;
  // ignore: unused_field
  final $Res Function($Val) _then;

  /// Create a copy of ProvisionToken
  /// with the given fields replaced by the non-null parameter values.
  @pragma('vm:prefer-inline')
  @override
  $Res call({
    Object? token = null,
    Object? expiresAt = null,
  }) {
    return _then(_value.copyWith(
      token: null == token
          ? _value.token
          : token // ignore: cast_nullable_to_non_nullable
              as String,
      expiresAt: null == expiresAt
          ? _value.expiresAt
          : expiresAt // ignore: cast_nullable_to_non_nullable
              as DateTime,
    ) as $Val);
  }
}

/// @nodoc
abstract class _$$ProvisionTokenImplCopyWith<$Res>
    implements $ProvisionTokenCopyWith<$Res> {
  factory _$$ProvisionTokenImplCopyWith(_$ProvisionTokenImpl value,
          $Res Function(_$ProvisionTokenImpl) then) =
      __$$ProvisionTokenImplCopyWithImpl<$Res>;
  @override
  @useResult
  $Res call(
      {@JsonKey(name: 'provision_token') String token,
      @JsonKey(name: 'expires_at') DateTime expiresAt});
}

/// @nodoc
class __$$ProvisionTokenImplCopyWithImpl<$Res>
    extends _$ProvisionTokenCopyWithImpl<$Res, _$ProvisionTokenImpl>
    implements _$$ProvisionTokenImplCopyWith<$Res> {
  __$$ProvisionTokenImplCopyWithImpl(
      _$ProvisionTokenImpl _value, $Res Function(_$ProvisionTokenImpl) _then)
      : super(_value, _then);

  /// Create a copy of ProvisionToken
  /// with the given fields replaced by the non-null parameter values.
  @pragma('vm:prefer-inline')
  @override
  $Res call({
    Object? token = null,
    Object? expiresAt = null,
  }) {
    return _then(_$ProvisionTokenImpl(
      token: null == token
          ? _value.token
          : token // ignore: cast_nullable_to_non_nullable
              as String,
      expiresAt: null == expiresAt
          ? _value.expiresAt
          : expiresAt // ignore: cast_nullable_to_non_nullable
              as DateTime,
    ));
  }
}

/// @nodoc
@JsonSerializable()
class _$ProvisionTokenImpl implements _ProvisionToken {
  const _$ProvisionTokenImpl(
      {@JsonKey(name: 'provision_token') required this.token,
      @JsonKey(name: 'expires_at') required this.expiresAt});

  factory _$ProvisionTokenImpl.fromJson(Map<String, dynamic> json) =>
      _$$ProvisionTokenImplFromJson(json);

  @override
  @JsonKey(name: 'provision_token')
  final String token;
  @override
  @JsonKey(name: 'expires_at')
  final DateTime expiresAt;

  @override
  String toString() {
    return 'ProvisionToken(token: $token, expiresAt: $expiresAt)';
  }

  @override
  bool operator ==(Object other) {
    return identical(this, other) ||
        (other.runtimeType == runtimeType &&
            other is _$ProvisionTokenImpl &&
            (identical(other.token, token) || other.token == token) &&
            (identical(other.expiresAt, expiresAt) ||
                other.expiresAt == expiresAt));
  }

  @JsonKey(includeFromJson: false, includeToJson: false)
  @override
  int get hashCode => Object.hash(runtimeType, token, expiresAt);

  /// Create a copy of ProvisionToken
  /// with the given fields replaced by the non-null parameter values.
  @JsonKey(includeFromJson: false, includeToJson: false)
  @override
  @pragma('vm:prefer-inline')
  _$$ProvisionTokenImplCopyWith<_$ProvisionTokenImpl> get copyWith =>
      __$$ProvisionTokenImplCopyWithImpl<_$ProvisionTokenImpl>(
          this, _$identity);

  @override
  Map<String, dynamic> toJson() {
    return _$$ProvisionTokenImplToJson(
      this,
    );
  }
}

abstract class _ProvisionToken implements ProvisionToken {
  const factory _ProvisionToken(
          {@JsonKey(name: 'provision_token') required final String token,
          @JsonKey(name: 'expires_at') required final DateTime expiresAt}) =
      _$ProvisionTokenImpl;

  factory _ProvisionToken.fromJson(Map<String, dynamic> json) =
      _$ProvisionTokenImpl.fromJson;

  @override
  @JsonKey(name: 'provision_token')
  String get token;
  @override
  @JsonKey(name: 'expires_at')
  DateTime get expiresAt;

  /// Create a copy of ProvisionToken
  /// with the given fields replaced by the non-null parameter values.
  @override
  @JsonKey(includeFromJson: false, includeToJson: false)
  _$$ProvisionTokenImplCopyWith<_$ProvisionTokenImpl> get copyWith =>
      throw _privateConstructorUsedError;
}
