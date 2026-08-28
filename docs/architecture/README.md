# Architecture

This directory records the architectural decisions that shaped Telemetry.

Each document begins with a pressure: something became difficult, something no longer fit, or a new capability created a new question.

The architecture then evolved in response.

## The Journey

1. [From Weather Station to Framework](001-from-weather-station-to-framework.md)
2. [The Display Boundary](002-the-display-boundary.md)
3. [The Data Boundary](003-the-data-boundary.md)
4. [Input Is Semantic](004-input-is-semantic.md)
5. [Who Owns Navigation?](005-who-owns-navigation.md)
6. [The Cost of Abstraction](006-the-cost-of-abstraction.md)
7. [Designing for Observability](007-designing-for-observability.md)

## How to Read These Documents

Each document answers a question created by the previous one:

```text
Pressure
   ↓
Question
   ↓
Architectural decision
   ↓
New capability
   ↓
New pressure
```

Architecture is not a collection of disconnected diagrams.

It is a response to pressure.

The same principle applies to the engineering notebook as a whole.

The purpose of recording the pressure is not to present a flawless history.

It is to preserve the reasoning that led to the decisions.

That is part of the architecture.