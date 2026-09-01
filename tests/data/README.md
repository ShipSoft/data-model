# Backward-compatibility reference data

This directory holds the on-disk compatibility contract of the SHiP data
model. Two independent safety nets are exercised by ctest (part of
`pixi run test` and CI):

1. **Compat read tests** (`compat_read_<version>`): every reference file
   must be readable with the *current* library, materialized into the
   current structs, with all values matching the canonical recipe. Members
   that did not exist in the writing version must read back
   default-initialized (ROOT RNTuple automatic schema evolution).
2. **Schema snapshot** (`schema_snapshot`): `schema_snapshot.txt` is a text
   dump of the persistent schema (TClass layout of every dictionary class +
   the RNTuple field tree). CI fails on *any* schema change — even a
   backward-compatible one — until the snapshot is deliberately regenerated
   in the same PR, so every schema change is a conscious, reviewable
   decision.

## Files

- `reference_v<X.Y.Z>.root` — frozen forever, **never modified or
  regenerated**. Written at release time by `scripts/release.sh` from
  exactly the tagged code with the ROOT version pinned in `pixi.lock` at
  that moment, so it captures both the schema and the writing ROOT.
  Exception: the files for v0.1.0–v0.4.0 predate this suite and were
  backfilled with `scripts/backfill_reference_files.sh` — they were written
  by each tag's *headers* (defining the on-disk schema) but by ROOT 6.40.02,
  not the historical ROOT versions.
- `reference_head.root` — tracks `main`; asserts "current code reads the
  current schema". Regenerated in the same PR as any event-model change.
- `schema_snapshot.txt` — committed schema dump, see above.

Each file is an RNTuple named `events` with 2 entries and top-level fields
`event_header` (v0.4.0+), `mcParticles`, `simHits`, `simParticles`,
`recParticles`, `simResult`.

Since v0.5.0 every persistent class carries an explicit version, declared in
`include/SHiP/LinkDef.h` as `options=version(N)` (required for RNTuple I/O
customization rules, [root-project/root#23146]) — bump it together with any
layout change; the `schema_snapshot` test records versions, so forgetting is
visible in the diff. Numbering starts at 2, because rootcling already emits 1
for classes without `ClassDef` and `TClass` reports that back as -1. Files
from v0.1.0–v0.4.0 were written by unversioned classes.

[root-project/root#23146]: https://github.com/root-project/root/issues/23146

## When a compat test fails in your PR

- `schema_snapshot` fails, `compat_read_*` pass: you changed the persistent
  schema in a backward-compatible way (e.g. added a member). If intentional,
  run and commit in the same PR:

  ```sh
  pixi run update-schema-snapshot
  pixi run update-reference-head
  ```

  This is at least a **minor** version bump.
- `compat_read_v*` fails: your change breaks reading of existing files
  (e.g. renaming a member silently drops its on-disk values — RNTuple
  matches members by name). Either make the change compatible (e.g. an
  [I/O customization rule](https://root.cern/doc/master/md_tree_2ntuple_2doc_2SchemaEvolution.html)
  mapping the old name), or accept it as a **breaking change**: mark the
  commit `!`/`BREAKING CHANGE` (major version bump) and adjust the
  expectations in `tests/test_read_reference.cpp` (masking table) — never by
  editing the frozen files.
- `compat_read_head` fails but frozen versions pass: the current schema and
  `reference_head.root` are out of sync — run `pixi run
  update-reference-head` (plus the snapshot) in this PR.

## Value recipe

Expected values are defined in `tests/reference_values.hpp` and are part of
the contract (a future non-C++ reader can check against the same formulas).
For members that existed at v0.1.0 they equal the `SHiP::test::make*`
generators in `tests/test_utils.hpp`; members added later have their own
formulas there (e.g. `SimHit::geometryNodeId = 900 + 7*i + offset`,
`RecParticle::hits` filled from `makeSimHits(offset + 2)`). Per entry
`e` (0-based): field offsets are `e` for the top-level collections and
`e + 5` inside `simResult`; each collection has 3 elements. A reference file
written by version V contains values for exactly the members existing in V;
newer members read back default-initialized and are masked accordingly in
`test_read_reference.cpp`.
