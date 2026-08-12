/// Backward-compatibility test for the frozen reconstruction fixture: read one
/// RNTuple of it (`trackfits` or `wrappers`) with the current library and
/// compare against the recipe in reference_values.hpp. See
/// tests/data/README.md.
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "ROOT/RNTuple.hxx"
#include "ROOT/RNTupleReader.hxx"
#include "SHiP/TrackFitResult.hpp"
#include "SHiP/detectors/UBTHit.hpp"
#include "TError.h"
#include "TFile.h"
#include "reference_values.hpp"
#include "test_utils.hpp"

namespace {

// Neither class defines operator==, so compare member by member here.
auto members(SHiP::TrackFitResult const& f) {
  return std::tie(f.n_meas, f.fit_status, f.chi2, f.ndf, f.q_over_p, f.phi,
                  f.theta, f.time, f.ref_loc, f.input_measurements_x,
                  f.input_measurements_y, f.fitted_measurements_x,
                  f.fitted_measurements_y, f.residuals_x, f.residuals_y);
}

bool same(std::vector<SHiP::TrackFitResult> const& a,
          std::vector<SHiP::TrackFitResult> const& b) {
  if (a.size() != b.size()) {
    return false;
  }
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (members(a[i]) != members(b[i])) {
      return false;
    }
  }
  return true;
}

bool same(std::vector<SHiP::UBTHit> const& a,
          std::vector<SHiP::UBTHit> const& b) {
  if (a.size() != b.size()) {
    return false;
  }
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (!(a[i].rec_hit == b[i].rec_hit)) {
      return false;
    }
  }
  return true;
}

template <typename T>
bool checkEntries(ROOT::RNTupleReader& reader, char const* field,
                  std::vector<T> (*make)(int)) {
  auto const value =
      reader.GetModel().GetDefaultEntry().GetPtr<std::vector<T>>(field);
  bool ok = true;
  for (int i = 0; i < SHiP::ref::kEntries; ++i) {
    reader.LoadEntry(i);
    ok &= SHiP::test::check(
        std::string(field) + " (entry " + std::to_string(i) + ")", true,
        same(make(i), *value));
  }
  return ok;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "usage: test_read_reco_fixture <fixture.root> "
                 "<trackfits|wrappers>\n";
    return 64;
  }
  std::string const file = argv[1];
  std::string const ntuple = argv[2];
  if (ntuple != "trackfits" && ntuple != "wrappers") {
    std::cerr << "FAIL: unknown RNTuple '" << ntuple << "'\n";
    return 64;
  }

  // Exit cleanly on a Fatal ROOT error instead of aborting, as in
  // test_read_reference.cpp.
  SetErrorHandler(
      +[](int level, Bool_t, char const* location, char const* message) {
        DefaultErrorHandler(level, kFALSE, location, message);
        if (level >= kFatal) {
          std::cout << "FAIL: fatal ROOT error (see message above)\n";
          std::exit(1);
        }
      });

  // Open through TFile first so the on-disk streamer infos are known to the
  // I/O customization rules (root-project/root#23146).
  std::unique_ptr<TFile> rootFile{TFile::Open(file.c_str())};
  if (!rootFile || rootFile->IsZombie()) {
    std::cout << "FAIL: cannot open file " << file << '\n';
    return 1;
  }
  std::unique_ptr<ROOT::RNTuple> const anchor{
      rootFile->Get<ROOT::RNTuple>(ntuple.c_str())};
  if (!anchor) {
    std::cout << "FAIL: no RNTuple '" << ntuple << "' in " << file << '\n';
    return 1;
  }
  auto reader = ROOT::RNTupleReader::Open(*anchor);
  if (!reader) {
    std::cout << "FAIL: cannot open RNTuple '" << ntuple << "' in " << file
              << '\n';
    return 1;
  }
  if (reader->GetNEntries() != SHiP::ref::kEntries) {
    std::cout << "FAIL: expected " << SHiP::ref::kEntries << " entries, got "
              << reader->GetNEntries() << '\n';
    return 1;
  }

  bool const ok =
      ntuple == "trackfits"
          ? checkEntries(*reader, "trackFitResults",
                         &SHiP::ref::makeTrackFitResults<>)
          : checkEntries(*reader, "ubtHits", &SHiP::ref::makeUBTHits<>);
  std::cout << (ok ? "Compatibility read passed"
                   : "Compatibility read FAILED (see tests/data/README.md)")
            << " for " << ntuple << '\n';
  return ok ? 0 : 1;
}
