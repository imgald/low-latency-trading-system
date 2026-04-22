# Low Latency Trading System

A deterministic, replay-driven C++20 trading kernel for learning low-latency architecture with correctness first.

Detailed implementation history (all completed phases and module deep-dives):
- [Implemented Phase Details](./docs/implemented_history.md)

## What This Repo Is

- single process, single symbol core
- deterministic replay (`same input -> same output`)
- fixed-point price + accounting-first state transitions
- teaching-oriented architecture that can evolve toward production patterns

## Current Big Architecture

Hot path (synchronous):

`MarketTick -> Strategy -> Risk -> Execution -> Accounting -> OMS state update`

Cold/side path (optional async):

`event sink/logging/persistence` via SPSC ring buffer (`sync` vs `async` modes for A/B latency comparison)

## Principles (Non-Negotiable)

- determinism over convenience
- fixed-point integer pricing (no float PnL logic)
- accounting correctness before performance
- explicit state machines (execution + order lifecycle)
- measure tail latency, not just averages

## Roadmap (High Level)

Completed:

- Phase 1: minimal runnable deterministic kernel
- Phase 2: latency instrumentation (`p50/p99/p99.9/max/tail mean`)
- Phase 3: SPSC async side-channel decoupling
- Phase 4: memory/layout cleanup and hot-path struct discipline
- Phase 5: more realistic execution simulation (partial fill/queue/cancel latency signals)
- Phase 6: OMS + mock gateway + order lifecycle state transitions
- Phase 6.5: OMS transition observability and reject-path visibility

Planned:

- Phase 7: resume-ready realism + benchmarking
  - reproducible benchmark harness + CI regression gates
  - stronger risk controls (`max_loss`, `kill_switch`, stale data guards)
  - baseline vs optimized latency evidence tables

## Immediate TODOs

- add reproducible benchmark scripts and fixed replay datasets
- add deterministic replay diff check in CI
- extend risk engine with loss/staleness guards
- add property-style accounting/risk invariants tests
- publish benchmark report section in README

## Build & Run

Build:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Run:

```powershell
.\build\trading_main.exe .\data\sample_replay.csv async .\data\event_journal_async.csv
```

General CLI:

```text
trading_main <csv-path> [sync|async] [persistence-path] [aggressive|passive] [passive-cancel-after-ns]
```
