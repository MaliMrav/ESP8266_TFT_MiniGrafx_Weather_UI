"""
Telemetry application composer.

Reads the application composition from telemetry.yaml and generates
the runtime composition required by the Telemetry framework.

The composer runs as a PlatformIO PRE extra script.

Input:

    telemetry.yaml

Output:

    .pio/build/<environment>/generated/telemetry/
        TelemetryComposition.h
        TelemetryComposition.cpp
        TelemetryMqttMappings.h
        TelemetryMqttMappings.cpp

The generated files are build artifacts. They are deliberately kept out
of src/ because they are derived from application composition rather than
being framework source.

Current supported observation types:

    power
    energy

Current supported source types:

    mqtt
"""

from __future__ import annotations

from pathlib import Path
import re
import sys


Import("env")


# PlatformIO runs PRE scripts again for integration/IDE discovery.
# Do not generate files or install dependencies during that pass.
if env.IsIntegrationDump():
    Return()


# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

try:
    import yaml
except ImportError:
    print("[Telemetry Composer] Installing PyYAML...")

    result = env.Execute(
        "$PYTHONEXE -m pip install --disable-pip-version-check pyyaml"
    )

    if result != 0:
        raise RuntimeError(
            "Telemetry Composer could not install PyYAML"
        )

    import yaml


# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------

PROJECT_DIR = Path(env["PROJECT_DIR"])
BUILD_DIR = Path(env.subst("$BUILD_DIR"))

COMPOSITION_FILE = PROJECT_DIR / "telemetry.yaml"
GENERATED_DIR = BUILD_DIR / "generated" / "telemetry"


# ---------------------------------------------------------------------------
# Errors
# ---------------------------------------------------------------------------

class CompositionError(Exception):
    """Raised when telemetry.yaml contains an invalid composition."""


def error(message: str) -> None:
    raise CompositionError(
        f"[Telemetry Composer] {message}"
    )


# ---------------------------------------------------------------------------
# Validation helpers
# ---------------------------------------------------------------------------

OBSERVATION_TYPES = {
    "power": {
        "sensor_type": "ENERGY_W",
        "required_unit": "W",
    },
    "energy": {
        "sensor_type": "ENERGY_WH",
        "required_unit": "Wh",
    },
}


SOURCE_TYPES = {
    "mqtt",
}


IDENTIFIER_PATTERN = re.compile(
    r"^[A-Za-z_][A-Za-z0-9_]*$"
)


def require_mapping(
    value,
    context: str,
) -> dict:
    if not isinstance(value, dict):
        error(
            f"{context} must be a mapping"
        )

    return value


def require_string(
    value,
    context: str,
) -> str:
    if not isinstance(value, str) or not value.strip():
        error(
            f"{context} must be a non-empty string"
        )

    return value.strip()


def require_identifier(
    value,
    context: str,
) -> str:
    value = require_string(
        value,
        context,
    )

    if not IDENTIFIER_PATTERN.match(value):
        error(
            f"{context} '{value}' is not a valid "
            "composition identifier"
        )

    return value


def humanize_identifier(
    identifier: str,
) -> str:
    """
    Produce a readable fallback label from the composition name.

    Example:

        current_power_production
            ->
        Current Power Production
    """

    words = identifier.replace(
        "-",
        "_",
    ).split("_")

    return " ".join(
        word.capitalize()
        for word in words
        if word
    )


# ---------------------------------------------------------------------------
# Composition model
# ---------------------------------------------------------------------------

class Observation:
    def __init__(
        self,
        name: str,
        key: str,
        observation_type: str,
        unit: str,
        source_type: str,
        source_topic: str,
    ):
        self.name = name
        self.key = key
        self.observation_type = observation_type
        self.unit = unit
        self.source_type = source_type
        self.source_topic = source_topic

    @property
    def sensor_type(self) -> str:
        return OBSERVATION_TYPES[
            self.observation_type
        ]["sensor_type"]

    @property
    def label(self) -> str:
        return humanize_identifier(
            self.name
        )


# ---------------------------------------------------------------------------
# YAML loading
# ---------------------------------------------------------------------------

