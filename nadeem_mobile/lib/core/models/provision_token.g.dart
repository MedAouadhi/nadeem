// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'provision_token.dart';

// **************************************************************************
// JsonSerializableGenerator
// **************************************************************************

_$ProvisionTokenImpl _$$ProvisionTokenImplFromJson(Map<String, dynamic> json) =>
    _$ProvisionTokenImpl(
      token: json['provision_token'] as String,
      expiresAt: DateTime.parse(json['expires_at'] as String),
    );

Map<String, dynamic> _$$ProvisionTokenImplToJson(
        _$ProvisionTokenImpl instance) =>
    <String, dynamic>{
      'provision_token': instance.token,
      'expires_at': instance.expiresAt.toIso8601String(),
    };
