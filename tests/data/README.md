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
- `reco_fixture_v0.5.0.root` — frozen like the release files, see
  [Reconstruction fixture](#reconstruction-fixture).
- `schema_snapshot.txt` — committed schema dump, see above.

Each reference file is an RNTuple named `events` with 2 entries and
top-level fields `event_header` (v0.4.0+), `mcParticles`, `simHits`,
`simParticles`, `recParticles`, `simResult`.

### Reconstruction fixture

The reference files don't contain `TrackFitResult` or any detector wrapper,
so nothing in them reads those classes back. `reco_fixture_v0.5.0.root`
covers them for the v0.5.0 layout. It holds two RNTuples with 2 entries
each: `trackfits`, with a `trackFitResults` field, and `wrappers`, with a
`ubtHits` field (`UBTHit` stands in for all five wrappers, which share one
shape). `tests/write_reco_fixture.cpp` wrote it against the v0.5.0 headers
through `scripts/backfill_reference_files.sh`. It is frozen like the
reference files. `compat_read_reco_trackfits_v0.5.0` and
`compat_read_reco_wrappers_v0.5.0` read one RNTuple each, so a failure in
one class cannot hide the result for the other.

Since v0.5.0 every persistent class carries an explicit version, declared in
`include/SHiP/LinkDef.h` as `options=version(N)` (required for RNTuple I/O
customization rules, [root-project/root#23146]) — bump it together with any
layout change; the `schema_snapshot` test records versions, so forgetting is
visible in the diff. Numbering starts at 2, because rootcling already emits 1
for classes without `ClassDef` and `TClass` reports that back as -1. Files
from v0.1.0–v0.4.0 were written by unversioned classes. v0.5.0 shipped the
camelCase layout as version 2, and `MCParticle` as version 3 because it
gained `mothers` in that release. The snake_case layout is version 3 for
every class with a renamed member, and version 4 for `MCParticle`.
`EventHeader` and `SimResult` had no member renamed and stay at version 2.

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
- `compat_read_v0.1.0`–`v0.4.0` fail: these four are marked `WILL_FAIL`
  (see the known issue below), so ctest reports them as failing only when
  they *pass*. Read it as good news about ROOT, and drop the marking.
- `compat_read_head` fails but frozen versions pass: the current schema and
  `reference_head.root` are out of sync — run `pixi run
  update-reference-head` (plus the snapshot) in this PR.

## Known issue: field renames vs ROOT 6.40 RNTuple

The snake_case field renames are covered by I/O customization rules in
`include/SHiP/LinkDef.h`, validated end-to-end on the TTree path. ROOT 6.40
however misapplies rules when reading **RNTuple** data written by
*unversioned* classes ([root-project/root#23146]). Per the workaround
proposed there, all classes carry an explicit version (see above) and
readers open files through `TFile` before attaching the `RNTupleReader`, so
the rules work for everything written from v0.5.0 on: `compat_read_v0.5.0`
and `compat_read_head` pass.

The pre-v0.5.0 reference files were written by the then-unversioned classes
and still cannot be rule-read: ROOT aborts on an internal assertion in
`RFieldMeta.cxx`. `compat_read_v0.1.0`–`v0.4.0` therefore carry the ctest
`WILL_FAIL` property, set in `tests/CMakeLists.txt` for every reference file
below v0.5.0. They still run and still assert the true values; ctest only
inverts the verdict.

Two consequences worth knowing:

- The day a ROOT carrying [root-project/root#23196] reaches `pixi.lock`,
  those four tests start **reporting as FAILED**, because `WILL_FAIL` does
  not tolerate a pass. That is the signal to drop the `WILL_FAIL` block and
  this section, not a regression.
- Until then, a genuine regression in one of those four reads is hidden.
  `compat_read_v0.5.0` and `compat_read_head` exercise the same reader and
  the same rules and are not marked, so the exposure is limited to the
  unversioned-file path itself.

The wrappers' `recHit` → `rec_hit` rename is not rule-covered at all:
nested-object rule sources crash ROOT 6.40, and no wrapper data has been
persisted to date.

[root-project/root#23196]: https://github.com/root-project/root/pull/23196

## Value recipe

Expected values are defined in `tests/reference_values.hpp` and are part of
the contract (a future non-C++ reader can check against the same formulas).
For members that existed at v0.1.0 they equal the `SHiP::test::make*`
generators in `tests/test_utils.hpp`; members added later have their own
formulas there (e.g. `SimHit::geometry_node_id = 900 + 7*i + offset`,
`MCParticle::mothers = {}` for `i == 0` and `{i - 1, (i + 1) % 3}` otherwise
— its entries are indices into this same 3-element collection, so unlike the
other members they carry no `offset`; the recipe encodes the documented
invariant, namely that `mothers[0]` equals `mother_id` and that an entry
without a mother has an empty list rather than a `-1` in it —
`RecParticle::hits` filled from `makeSimHits(offset + 2)`). Per entry
`e` (0-based): field offsets are `e` for the top-level collections and
`e + 5` inside `simResult`; each collection has 3 elements. A reference file
written by version V contains values for exactly the members existing in V;
newer members read back default-initialized and are masked accordingly in
`test_read_reference.cpp`.

The mother-index invariants the `MCParticle` recipe encodes are machine-checked
rather than merely asserted here: `SHiP::mothers_are_consistent` (declared beside
the struct in `include/SHiP/MCParticle.hpp`) is exercised against both recipes
by the `mc_mothers` test and against every file this suite reads, so a future
recipe change cannot quietly contradict them.
