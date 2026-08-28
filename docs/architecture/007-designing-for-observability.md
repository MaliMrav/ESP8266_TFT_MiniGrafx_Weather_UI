# Designing for Observability

## The Pressure

The original Telemetry hardware was built without a dedicated USB or serial connection.

At the time, that appeared reasonable.

The device was intended to be a small, network-connected display, and serial diagnostics were not considered a requirement for the deployed system.

Development disagreed.

When the touch system became difficult to diagnose, there was no convenient serial console available.

The problem was therefore not simply:

> "How do we debug the touch screen?"

It became:

> **"How do we make the system observable when the obvious diagnostic interface does not exist?"**

---

## The Constraint

The hardware already provided something capable of displaying information:

```text
TFT display
```

That suggested a different diagnostic path.

Instead of adding a new hardware interface, the existing display could temporarily become an observability surface.

The resulting approach was:

```text
Initial assumption
        ↓
No USB / serial interface
        ↓
Touch debugging becomes difficult
        ↓
Constraint becomes explicit
        ↓
Display becomes diagnostic surface
        ↓
Diagnostic overlay
        ↓
Observe the system
        ↓
Form hypothesis
        ↓
Test
```

---

## The Diagnostic Overlay

`DebugOverlay` exposes the information required to investigate touch behaviour directly on the display.

The diagnostic information includes:

- the interpreted `InputAction`
- the mapped screen coordinates
- the raw touch coordinates

The overlay is controlled by a compile-time `DEBUG_OVERLAY` switch and therefore does not form part of a production build when disabled.

This is important architecturally.

The diagnostic capability is not merely a collection of temporary `Serial.print()` statements.

It is a deliberate way of making otherwise invisible runtime behaviour observable.

---

## What We Learned

The immediate lesson was practical:

> **When the preferred diagnostic interface is unavailable, use the interfaces the system already has.**

The deeper lesson is architectural:

> **Observability is itself a system capability.**

A system that cannot expose enough information to investigate its own behaviour is difficult to engineer, regardless of how correct the underlying implementation might be.

The lack of a serial interface therefore became a useful constraint.

It forced us to make the interaction pipeline visible:

```text
Touch
  ↓
TouchManager
  ↓
InputAction + position
  ↓
Screen
```

That visibility helped us reason about the system rather than guessing about it.

---

## Constraint as a Teacher

The later ESP32/CYD platform provides USB and makes conventional serial diagnostics much easier.

That is not evidence that the original hardware decision was a failure.

It gives Telemetry two useful reference environments:

```text
ESP8266
    │
    └── constrained observability
            → make the existing system explain itself

ESP32/CYD
    │
    └── expanded observability
            → conventional development interfaces are available
```

The ESP8266 remains valuable precisely because its limitations make resource use, instrumentation and architectural boundaries visible.

The ESP32/CYD provides a richer environment in which more advanced integration and control capabilities can be explored.

---

## The Engineering Lesson

Good debugging is not:

```text
Something is wrong
      ↓
Change something
      ↓
Flash firmware
      ↓
Hope
```

It is:

```text
Observe
   ↓
Form a hypothesis
   ↓
Design an experiment
   ↓
Collect evidence
   ↓
Revise the model
   ↓
Repeat
```

The diagnostic display became useful because it enabled that process.

> **A constraint can expose an architectural capability that otherwise might never have been designed.**

And the broader principle is:

> **Design for observability. When observability was not designed in, engineer another way to observe the system.**