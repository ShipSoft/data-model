/// Compile every public header on its own, in one translation unit.
///
/// Two things depend on this file existing. The obvious one: the library is
/// header-only, so nothing else proves that a header stands up without a
/// particular include order in front of it.
///
/// The less obvious one: clang-tidy only ever sees a header through a
/// translation unit that includes it. SHiPDataModel has no sources of its
/// own, so the compilation database holds the test programs and the generated
/// dictionary - and the dictionary is filtered out of the clang-tidy run.
/// Before this file, TrackFitResult and everything under detectors/ were
/// included by no test, which left the naming convention in
/// include/SHiP/.clang-tidy unchecked for exactly the classes that carry the
/// most renamed members. Add new headers here as they appear.
#include "SHiP/EventHeader.hpp"
#include "SHiP/MCParticle.hpp"
#include "SHiP/QuantityView.hpp"
#include "SHiP/RecHit.hpp"
#include "SHiP/RecParticle.hpp"
#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"
#include "SHiP/SimResult.hpp"
#include "SHiP/TrackFitResult.hpp"
#include "SHiP/Units.hpp"
#include "SHiP/detectors/CaloHit.hpp"
#include "SHiP/detectors/SBTHit.hpp"
#include "SHiP/detectors/StrawTubesHit.hpp"
#include "SHiP/detectors/TimeDetHit.hpp"
#include "SHiP/detectors/UBTHit.hpp"
#include "SHiP/detectors/detector_id.hpp"
