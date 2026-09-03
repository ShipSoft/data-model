// Dictionary selection for the SHiP data model.
//
// Every persistent class carries an explicit version via
// `options=version(N)`; RNTuple I/O customization rules misbehave for
// unversioned classes (root-project/root#23146). Declaring it here rather
// than with ClassDef keeps the data-model headers free of any ROOT
// dependency. Bump the version in the same change as any layout
// modification (member added/removed/renamed/retyped).
//
// Numbering starts at 2: rootcling already emits 1 for classes without
// ClassDef, and TClass reports that back as -1, so `version(1)` is
// indistinguishable from no version at all.

#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Event metadata
#pragma link C++ options=version(2) class SHiP::EventHeader+;

// MC / generation
#pragma link C++ options=version(3) class SHiP::MCParticle+;
#pragma link C++ class std::vector<SHiP::MCParticle>+;

// Simulation
#pragma link C++ options=version(2) class SHiP::SimHit+;
#pragma link C++ options=version(2) class SHiP::SimParticle+;
#pragma link C++ options=version(2) class SHiP::SimResult+;
#pragma link C++ class std::vector<SHiP::SimHit>+;
#pragma link C++ class std::vector<SHiP::SimParticle>+;

// Digitisation
#pragma link C++ options=version(2) class SHiP::RecHit+;
#pragma link C++ class std::vector<SHiP::RecHit>+;
#pragma link C++ options=version(2) class SHiP::UBTHit+;
#pragma link C++ class std::vector<SHiP::UBTHit>+;
#pragma link C++ options=version(2) class SHiP::SBTHit+;
#pragma link C++ class std::vector<SHiP::SBTHit>+;
#pragma link C++ options=version(2) class SHiP::TimeDetHit+;
#pragma link C++ class std::vector<SHiP::TimeDetHit>+;
#pragma link C++ options=version(2) class SHiP::StrawTubesHit+;
#pragma link C++ class std::vector<SHiP::StrawTubesHit>+;
#pragma link C++ options=version(2) class SHiP::CaloHit+;
#pragma link C++ class std::vector<SHiP::CaloHit>+;

// Reconstruction
#pragma link C++ options=version(2) class SHiP::TrackFitResult+;
#pragma link C++ class std::vector<SHiP::TrackFitResult>+;
#pragma link C++ options=version(2) class SHiP::RecParticle+;
#pragma link C++ class std::vector<SHiP::RecParticle>+;
#endif
