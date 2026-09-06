#include "TopicMappings.h"

const TopicMapping topicMappings[] = {

    // -------------------------------------------------------------------------
    // Weather — Kitchen
    // -------------------------------------------------------------------------

    {
        Topics::Kitchen::Temp::value,
        WeatherObservations::KITCHEN_TEMPERATURE,
        TopicMapping::VALUE
    },

    {
        Topics::Kitchen::Temp::min,
        WeatherObservations::KITCHEN_TEMPERATURE,
        TopicMapping::MIN
    },

    {
        Topics::Kitchen::Temp::max,
        WeatherObservations::KITCHEN_TEMPERATURE,
        TopicMapping::MAX
    },

    {
        Topics::Kitchen::Temp::trend,
        WeatherObservations::KITCHEN_TEMPERATURE,
        TopicMapping::TREND
    },

    {
        Topics::Kitchen::Hum::value,
        WeatherObservations::KITCHEN_HUMIDITY,
        TopicMapping::VALUE
    },

    {
        Topics::Kitchen::Hum::min,
        WeatherObservations::KITCHEN_HUMIDITY,
        TopicMapping::MIN
    },

    {
        Topics::Kitchen::Hum::max,
        WeatherObservations::KITCHEN_HUMIDITY,
        TopicMapping::MAX
    },

    {
        Topics::Kitchen::Hum::trend,
        WeatherObservations::KITCHEN_HUMIDITY,
        TopicMapping::TREND
    },

    // -------------------------------------------------------------------------
    // Weather — Pergola
    // -------------------------------------------------------------------------

    {
        Topics::Pergola::Temp::value,
        WeatherObservations::PERGOLA_TEMPERATURE,
        TopicMapping::VALUE
    },

    {
        Topics::Pergola::Temp::min,
        WeatherObservations::PERGOLA_TEMPERATURE,
        TopicMapping::MIN
    },

    {
        Topics::Pergola::Temp::max,
        WeatherObservations::PERGOLA_TEMPERATURE,
        TopicMapping::MAX
    },

    {
        Topics::Pergola::Temp::trend,
        WeatherObservations::PERGOLA_TEMPERATURE,
        TopicMapping::TREND
    },

    {
        Topics::Pergola::Hum::value,
        WeatherObservations::PERGOLA_HUMIDITY,
        TopicMapping::VALUE
    },

    {
        Topics::Pergola::Hum::min,
        WeatherObservations::PERGOLA_HUMIDITY,
        TopicMapping::MIN
    },

    {
        Topics::Pergola::Hum::max,
        WeatherObservations::PERGOLA_HUMIDITY,
        TopicMapping::MAX
    },

    {
        Topics::Pergola::Hum::trend,
        WeatherObservations::PERGOLA_HUMIDITY,
        TopicMapping::TREND
    },

    {
        Topics::Pergola::Pressure::value,
        WeatherObservations::PRESSURE,
        TopicMapping::VALUE
    },

    {
        Topics::Pergola::Pressure::min,
        WeatherObservations::PRESSURE,
        TopicMapping::MIN
    },

    {
        Topics::Pergola::Pressure::max,
        WeatherObservations::PRESSURE,
        TopicMapping::MAX
    },

    {
        Topics::Pergola::Pressure::trend,
        WeatherObservations::PRESSURE,
        TopicMapping::TREND
    },

    // -------------------------------------------------------------------------
    // Solar — current power
    // -------------------------------------------------------------------------

    {
        Topics::Solar::Current::production,
        EnergyObservations::CURRENT_POWER_PRODUCTION,
        TopicMapping::VALUE
    },

    {
        Topics::Solar::Current::consumption,
        EnergyObservations::CURRENT_POWER_CONSUMPTION,
        TopicMapping::VALUE
    },

    // -------------------------------------------------------------------------
    // Solar — today's accumulated energy
    // -------------------------------------------------------------------------

    {
        Topics::Solar::Today::production,
        EnergyObservations::ENERGY_PRODUCTION_TODAY,
        TopicMapping::VALUE
    },

    {
        Topics::Solar::Today::consumption,
        EnergyObservations::ENERGY_CONSUMPTION_TODAY,
        TopicMapping::VALUE
    }
    /*
    // -------------------------------------------------------------------------
    // Solar — Lifetime's accumulated energy
    // -------------------------------------------------------------------------

    {
        Topics::Solar::Lifetime::production,
        EnergyObservations::ENERGY_PRODUCTION_LIFETIME,
        TopicMapping::VALUE
    },

    {
        Topics::Solar::Lifetime::consumption,
        EnergyObservations::ENERGY_CONSUMPTION_LIFETIME,
        TopicMapping::VALUE
    }
    */
};

const uint8_t TOPIC_COUNT =
    sizeof(topicMappings) / sizeof(topicMappings[0]);