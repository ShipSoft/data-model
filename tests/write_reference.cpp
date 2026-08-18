/// Write a frozen compatibility reference file (see tests/data/README.md).
///
/// Version-agnostic: compiles against the headers of every released tag, so
/// the same source both backfills old-version reference files (built against
/// a tag's headers) and writes the current one at release time.
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "ROOT/RNTupleModel.hxx"
#include "ROOT/RNTupleWriter.hxx"
#include "reference_values.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: write_reference <output.root>\n";
    return 64;
  }
  auto model = ROOT::RNTupleModel::Create();
#ifdef SHIP_REF_HAS_EVENT_HEADER
  auto eventHeader = model->MakeField<SHiP::EventHeader>("event_header");
#endif
  auto mcParticles =
      model->MakeField<std::vector<SHiP::MCParticle>>("mcParticles");
  auto simHits = model->MakeField<std::vector<SHiP::SimHit>>("simHits");
  auto simParticles =
      model->MakeField<std::vector<SHiP::SimParticle>>("simParticles");
  auto recParticles =
      model->MakeField<std::vector<SHiP::RecParticle>>("recParticles");
  auto simResult = model->MakeField<SHiP::SimResult>("simResult");
  auto writer =
      ROOT::RNTupleWriter::Recreate(std::move(model), "events", argv[1]);
  if (!writer) {
    std::cerr << "FAIL: cannot create RNTuple writer for " << argv[1] << '\n';
    return 1;
  }
  for (int entry = 0; entry < SHiP::ref::kEntries; ++entry) {
#ifdef SHIP_REF_HAS_EVENT_HEADER
    *eventHeader = SHiP::ref::makeEventHeader(entry);
#endif
    *mcParticles = SHiP::ref::makeMCParticles(entry);
    *simHits = SHiP::ref::makeSimHits(entry);
    *simParticles = SHiP::ref::makeSimParticles(entry);
    *recParticles = SHiP::ref::makeRecParticles(entry);
    *simResult = SHiP::ref::makeSimResult(entry);
    writer->Fill();
  }
  std::cout << "wrote " << SHiP::ref::kEntries << " entries to " << argv[1]
            << '\n';
  return 0;
}
