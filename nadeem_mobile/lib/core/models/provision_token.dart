import 'package:freezed_annotation/freezed_annotation.dart';

part 'provision_token.freezed.dart';
part 'provision_token.g.dart';

@freezed
class ProvisionToken with _$ProvisionToken {
  const factory ProvisionToken({
    @JsonKey(name: 'provision_token') required String token,
    @JsonKey(name: 'expires_at') required DateTime expiresAt,
  }) = _ProvisionToken;

  factory ProvisionToken.fromJson(Map<String, dynamic> json) =>
      _$ProvisionTokenFromJson(json);
}
