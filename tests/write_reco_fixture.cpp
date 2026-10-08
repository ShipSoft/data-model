/// Write the frozen reconstruction fixture (see tests/data/README.md).
///
/// The reference files written by write_reference carry no TrackFitResult and
/// no detector wrapper, so this writes them separately: one RNTuple `trackfits`
/// and one RNTuple `wrappers`, each with SHiP::ref::kEntries entries. Two
/// RNTuples rather than two fields, so that a reader failing on one class
/// cannot hide the result for the other. Like write_reference, it compiles
/// against the headers of any tag from v0.3.0 on.
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "ROOT/RNTupleModel.hxx"
#include "ROOT/RNTupleWriter.hxx"
#include "TFile.h"
#include "reference_values.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: write_reco_fixture <output.root>\n";
    return 64;
  }
  std::unique_ptr<TFile> file{TFile::Open(argv[1], "RECREATE")};
  if (!file || file->IsZombie()) {
    std::cerr << "FAIL: cannot create " << argv[1] << '\n';
    return 1;
  }
  {
    auto model = ROOT::RNTupleModel::Create();
    auto const trackFitResults =
        model->MakeField<std::vector<SHiP::TrackFitResult>>("trackFitResults");
    auto writer =
        ROOT::RNTupleWriter::Append(std::move(model), "trackfits", *file);
    for (int entry = 0; entry < SHiP::ref::kEntries; ++entry) {
      *trackFitResults = SHiP::ref::makeTrackFitResults(entry);
      writer->Fill();
    }
  }
  {
    auto model = ROOT::RNTupleModel::Create();
    auto const ubtHits = model->MakeField<std::vector<SHiP::UBTHit>>("ubtHits");
    auto writer =
        ROOT::RNTupleWriter::Append(std::move(model), "wrappers", *file);
    for (int entry = 0; entry < SHiP::ref::kEntries; ++entry) {
      *ubtHits = SHiP::ref::makeUBTHits(entry);
      writer->Fill();
    }
  }
  std::cout << "wrote " << SHiP::ref::kEntries
            << " entries each to trackfits and wrappers in " << argv[1] << '\n';
  return 0;
}
