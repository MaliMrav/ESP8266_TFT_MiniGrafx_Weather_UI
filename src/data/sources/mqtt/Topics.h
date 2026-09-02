#pragma once

// Topics defines all MQTT topic strings used by this firmware.
//
// Topics are organised by domain/location and measurement type using nested
// namespaces. This makes the mapping between external MQTT topics and the
// domain-owned sensor IDs explicit and easy to audit.
//
// These strings are referenced only from TopicMappings.cpp.
// Changing a topic requires a change here only.
//
// Solar topic names retain the current HA/Envoy naming convention. The
// firmware does not care whether the values originate from Envoy, HA,
// AppDaemon, or another upstream producer; MQTT is the transport boundary.

namespace Topics
{
    namespace Kitchen
    {
        namespace Temp
        {
            constexpr const char* value = "Kitchen_Enviro/Environment/Temperature";
            constexpr const char* min   = "ha/kitchen/temp/min";
            constexpr const char* max   = "ha/kitchen/temp/max";
            constexpr const char* trend = "ha/kitchen/temp/trend";
        }

        namespace Hum
        {
            constexpr const char* value = "Kitchen_Enviro/Environment/Humidity";
            constexpr const char* min   = "ha/kitchen/hum/min";
            constexpr const char* max   = "ha/kitchen/hum/max";
            constexpr const char* trend = "ha/kitchen/hum/trend";
        }

        namespace Pressure
        {
            constexpr const char* value = "Kitchen_Enviro/Environment/Pressure";
            constexpr const char* min   = "ha/kitchen/pressure/min";
            constexpr const char* max   = "ha/kitchen/pressure/max";
            constexpr const char* trend = "ha/kitchen/pressure/trend";
        }
    }

    namespace Pergola
    {
        namespace Temp
        {
            constexpr const char* value = "Pergola_Enviro/Environment/Temperature";
            constexpr const char* min   = "ha/pergola/temp/min";
            constexpr const char* max   = "ha/pergola/temp/max";
            constexpr const char* trend = "ha/pergola/temp/trend";
        }

        namespace Hum
        {
            constexpr const char* value = "Pergola_Enviro/Environment/Humidity";
            constexpr const char* min   = "ha/pergola/hum/min";
            constexpr const char* max   = "ha/pergola/hum/max";
            constexpr const char* trend = "ha/pergola/hum/trend";
        }

        namespace Pressure
        {
            constexpr const char* value = "Pergola_Enviro/Environment/Pressure";
            constexpr const char* min   = "ha/pergola/pressure/min";
            constexpr const char* max   = "ha/pergola/pressure/max";
            constexpr const char* trend = "ha/pergola/pressure/trend";
        }
    }
  // -------------------------------------------------------
  // Envoy solar — replace placeholders with your MQTT topics
  // -------------------------------------------------------
    namespace Solar
    {
        namespace Current
        {
            constexpr const char* production  = "ha/envoy/current_power_production";
            constexpr const char* consumption = "ha/envoy/current_power_consumption";
            constexpr const char* export_     = "ha/envoy/current_power_export";
            constexpr const char* battery     = "ha/envoy/current_power_battery";
        }

        namespace Today
        {
            constexpr const char* production  = "ha/envoy/energy_production_today";
            constexpr const char* consumption = "ha/envoy/energy_consumption_today";
            constexpr const char* export_     = "ha/envoy/energy_export_today";
            constexpr const char* battery     = "ha/envoy/energy_battery_today";
        }

        namespace Lifetime
        {
            constexpr const char* production  = "ha/envoy/lifetime_energy_production";
            constexpr const char* consumption = "ha/envoy/lifetime_energy_consumption";
            constexpr const char* export_     = "ha/envoy/lifetime_energy_export";
            constexpr const char* battery     = "ha/envoy/lifetime_energy_battery";
        }
    }
}
