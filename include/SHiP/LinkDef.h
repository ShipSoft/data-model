#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// MC / generation
#pragma link C++ class SHiP::MCParticle+;
#pragma link C++ class std::vector<SHiP::MCParticle>+;

// Simulation
#pragma link C++ class SHiP::SimHit+;
#pragma link C++ class SHiP::SimParticle+;
#pragma link C++ class SHiP::SimResult+;
#pragma link C++ class std::vector<SHiP::SimHit>+;
#pragma link C++ class std::vector<SHiP::SimParticle>+;

// Digitisation
#pragma link C++ class SHiP::RecHit+;
#pragma link C++ class std::vector<SHiP::RecHit>+;
#pragma link C++ class SHiP::UBTHit+;
#pragma link C++ class std::vector<SHiP::UBTHit>+;
#pragma link C++ class SHiP::SBTHit+;
#pragma link C++ class std::vector<SHiP::SBTHit>+;
#pragma link C++ class SHiP::TimeDetHit+;
#pragma link C++ class std::vector<SHiP::TimeDetHit>+;
#pragma link C++ class SHiP::StrawTubesHit+;
#pragma link C++ class std::vector<SHiP::StrawTubesHit>+;
#pragma link C++ class SHiP::CaloHit+;
#pragma link C++ class std::vector<SHiP::CaloHit>+;

// Reconstruction
#pragma link C++ class SHiP::TrackFitResult+;
#pragma link C++ class std::vector<SHiP::TrackFitResult>+;
#pragma link C++ class SHiP::RecParticle+;
#pragma link C++ class std::vector<SHiP::RecParticle>+;
#endif
