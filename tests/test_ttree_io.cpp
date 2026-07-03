/// Round-trip test: write every dictionary class to a TTree branch, read it
/// back and compare all members exactly. Returns non-zero on failure.

#include <TClass.h>
#include <TFile.h>
#include <TTree.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "SHiP/MCParticle.hpp"
#include "SHiP/RecParticle.hpp"
#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"
#include "SHiP/SimResult.hpp"
#include "test_utils.hpp"

namespace {

constexpr int kEntries = 2;
constexpr char const* kFileName = "test_ttree_io_tmp.root";

bool checkDictionaries() {
  bool ok = true;
  for (auto const* name :
       {"SHiP::MCParticle", "SHiP::SimHit", "SHiP::SimParticle",
        "SHiP::SimResult", "SHiP::RecParticle", "std::vector<SHiP::MCParticle>",
        "std::vector<SHiP::SimHit>", "std::vector<SHiP::SimParticle>",
        "std::vector<SHiP::RecParticle>"}) {
    if (TClass::GetClass(name) == nullptr) {
      std::cout << "dictionary for " << name
                << ": FAIL (TClass::GetClass is null)\n";
      ok = false;
    } else {
      std::cout << "dictionary for " << name << ": PASS\n";
    }
  }
  return ok;
}

bool writeFile() {
  TFile file(kFileName, "RECREATE");
  if (file.IsZombie()) {
    std::cout << "FAIL: cannot create " << kFileName << '\n';
    return false;
  }
  TTree tree("events", "SHiP data model I/O test");

  std::vector<SHiP::MCParticle> mcParticles;
  std::vector<SHiP::SimHit> simHits;
  std::vector<SHiP::SimParticle> simParticles;
  std::vector<SHiP::RecParticle> recParticles;
  SHiP::SimResult simResult;

  tree.Branch("mcParticles", &mcParticles);
  tree.Branch("simHits", &simHits);
  tree.Branch("simParticles", &simParticles);
  tree.Branch("recParticles", &recParticles);
  tree.Branch("simResult", &simResult);

  for (int entry = 0; entry < kEntries; ++entry) {
    mcParticles = SHiP::test::makeMCParticles(entry);
    simHits = SHiP::test::makeSimHits(entry);
    simParticles = SHiP::test::makeSimParticles(entry);
    recParticles = SHiP::test::makeRecParticles(entry);
    simResult = SHiP::test::makeSimResult(entry);
    tree.Fill();
  }
  file.Write();
  return true;
}

bool readAndCompare() {
  TFile file(kFileName, "READ");
  if (file.IsZombie()) {
    std::cout << "FAIL: cannot open " << kFileName << '\n';
    return false;
  }
  auto* tree = file.Get<TTree>("events");
  if (tree == nullptr) {
    std::cout << "FAIL: tree 'events' not found\n";
    return false;
  }
  if (tree->GetEntries() != kEntries) {
    std::cout << "FAIL: expected " << kEntries << " entries, got "
              << tree->GetEntries() << '\n';
    return false;
  }

  auto* mcParticles = new std::vector<SHiP::MCParticle>();
  auto* simHits = new std::vector<SHiP::SimHit>();
  auto* simParticles = new std::vector<SHiP::SimParticle>();
  auto* recParticles = new std::vector<SHiP::RecParticle>();
  auto* simResult = new SHiP::SimResult();

  tree->SetBranchAddress("mcParticles", &mcParticles);
  tree->SetBranchAddress("simHits", &simHits);
  tree->SetBranchAddress("simParticles", &simParticles);
  tree->SetBranchAddress("recParticles", &recParticles);
  tree->SetBranchAddress("simResult", &simResult);

  bool ok = true;
  for (int entry = 0; entry < kEntries; ++entry) {
    tree->GetEntry(entry);
    std::string const suffix = " (entry " + std::to_string(entry) + ")";
    ok &= SHiP::test::check("MCParticle round-trip" + suffix,
                            SHiP::test::makeMCParticles(entry), *mcParticles);
    ok &= SHiP::test::check("SimHit round-trip" + suffix,
                            SHiP::test::makeSimHits(entry), *simHits);
    ok &= SHiP::test::check("SimParticle round-trip" + suffix,
                            SHiP::test::makeSimParticles(entry), *simParticles);
    ok &= SHiP::test::check("RecParticle round-trip" + suffix,
                            SHiP::test::makeRecParticles(entry), *recParticles);
    ok &= SHiP::test::check("SimResult round-trip" + suffix,
                            SHiP::test::makeSimResult(entry), *simResult);
  }

  tree->ResetBranchAddresses();
  delete mcParticles;
  delete simHits;
  delete simParticles;
  delete recParticles;
  delete simResult;
  return ok;
}

}  // namespace

int main() {
  bool ok = checkDictionaries();
  ok &= writeFile() && readAndCompare();
  std::remove(kFileName);

  std::cout << (ok ? "All TTree I/O tests passed" : "TTree I/O tests FAILED")
            << '\n';
  return ok ? 0 : 1;
}