def load_composition() -> dict:
    if not COMPOSITION_FILE.exists():
        error(
            f"composition file not found: "
            f"{COMPOSITION_FILE}"
        )

    try:
        with COMPOSITION_FILE.open(
            "r",
            encoding="utf-8",
        ) as stream:
            document = yaml.safe_load(stream)

    except yaml.YAMLError as exc:
        error(
            f"invalid YAML in telemetry.yaml:\n{exc}"
        )

    if document is None:
        error(
            "telemetry.yaml is empty"
        )

    return require_mapping(
        document,
        "telemetry.yaml",
    )


# ---------------------------------------------------------------------------
# Observation parsing
# ---------------------------------------------------------------------------

def parse_observations(
    document: dict,
) -> list[Observation]:
    if "observations" not in document:
        error(
            "missing top-level 'observations' section"
        )

    definitions = require_mapping(
        document["observations"],
        "observations",
    )

    if not definitions:
        error(
            "observations must contain at least one observation"
        )

    observations: list[Observation] = []

    names: set[str] = set()
    keys: set[str] = set()
    mqtt_topics: set[str] = set()

    for raw_name, raw_definition in definitions.items():

        name = require_identifier(
            raw_name,
            "observation name",
        )

        if name in names:
            error(
                f"duplicate observation name '{name}'"
            )

        names.add(name)

        definition = require_mapping(
            raw_definition,
            f"observation '{name}'",
        )

        key = require_string(
            definition.get("key"),
            f"observation '{name}'.key",
        )

        if key in keys:
            error(
                f"duplicate ObservationKey '{key}'"
            )

        keys.add(key)

        observation_type = require_string(
            definition.get("type"),
            f"observation '{name}'.type",
        )

        if observation_type not in OBSERVATION_TYPES:
            supported = ", ".join(
                sorted(OBSERVATION_TYPES)
            )

            error(
                f"observation '{name}' has unsupported "
                f"type '{observation_type}'. "
                f"Supported types: {supported}"
            )

        unit = require_string(
            definition.get("unit"),
            f"observation '{name}'.unit",
        )

        expected_unit = OBSERVATION_TYPES[
            observation_type
        ]["required_unit"]

        if unit != expected_unit:
            error(
                f"observation '{name}' declares unit "
                f"'{unit}', but type '{observation_type}' "
                f"requires canonical unit '{expected_unit}'"
            )

        source = require_mapping(
            definition.get("source"),
            f"observation '{name}'.source",
        )

        source_type = require_string(
            source.get("type"),
            f"observation '{name}'.source.type",
        )

        if source_type not in SOURCE_TYPES:
            supported = ", ".join(
                sorted(SOURCE_TYPES)
            )

            error(
                f"observation '{name}' has unsupported "
                f"source type '{source_type}'. "
                f"Supported source types: {supported}"
            )

        source_topic = require_string(
            source.get("topic"),
            f"observation '{name}'.source.topic",
        )

        if source_type == "mqtt":

            if source_topic in mqtt_topics:
                error(
                    f"duplicate MQTT topic "
                    f"'{source_topic}'"
                )

            mqtt_topics.add(source_topic)

        observations.append(
            Observation(
                name=name,
                key=key,
                observation_type=observation_type,
                unit=unit,
                source_type=source_type,
                source_topic=source_topic,
            )
        )

    return observations


# ---------------------------------------------------------------------------
# C++ escaping
# ---------------------------------------------------------------------------

def cpp_string(value: str) -> str:
    return (
        value
        .replace("\\", "\\\\")
        .replace('"', '\\"')
        .replace("\n", "\\n")
        .replace("\r", "\\r")
    )


# ---------------------------------------------------------------------------
# Header generation
# ---------------------------------------------------------------------------

def generate_composition_header() -> str:
    return """\
#pragma once

// -----------------------------------------------------------------------------
// GENERATED FILE — DO NOT EDIT
//
// Generated by tools/telemetry_compose.py from telemetry.yaml.
//
// This file is build output, not application source.
// -----------------------------------------------------------------------------

namespace TelemetryComposition
{
    // Register all observations declared by telemetry.yaml.
    //
    // Returns false when the runtime observation composition cannot be
    // created within the available registry/repository capacity.
    bool registerObservations();
}
"""


# ---------------------------------------------------------------------------
# Composition implementation generation
# ---------------------------------------------------------------------------

