#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// I/O customization rules keep files written before the snake_case field
// renames (data-model <= v0.4.0) fully readable: each rule maps an on-disk
// camelCase member to its renamed in-memory member. The rules are scoped to
// the checksums of the historical class layouts (TClass::GetCheckSum, taken
// from tests/data/schema_snapshot.txt as of v0.4.0 and from the frozen
// reference files for earlier layouts) so they are only considered for
// matching on-disk data; version matching cannot be used because the
// pre-v0.5.0 structs were unversioned and report class version -1. If a
// class layout gains another era, append its checksum to the rule list —
// the compat suite (tests/data/README.md) fails if a rule misses an era
// covered by a frozen reference file.
//
// KNOWN ISSUE (root-project/root#23146): ROOT 6.40.02 applies these rules
// correctly when reading TTree data, but misapplies them when reading
// RNTuple data written by UNVERSIONED classes. Per the workaround proposed
// upstream, all classes now carry an explicit ClassDefNV version and
// readers must open files through TFile before attaching the
// RNTupleReader; rules then work for all data written from v0.5.0 on.
// Data written by the unversioned pre-v0.5.0 classes still cannot be
// rule-read (ROOT aborts on an internal assertion), so the compat_read_v*
// tests are EXPECTED to fail until ROOT supports reading unversioned data
// into versioned classes. They assert the true values so the fix is
// detected the moment it arrives; do not "fix" the tests.

// Event metadata
#pragma link C++ class SHiP::EventHeader+;

// MC / generation
#pragma link C++ class SHiP::MCParticle+;
#pragma link C++ class std::vector<SHiP::MCParticle>+;
// MCParticle layout eras: v0.1.0-v0.4.0 = 387412058 (0x1717705a)
#pragma read sourceClass="SHiP::MCParticle" targetClass="SHiP::MCParticle" checksum="[387412058]" source="std::int32_t pdgCode; std::int32_t motherId" target="pdg_code,mother_id" code="{ pdg_code = onfile.pdgCode; mother_id = onfile.motherId; }"

// Simulation
#pragma link C++ class SHiP::SimHit+;
#pragma link C++ class SHiP::SimParticle+;
#pragma link C++ class SHiP::SimResult+;
#pragma link C++ class std::vector<SHiP::SimHit>+;
#pragma link C++ class std::vector<SHiP::SimParticle>+;
// SimHit layout eras: v0.1.0-v0.3.0 = 3449694654 (0xcd9e29be),
//                     v0.4.0 (adds geometryNodeId) = 3911017334 (0xe91d6376)
#pragma read sourceClass="SHiP::SimHit" targetClass="SHiP::SimHit" checksum="[3449694654,3911017334]" source="std::int32_t detectorId; std::int32_t trackId; std::int32_t pdgCode; double energyDeposit; double pathLength" target="detector_id,track_id,pdg_code,energy_deposit,path_length" code="{ detector_id = onfile.detectorId; track_id = onfile.trackId; pdg_code = onfile.pdgCode; energy_deposit = onfile.energyDeposit; path_length = onfile.pathLength; }"
#pragma read sourceClass="SHiP::SimHit" targetClass="SHiP::SimHit" checksum="[3911017334]" source="std::int32_t geometryNodeId" target="geometry_node_id" code="{ geometry_node_id = onfile.geometryNodeId; }"
// SimParticle layout eras: v0.1.0-v0.4.0 = 4053157024 (0xf19644a0)
#pragma read sourceClass="SHiP::SimParticle" targetClass="SHiP::SimParticle" checksum="[4053157024]" source="std::int32_t trackId; std::int32_t parentId; std::int32_t pdgCode; std::int32_t creatorProcess" target="track_id,parent_id,pdg_code,creator_process" code="{ track_id = onfile.trackId; parent_id = onfile.parentId; pdg_code = onfile.pdgCode; creator_process = onfile.creatorProcess; }"

// Digitisation
#pragma link C++ class SHiP::RecHit+;
#pragma link C++ class std::vector<SHiP::RecHit>+;
// RecHit layout eras: v0.3.0-v0.4.0 = 3772464393 (0xe0db3d09)
#pragma read sourceClass="SHiP::RecHit" targetClass="SHiP::RecHit" checksum="[3772464393]" source="std::int32_t detectorId; std::int32_t trackId; std::int32_t pdgCode; double energyDeposit; double pathLength" target="detector_id,track_id,pdg_code,energy_deposit,path_length" code="{ detector_id = onfile.detectorId; track_id = onfile.trackId; pdg_code = onfile.pdgCode; energy_deposit = onfile.energyDeposit; path_length = onfile.pathLength; }"
// The detector wrappers' recHit -> rec_hit rename is deliberately NOT
// covered by a rule: an object-typed rule source (SHiP::RecHit recHit)
// whose class itself has rules triggers a double free in ROOT 6.40.02
// (TTree path; nested staging). No wrapper data has been persisted so far.
// Add the rules once ROOT handles nested-object rule sources.
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
// TrackFitResult layout eras: v0.3.0-v0.4.0 = 3517659236 (0xd1ab3864)
#pragma read sourceClass="SHiP::TrackFitResult" targetClass="SHiP::TrackFitResult" checksum="[3517659236]" source="std::int32_t nMeas; std::int32_t fitStatus; double qoverp; std::array<double,3> refLoc; std::vector<double> inputMeasurementsX; std::vector<double> inputMeasurementsY; std::vector<double> fittedMeasurementsX; std::vector<double> fittedMeasurementsY; std::vector<double> residualsX; std::vector<double> residualsY" target="n_meas,fit_status,q_over_p,ref_loc,input_measurements_x,input_measurements_y,fitted_measurements_x,fitted_measurements_y,residuals_x,residuals_y" code="{ n_meas = onfile.nMeas; fit_status = onfile.fitStatus; q_over_p = onfile.qoverp; ref_loc = onfile.refLoc; input_measurements_x = onfile.inputMeasurementsX; input_measurements_y = onfile.inputMeasurementsY; fitted_measurements_x = onfile.fittedMeasurementsX; fitted_measurements_y = onfile.fittedMeasurementsY; residuals_x = onfile.residualsX; residuals_y = onfile.residualsY; }"
#pragma link C++ class SHiP::RecParticle+;
#pragma link C++ class std::vector<SHiP::RecParticle>+;
// RecParticle layout eras: v0.1.0-v0.2.0 = 2059331417 (0x7abee759),
//                          v0.3.0-v0.4.0 (adds hits) = 3910067879 (0xe90ee6a7)
#pragma read sourceClass="SHiP::RecParticle" targetClass="SHiP::RecParticle" checksum="[2059331417,3910067879]" source="std::int32_t trackId; std::int32_t parentId; std::int32_t pdgCode; std::int32_t creatorProcess; double ipPV" target="track_id,parent_id,pdg_code,creator_process,ip_pv" code="{ track_id = onfile.trackId; parent_id = onfile.parentId; pdg_code = onfile.pdgCode; creator_process = onfile.creatorProcess; ip_pv = onfile.ipPV; }"
#endif
