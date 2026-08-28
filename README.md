# Telemetry

**An open engineering notebook for people who want to understand not only how systems are built, but why they're designed that way.**

## Stop

Before you clone this repository, ask yourself one question.

Are you looking for another ESP8266 project?

Or are you trying to understand how engineers think?

If you're looking for another weather station, MQTT dashboard, TFT display, or ESP32 framework, there are already thousands of excellent repositories that will get you there.

Telemetry exists for a different reason.

The firmware isn't the destination.

It's the conversation.

The real goal is the moment when something clicks and you find yourself saying:

**"Ahh… now I understand why."**

If that sounds interesting…

Welcome aboard.

# What is Telemetry?

Telemetry began life as a simple weather display running on an ESP8266.

Like many hobby projects, it started with a single objective:

Display sensor values on a screen.

That objective didn't survive very long.

Every feature introduced another architectural question.

Should the display know about MQTT?

Should touch input know about the screen?

Should calibration interrupt boot?

Why does a WeatherScreen know it's running on an ILI9341?

Could the same code run on a completely different display?

Could touch be replaced by buttons?

Could the application become independent of the hardware?

Every answer led to another question.

Eventually it became clear that this repository wasn't really about weather.

Or displays.

Or telemetry.

It was about learning how to think about systems.

Today Telemetry is an embedded framework.

More importantly, it's an engineering notebook.

# Who is this for?

Telemetry was built for curious people.

You might be:

- an electronics enthusiast building your first ESP project
- a firmware developer looking for architectural ideas
- a software engineer exploring embedded systems
- a systems engineer interested in observability
- an enterprise architect wondering how good architectural thinking scales down to microcontrollers
- someone who simply enjoys understanding how things work

You don't need to know everything before you start.

None of us did.

# What makes Telemetry different?

Most repositories teach **what**.

Some teach **how**.

Telemetry tries to teach **why**.

The source code is only part of the project.

- The architecture.
- The design decisions.
- The abstractions.
- The naming.
- The trade-offs.
- The failures.

Those are the interesting parts.

Every major design decision exists for a reason.

Where possible, that reason is documented.

Because understanding the reasoning is ultimately far more valuable than memorising the implementation.

# Core Principles

Telemetry follows a few simple principles.

## Teach the reasoning, not just the result

Code answers _what_.

Architecture answers _why_.

Good engineers need both.

## Every abstraction has a cost

Abstractions are powerful.

They're also expensive.

Telemetry introduces abstractions only when they make the system easier to understand in the long term.

## Hardware should be replaceable

Applications shouldn't know what display they're running on.

Displays shouldn't know where data comes from.

Input devices shouldn't know which screen they're controlling.

Replacing hardware shouldn't require rewriting application logic.

## Comments explain intent

The compiler already understands syntax.

Future humans need help understanding intent.

Telemetry comments explain _why something exists_, not _what C++ already says_.

## Debugging is science

Good debugging follows the same process regardless of whether you're investigating:

- an ESP8266
- a routing issue
- a VLAN
- an MQTT broker
- a distributed application

The process is always the same.

- Observe.
- Form a hypothesis.
- Design an experiment.
- Collect evidence.
- Revise your model.
- Repeat.

## Design for observability

The original Telemetry hardware was built without a dedicated USB or serial connection.

At the time, that seemed like a reasonable engineering decision. The device was intended to be a small, network-connected display, and there was no expectation that serial diagnostics would become an important part of development.

During development, that assumption became a constraint.

When the touch system needed to be diagnosed, there was no convenient serial console available.

Rather than redesign the hardware, we made the display itself an observability surface.

A diagnostic overlay was introduced so that the system could expose the information needed to investigate touch behaviour, including:

- the interpreted `InputAction`
- screen coordinates
- raw touch coordinates

The constraint turned out to be useful.

It forced us to make the system observable through an interface the hardware already possessed.

The resulting diagnostic capability became part of the development architecture rather than a collection of temporary debug prints.

The later ESP32/CYD platform provides USB and therefore makes conventional serial diagnostics much easier. That does not make the original hardware decision a mistake. It gives us two useful engineering perspectives:

```text
ESP8266
    │
    └── constrained observability
            → instrument what the system already exposes

ESP32/CYD
    │
    └── expanded observability
            → conventional development interfaces available
```

The two platforms therefore teach different lessons.

The ESP8266 is a deliberately constrained reference platform. Its limitations encourage disciplined resource use and force us to think carefully about what the system needs to expose.

The ESP32/CYD provides a richer development and runtime environment in which more conventional instrumentation and integration techniques become practical.

> **Design for observability. When observability was not designed in, engineer another way to observe the system.**

The engineering story behind this decision is recorded in [Designing for Observability](docs/architecture/007-designing-for-observability.md).

## Firmware should be observable

Invisible systems are difficult to understand.

Telemetry embraces logging, diagnostics, status screens and clear runtime behaviour because engineering starts with observation.

## Architecture is User Experience

Not just for end users.

- For developers.
- For contributors.
- For future-you.

Readable code is a feature.

# Repository Structure

The project intentionally separates concerns.

```text
Application
    │
    ▼
ScreenManager
    │
    ▼
Input Actions
    │
    ▼
InputManager
    │
    ▼
Hardware Profiles
    │
    ▼
Physical Hardware
```

Every layer exists to reduce coupling.

Each component has a single responsibility.

Whenever possible, dependencies point downwards.

# What You'll Find Here

Telemetry currently includes:

- modular screen management
- device-independent input architecture
- board profiles
- display abstraction
- touch abstraction
- MQTT integration
- OTA updates
- filesystem persistence
- calibration support
- sensor repository pattern
- configuration framework
- extensive documentation explaining architectural decisions

Future work will continue moving toward a fully hardware-independent embedded framework.

# More Than Embedded

The principles explored here extend far beyond firmware.

The same ideas apply to:

- networking
- distributed systems
- enterprise architecture
- cloud platforms
- user experience
- software engineering

Technology changes.

Good engineering principles don't.

# Contributing

Pull requests are welcome.

Questions are even more welcome.

If you disagree with an architectural decision, explain your reasoning.

Engineering progresses through discussion, not certainty.

# Why I Built This

Throughout my career I've been fortunate enough to work across electronics, networking, enterprise architecture and user experience.

The technology has changed many times.

The way of thinking never has.

I've always believed the best mentors don't simply provide answers.

They help people understand why those answers make sense.

Telemetry is my attempt to do the same.

If something in this repository helps you pause, smile, and think:

> **"Ahh… now I understand why."**

then Telemetry has achieved exactly what it was built to do.

_"Leave the code - and the engineer reading it - better than you found them."_