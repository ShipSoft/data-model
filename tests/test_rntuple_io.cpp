/// Round-trip test: write every dictionary class to an RNTuple, read it back
/// and compare all members exactly. Returns non-zero on failure.

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ROOT/RNTupleModel.hxx"
#include "ROOT/RNTupleReader.hxx"
#include "ROOT/RNTupleWriter.hxx"
#include "SHiP/MCParticle.hpp"
#include "SHiP/RecParticle.hpp"
#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"
#include "SHiP/SimResult.hpp"
#include "test_utils.hpp"

namespace {

constexpr int kEntries = 2;
constexpr char const* kFileName = "test_rntuple_io_tmp.root";

bool writeFile() {
  auto model = ROOT::RNTupleModel::Create();
  auto mcParticles =
      model->MakeField<std::vector<SHiP::MCParticle>>("mcParticles");
  auto simHits = model->MakeField<std::vector<SHiP::SimHit>>("simHits");
  auto simParticles =
      model->MakeField<std::vector<SHiP::SimParticle>>("simParticles");
  auto recParticles =
      model->MakeField<std::vector<SHiP::RecParticle>>("recParticles");
  auto simResult = model->MakeField<SHiP::SimResult>("simResult");

  auto writer =
      ROOT::RNTupleWriter::Recreate(std::move(model), "events", kFileName);
  if (!writer) {
    std::cout << "FAIL: cannot create RNTuple writer for " << kFileName << '\n';
    return false;
  }
  for (int entry = 0; entry < kEntries; ++entry) {
    *mcParticles = SHiP::test::makeMCParticles(entry);
    *simHits = SHiP::test::makeSimHits(entry);
    *simParticles = SHiP::test::makeSimParticles(entry);
    *recParticles = SHiP::test::makeRecParticles(entry);
    *simResult = SHiP::test::makeSimResult(entry);
    writer->Fill();
  }
  return true;
}

bool readAndCompare() {
  auto reader = ROOT::RNTupleReader::Open("events", kFileName);
  if (!reader) {
    std::cout << "FAIL: cannot open RNTuple 'events' in " << kFileName << '\n';
    return false;
  }
  if (reader->GetNEntries() != kEntries) {
    std::cout << "FAIL: expected " << kEntries << " entries, got "
              << reader->GetNEntries() << '\n';
    return false;
  }

  auto const& entry = reader->GetModel().GetDefaultEntry();
  auto mcParticles = entry.GetPtr<std::vector<SHiP::MCParticle>>("mcParticles");
  auto simHits = entry.GetPtr<std::vector<SHiP::SimHit>>("simHits");
  auto simParticles =
      entry.GetPtr<std::vector<SHiP::SimParticle>>("simParticles");
  auto recParticles =
      entry.GetPtr<std::vector<SHiP::RecParticle>>("recParticles");
  auto simResult = entry.GetPtr<SHiP::SimResult>("simResult");

  bool ok = true;
  for (int i = 0; i < kEntries; ++i) {
    reader->LoadEntry(i);
    std::string const suffix = " (entry " + std::to_string(i) + ")";
    ok &= SHiP::test::check("MCParticle round-trip" + suffix,
                            SHiP::test::makeMCParticles(i), *mcParticles);
    ok &= SHiP::test::check("SimHit round-trip" + suffix,
                            SHiP::test::makeSimHits(i), *simHits);
    ok &= SHiP::test::check("SimParticle round-trip" + suffix,
                            SHiP::test::makeSimParticles(i), *simParticles);
    ok &= SHiP::test::check("RecParticle round-trip" + suffix,
                            SHiP::test::makeRecParticles(i), *recParticles);
    ok &= SHiP::test::check("SimResult round-trip" + suffix,
                            SHiP::test::makeSimResult(i), *simResult);
  }
  return ok;
}

}  // namespace

int main() {
  bool ok = writeFile() && readAndCompare();
  std::remove(kFileName);

  std::cout << (ok ? "All RNTuple I/O tests passed"
                   : "RNTuple I/O tests FAILED")
            << '\n';
  return ok ? 0 : 1;
}
