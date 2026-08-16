#pragma once

namespace WiFiSecrets {
  constexpr const char* SSID     = "ssid_placeholder";
  constexpr const char* PASSWORD = "password_placeholder";
}

namespace MqttSecrets {
  constexpr const char* SERVER   = "server_placeholder";
  constexpr const char* CLIENT   = "client_placeholder";
  constexpr const char* USERNAME = "username_placeholder";
  constexpr const char* PASSWORD = "password_placeholder";
}

namespace EnvoySecrets {
  // Local IP address of your Enphase Envoy gateway on the LAN.
  constexpr const char* HOST  = "192.168.x.x";
  // Long-lived API token from the Envoy local interface.
  // Obtain from: https://<envoy-ip>/auth/check_jwt
  constexpr const char* TOKEN = "your_envoy_api_token_here";
}

namespace OtaSecrets {
  constexpr const char* PASSWORD      = "ota_password_placeholder";
  constexpr const char* PASSWORD_HASH = "ota_password_hash_placeholder";
}