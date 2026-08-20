/// Backward-compatibility test: read a frozen reference file written by an
/// older (or the current) version of the data model and compare against the
/// canonical value recipe, with expectations masked to the writing version
/// (members that did not exist on disk must read back default-initialized).
/// See tests/data/README.md for the compatibility policy.
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "ROOT/RNTupleReader.hxx"
#include "SHiP/EventHeader.hpp"
#include "SHiP/MCParticle.hpp"
#include "SHiP/RecParticle.hpp"
#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"
#include "SHiP/SimResult.hpp"
#include "reference_values.hpp"
#include "test_utils.hpp"

namespace {

/// Writing version of a reference file; "head" sorts after every release.
struct Version {
  int major{0};
  int minor{0};
  int patch{0};
  bool head{false};

  friend bool operator<(Version const& a, Version const& b) {
    if (a.head != b.head) {
      return b.head;
    }
    if (a.major != b.major) {
      return a.major < b.major;
    }
    if (a.minor != b.minor) {
      return a.minor < b.minor;
    }
    return a.patch < b.patch;
  }
  friend bool operator>=(Version const& a, Version const& b) {
    return !(a < b);
  }
};

constexpr Version kV030{0, 3, 0};
constexpr Version kV040{0, 4, 0};

bool parseVersion(std::string_view arg, Version& out) {
  if (arg == "head") {
    out = Version{0, 0, 0, true};
    return true;
  }
  int major = 0;
  int minor = 0;
  int patch = 0;
  if (std::sscanf(arg.data(), "v%d.%d.%d", &major, &minor, &patch) != 3) {
    return false;
  }
  out = Version{major, minor, patch};
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "usage: test_read_reference <reference.root> <version|head>\n";
    return 64;
  }
  std::string const file = argv[1];
  std::string const versionLabel = argv[2];
  Version version;
  if (!parseVersion(versionLabel, version)) {
    std::cerr << "FAIL: cannot parse version '" << versionLabel << "'\n";
    return 64;
  }

  // Open WITHOUT an imposed model: the model is built from the on-disk
  // descriptor and class fields are reconstructed from the current
  // dictionary, so automatic schema evolution maps on-disk members by name
  // and default-initializes members missing on disk.
  auto reader = ROOT::RNTupleReader::Open("events", file);
  if (!reader) {
    std::cout << "FAIL: cannot open RNTuple 'events' in " << file << '\n';
    return 1;
  }
  auto const& desc = reader->GetDescriptor();

  // Structural expectations: top-level fields present iff the writing
  // version had them (also cross-checks the filename-derived version).
  bool const hasEventHeader =
      desc.FindFieldId("event_header") != ROOT::kInvalidDescriptorId;
  bool ok = SHiP::test::check("event_header present iff >= v0.4.0",
                              version >= kV040, hasEventHeader);
  for (auto const* name : {"mcParticles", "simHits", "simParticles",
                           "recParticles", "simResult"}) {
    ok &=
        SHiP::test::check(std::string(name) + " present", true,
                          desc.FindFieldId(name) != ROOT::kInvalidDescriptorId);
  }
  if (reader->GetNEntries() != SHiP::ref::kEntries) {
    std::cout << "FAIL: expected " << SHiP::ref::kEntries << " entries, got "
              << reader->GetNEntries() << '\n';
    return 1;
  }

  auto const& entry = reader->GetModel().GetDefaultEntry();
  std::shared_ptr<SHiP::EventHeader> eventHeader;
  if (hasEventHeader) {
    eventHeader = entry.GetPtr<SHiP::EventHeader>("event_header");
  }
  auto mcParticles = entry.GetPtr<std::vector<SHiP::MCParticle>>("mcParticles");
  auto simHits = entry.GetPtr<std::vector<SHiP::SimHit>>("simHits");
  auto simParticles =
      entry.GetPtr<std::vector<SHiP::SimParticle>>("simParticles");
  auto recParticles =
      entry.GetPtr<std::vector<SHiP::RecParticle>>("recParticles");
  auto simResult = entry.GetPtr<SHiP::SimResult>("simResult");

  // Mask table: one entry per "member M introduced in version X" — members
  // that did not exist in the writing version must read back as defaults.
  auto maskSimHits = [&](std::vector<SHiP::SimHit>& hits) {
    if (version < kV040) {
      for (auto& h : hits) {
        h.geometry_node_id = 0;
      }
    }
  };
  auto maskRecParticles = [&](std::vector<SHiP::RecParticle>& particles) {
    if (version < kV030) {
      for (auto& p : particles) {
        p.hits.clear();
      }
    }
  };

  for (int i = 0; i < SHiP::ref::kEntries; ++i) {
    reader->LoadEntry(i);
    std::string const suffix =
        " (" + versionLabel + ", entry " + std::to_string(i) + ")";
    if (hasEventHeader) {
      ok &= SHiP::test::check("EventHeader" + suffix,
                              SHiP::ref::makeEventHeader(i), *eventHeader);
    }
    ok &= SHiP::test::check("MCParticle" + suffix,
                            SHiP::ref::makeMCParticles(i), *mcParticles);

    auto expectedSimHits = SHiP::ref::makeSimHits(i);
    maskSimHits(expectedSimHits);
    ok &= SHiP::test::check("SimHit" + suffix, expectedSimHits, *simHits);

    ok &= SHiP::test::check("SimParticle" + suffix,
                            SHiP::ref::makeSimParticles(i), *simParticles);

    auto expectedRecParticles = SHiP::ref::makeRecParticles(i);
    maskRecParticles(expectedRecParticles);
    ok &= SHiP::test::check("RecParticle" + suffix, expectedRecParticles,
                            *recParticles);

    auto expectedSimResult = SHiP::ref::makeSimResult(i);
    maskSimHits(expectedSimResult.hits);
    ok &=
        SHiP::test::check("SimResult" + suffix, expectedSimResult, *simResult);
  }
  std::cout << (ok ? "Compatibility read passed"
                   : "Compatibility read FAILED (see tests/data/README.md)")
            << " for " << versionLabel << '\n';
  return ok ? 0 : 1;
}
