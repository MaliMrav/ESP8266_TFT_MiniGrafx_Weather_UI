#include "TopicMappings.h"

const TopicMapping topicMappings[] = {
    // ---------------------------------------------------------------------
    // Weather — Kitchen
    // ---------------------------------------------------------------------
    { Topics::Kitchen::Temp::value,     SENSOR_KITCHEN_TEMP,  TopicMapping::VALUE },
    { Topics::Kitchen::Temp::min,       SENSOR_KITCHEN_TEMP,  TopicMapping::MIN   },
    { Topics::Kitchen::Temp::max,       SENSOR_KITCHEN_TEMP,  TopicMapping::MAX   },
    { Topics::Kitchen::Temp::trend,     SENSOR_KITCHEN_TEMP,  TopicMapping::TREND },

    { Topics::Kitchen::Hum::value,      SENSOR_KITCHEN_HUM,   TopicMapping::VALUE },
    { Topics::Kitchen::Hum::min,        SENSOR_KITCHEN_HUM,   TopicMapping::MIN   },
    { Topics::Kitchen::Hum::max,        SENSOR_KITCHEN_HUM,   TopicMapping::MAX   },
    { Topics::Kitchen::Hum::trend,      SENSOR_KITCHEN_HUM,   TopicMapping::TREND },

    // ---------------------------------------------------------------------
    // Weather — Pergola
    // ---------------------------------------------------------------------
    { Topics::Pergola::Temp::value,     SENSOR_PERGOLA_TEMP,  TopicMapping::VALUE },
    { Topics::Pergola::Temp::min,       SENSOR_PERGOLA_TEMP,  TopicMapping::MIN   },
    { Topics::Pergola::Temp::max,       SENSOR_PERGOLA_TEMP,  TopicMapping::MAX   },
    { Topics::Pergola::Temp::trend,     SENSOR_PERGOLA_TEMP,  TopicMapping::TREND },

    { Topics::Pergola::Hum::value,      SENSOR_PERGOLA_HUM,   TopicMapping::VALUE },
    { Topics::Pergola::Hum::min,        SENSOR_PERGOLA_HUM,   TopicMapping::MIN   },
    { Topics::Pergola::Hum::max,        SENSOR_PERGOLA_HUM,   TopicMapping::MAX   },
    { Topics::Pergola::Hum::trend,      SENSOR_PERGOLA_HUM,   TopicMapping::TREND },

    { Topics::Pergola::Pressure::value, SENSOR_PRESSURE,      TopicMapping::VALUE },
    { Topics::Pergola::Pressure::min,   SENSOR_PRESSURE,      TopicMapping::MIN   },
    { Topics::Pergola::Pressure::max,   SENSOR_PRESSURE,      TopicMapping::MAX   },
    { Topics::Pergola::Pressure::trend, SENSOR_PRESSURE,      TopicMapping::TREND },

    // ---------------------------------------------------------------------
    // Solar — current power
    // ---------------------------------------------------------------------
    { Topics::Solar::Power::production,  SENSOR_SOLAR_POWER_NOW,       TopicMapping::VALUE },
    { Topics::Solar::Power::consumption, SENSOR_CONSUMPTION_POWER_NOW, TopicMapping::VALUE },
    { Topics::Solar::Power::export_,     SENSOR_EXPORT_POWER_NOW,      TopicMapping::VALUE },
    { Topics::Solar::Power::battery,     SENSOR_BATTERY_POWER_NOW,     TopicMapping::VALUE },

    // ---------------------------------------------------------------------
    // Solar — today's accumulated energy
    // ---------------------------------------------------------------------
    { Topics::Solar::Today::production,  SENSOR_SOLAR_ENERGY_TODAY,       TopicMapping::VALUE },
    { Topics::Solar::Today::consumption, SENSOR_CONSUMPTION_ENERGY_TODAY, TopicMapping::VALUE },
    { Topics::Solar::Today::export_,     SENSOR_EXPORT_ENERGY_TODAY,      TopicMapping::VALUE },
    { Topics::Solar::Today::battery,     SENSOR_BATTERY_ENERGY_TODAY,     TopicMapping::VALUE },
};

const uint8_t TOPIC_COUNT =
    sizeof(topicMappings) / sizeof(topicMappings[0]);
