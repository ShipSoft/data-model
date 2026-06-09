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

// Reconstruction
#pragma link C++ class SHiP::RecParticle+;
#pragma link C++ class std::vector<SHiP::RecParticle>+;
#endif
