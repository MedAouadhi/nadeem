# devices/tests/test_tokens.py
from devices.tokens import generate_token, hash_token


def test_generate_token_is_url_safe_and_long():
    t = generate_token()
    assert len(t) >= 40
    assert all(c.isalnum() or c in "-_" for c in t)


def test_hash_is_deterministic_64_hex():
    h1 = hash_token("abc")
    h2 = hash_token("abc")
    assert h1 == h2
    assert len(h1) == 64
    assert h1 != hash_token("abd")