def generate_composition_cpp(
    observations: list[Observation],
) -> str:

    lines: list[str] = []

    lines.extend(
        [
            '#include "TelemetryComposition.h"',
            "",
            '#include "../../../src/data/ObservationRegistry.h"',
            '#include "../../../src/models/SensorRepository.h"',
            '#include "../../../src/models/SensorTile.h"',
            "",
            "namespace",
            "{",
            "",
            "    bool registerObservation(",
            "        const ObservationKey& key,",
            "        const SensorTile& tile)",
            "    {",
            "        const ObservationHandle handle =",
            "            ObservationRegistry::registerObservation(key);",
            "",
            "        if (!handle.isValid())",
            "        {",
            "            return false;",
            "        }",
            "",
            "        return SensorRepository::registerObservation(",
            "            handle,",
            "            tile);",
            "    }",
            "",
            "}",
            "",
            "namespace TelemetryComposition",
            "{",
            "",
            "    bool registerObservations()",
            "    {",
            "",
        ]
    )

    for observation in observations:

        lines.extend(
            [
                "        if (!registerObservation(",
                "                ObservationKey{",
                f'                    "{cpp_string(observation.key)}"',
                "                },",
                "                SensorTile{",
                f'                    "{cpp_string(observation.label)}",',
                f'                    "{cpp_string(observation.unit)}",',
                f"                    {observation.sensor_type}",
                "                }))",
                "        {",
                "            return false;",
                "        }",
                "",
            ]
        )

    lines.extend(
        [
            "        return true;",
            "    }",
            "",
            "}",
            "",
        ]
    )

    return "\n".join(lines)


# ---------------------------------------------------------------------------
# MQTT mapping generation
# ---------------------------------------------------------------------------

def generate_mqtt_header() -> str:
    return """\
#pragma once

#include "../../../src/data/ObservationKey.h"

struct TelemetryMqttMapping
{
    const char* topic;
    ObservationKey observation;
};

extern const TelemetryMqttMapping telemetryMqttMappings[];
extern const unsigned int telemetryMqttMappingCount;
"""


def generate_mqtt_cpp(
    observations: list[Observation],
) -> str:

    lines: list[str] = [
        '#include "TelemetryMqttMappings.h"',
        "",
        "const TelemetryMqttMapping telemetryMqttMappings[] =",
        "{",
    ]

    for observation in observations:

        lines.extend(
            [
                "    {",
                f'        "{cpp_string(observation.source_topic)}",',
                "        ObservationKey{",
                f'            "{cpp_string(observation.key)}"',
                "        }",
                "    },",
            ]
        )

    lines.extend(
        [
            "};",
            "",
            "const unsigned int telemetryMqttMappingCount =",
            "    sizeof(telemetryMqttMappings) /",
            "    sizeof(telemetryMqttMappings[0]);",
            "",
        ]
    )

    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Write generated files
# ---------------------------------------------------------------------------

def write_generated_file(
    filename: str,
    content: str,
) -> None:

    path = GENERATED_DIR / filename

    path.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    path.write_text(
        content,
        encoding="utf-8",
    )


# ---------------------------------------------------------------------------
# Composer entry point
# ---------------------------------------------------------------------------

def compose() -> None:

    print(
        "[Telemetry Composer] "
        "Reading telemetry.yaml"
    )

    document = load_composition()

    observations = parse_observations(
        document
    )

    GENERATED_DIR.mkdir(
        parents=True,
        exist_ok=True,
    )

    write_generated_file(
        "TelemetryComposition.h",
        generate_composition_header(),
    )

    write_generated_file(
        "TelemetryComposition.cpp",
        generate_composition_cpp(
            observations
        ),
    )

    write_generated_file(
        "TelemetryMqttMappings.h",
        generate_mqtt_header(),
    )

    write_generated_file(
        "TelemetryMqttMappings.cpp",
        generate_mqtt_cpp(
            observations
        ),
    )

    # PlatformIO normally compiles only src/.
    # BuildSources() explicitly adds the generated tree to the build.
    env.BuildSources(
        str(GENERATED_DIR),
        str(GENERATED_DIR),
    )

    print(
        "[Telemetry Composer] "
        f"Validated {len(observations)} observations"
    )

    print(
        "[Telemetry Composer] "
        f"Generated: {GENERATED_DIR}"
    )

    for observation in observations:

        print(
            "[Telemetry Composer] "
            f"  {observation.name}"
            f" <- {observation.source_type}"
            f": {observation.source_topic}"
        )


try:
    compose()

except CompositionError as exc:
    print(str(exc))
    sys.exit(1)