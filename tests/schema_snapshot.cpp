/// Schema snapshot gate: dump the persistent schema of every dictionary class
/// (TClass layout + RNTuple field tree) and compare it against the committed
/// snapshot, so that ANY schema change — even a backward-compatible one —
/// fails CI until the snapshot is deliberately regenerated in the same PR.
///
///   schema_snapshot --check <snapshot.txt>   exit 1 on mismatch (ctest)
///   schema_snapshot --update <snapshot.txt>  rewrite the snapshot
///
/// The class list is generated at configure time from include/SHiP/LinkDef.h
/// (see tests/CMakeLists.txt), so newly linked classes enter the snapshot
/// automatically. The dump is deterministic for a given dictionary and ROOT
/// version; a ROOT version bump may change it (e.g. type-name normalization),
/// in which case the snapshot is regenerated in the same PR.
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "ROOT/RField.hxx"
#include "ROOT/RNTupleDescriptor.hxx"
#include "ROOT/RNTupleModel.hxx"
#include "ROOT/RNTupleReader.hxx"
#include "ROOT/RNTupleWriter.hxx"
#include "TClass.h"
#include "TDataMember.h"
#include "TList.h"

namespace {

char const* const kDictionaryClasses[] = {
#include "dictionary_classes.inc"
};

constexpr char const* kProbeFileName = "schema_snapshot_probe_tmp.root";

void dumpTClassSection(std::ostream& os) {
  os << "# TClass layout (version, checksum, data members)\n";
  for (auto const* name : kDictionaryClasses) {
    auto* cls = TClass::GetClass(name);
    if (cls == nullptr) {
      os << "class " << name << " MISSING FROM DICTIONARY\n";
      continue;
    }
    os << "class " << name << " version=" << cls->GetClassVersion()
       << " checksum=0x" << std::hex << cls->GetCheckSum() << std::dec << '\n';
    for (auto const* obj : *cls->GetListOfDataMembers()) {
      auto const* member = static_cast<TDataMember const*>(obj);
      if (!member->IsPersistent()) {
        continue;
      }
      os << "  " << member->GetTrueTypeName() << ' ' << member->GetName()
         << '\n';
    }
  }
}

void dumpField(ROOT::RNTupleDescriptor const& desc, ROOT::DescriptorId_t id,
               int indent, std::ostream& os) {
  auto const& field = desc.GetFieldDescriptor(id);
  os << std::string(2 * indent, ' ') << field.GetFieldName() << " : "
     << field.GetTypeName() << " (typever=" << field.GetTypeVersion();
  if (auto checksum = field.GetTypeChecksum()) {
    os << ", checksum=0x" << std::hex << *checksum << std::dec;
  }
  os << ")\n";
  for (auto const& child : desc.GetFieldIterable(id)) {
    dumpField(desc, child.GetId(), indent + 1, os);
  }
}

bool dumpRNTupleSection(std::ostream& os) {
  os << "# RNTuple field tree (one top-level field per dictionary class)\n";
  auto model = ROOT::RNTupleModel::Create();
  for (auto const* name : kDictionaryClasses) {
    std::string fieldName = name;
    for (auto& c : fieldName) {
      if (c == ':') {
        c = '_';
      }
    }
    model->AddField(ROOT::RFieldBase::Create(fieldName, name).Unwrap());
  }
  {
    auto writer = ROOT::RNTupleWriter::Recreate(std::move(model),
                                                "schema_probe", kProbeFileName);
    if (!writer) {
      std::cerr << "FAIL: cannot create probe RNTuple\n";
      return false;
    }
  }
  auto reader = ROOT::RNTupleReader::Open("schema_probe", kProbeFileName);
  if (!reader) {
    std::cerr << "FAIL: cannot reopen probe RNTuple\n";
    return false;
  }
  auto const& desc = reader->GetDescriptor();
  for (auto const& field : desc.GetFieldIterable(desc.GetFieldZeroId())) {
    dumpField(desc, field.GetId(), 0, os);
  }
  std::remove(kProbeFileName);
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 3 || (std::string(argv[1]) != "--check" &&
                    std::string(argv[1]) != "--update")) {
    std::cerr << "usage: schema_snapshot --check|--update <snapshot.txt>\n";
    return 64;
  }
  std::string const mode = argv[1];
  std::string const snapshotPath = argv[2];

  std::ostringstream dump;
  dumpTClassSection(dump);
  if (!dumpRNTupleSection(dump)) {
    return 1;
  }

  if (mode == "--update") {
    std::ofstream out{snapshotPath};
    out << dump.str();
    if (!out) {
      std::cerr << "FAIL: cannot write " << snapshotPath << '\n';
      return 1;
    }
    std::cout << "schema snapshot updated: " << snapshotPath << '\n';
    return 0;
  }

  std::ifstream in{snapshotPath};
  if (!in) {
    std::cout << "FAIL: cannot read snapshot " << snapshotPath
              << " — run: pixi run update-schema-snapshot\n";
    return 1;
  }
  std::stringstream committed;
  committed << in.rdbuf();
  if (committed.str() == dump.str()) {
    std::cout << "schema snapshot matches\n";
    return 0;
  }

  std::istringstream expected{committed.str()};
  std::istringstream actual{dump.str()};
  std::string expectedLine;
  std::string actualLine;
  int lineNumber = 1;
  while (true) {
    bool const haveExpected =
        static_cast<bool>(std::getline(expected, expectedLine));
    bool const haveActual = static_cast<bool>(std::getline(actual, actualLine));
    if (!haveExpected && !haveActual) {
      break;
    }
    if (!haveExpected || !haveActual || expectedLine != actualLine) {
      std::cout << "schema snapshot MISMATCH at line " << lineNumber << ":\n"
                << "  committed: "
                << (haveExpected ? expectedLine : "<end of file>") << '\n'
                << "  current:   "
                << (haveActual ? actualLine : "<end of file>") << '\n';
      break;
    }
    ++lineNumber;
  }
  std::cout << "The persistent schema changed. If intentional, regenerate the\n"
               "snapshot in this PR (and reference_head.root if the event\n"
               "model changed):\n"
               "  pixi run update-schema-snapshot\n"
               "  pixi run update-reference-head\n"
               "See tests/data/README.md for the compatibility policy.\n";
  return 1;
}
