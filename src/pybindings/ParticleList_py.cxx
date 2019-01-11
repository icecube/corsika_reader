#include <boost/python.hpp>
#include <sstream>
#include <corsika/particle/ParticleList.h>
#include <corsika/Particle.h>

using namespace boost::python;
using namespace corsika;

void register_ParticleList()
{
  scope block_scope =
    class_<ParticleList>("ParticleList")
    .def("__call__", &ParticleList::operator(), return_internal_reference<>())
    .def("name_from_corsika", &ParticleList::NameFromCorsika)
    .staticmethod("name_from_corsika")
    .def("name_from_pdg", &ParticleList::NameFromPDG)
    .staticmethod("name_from_pdg")
    .def("corsika_to_pdg", &ParticleList::CorsikaToPDG)
    .staticmethod("corsika_to_pdg")
    .def("get", &ParticleList::Get, return_internal_reference<>())
    .staticmethod("get")
    ;

  // enum_<CorsikaParticle::Type>("Type")
  //   .value("eUndefined"        , CorsikaParticle::eUndefined)
  //   .value("eElectron"         , CorsikaParticle::eElectron)
  //   .value("ePositron"         , CorsikaParticle::ePositron)
  //   .value("eNuElectron"       , CorsikaParticle::eNuElectron)
  //   .value("eAntiNuElectron"   , CorsikaParticle::eAntiNuElectron)
  //   .value("eMuon"             , CorsikaParticle::eMuon)
  //   .value("eAntiMuon"         , CorsikaParticle::eAntiMuon)
  //   .value("eNuMuon"           , CorsikaParticle::eNuMuon)
  //   .value("eAntiNuMuon"       , CorsikaParticle::eAntiNuMuon)
  //   .value("eTau"              , CorsikaParticle::eTau)
  //   .value("eAntiTau"          , CorsikaParticle::eAntiTau)
  //   .value("eNuTau"            , CorsikaParticle::eNuTau)
  //   .value("eAntiNuTau"        , CorsikaParticle::eAntiNuTau)
  //   .value("ePhoton"           , CorsikaParticle::ePhoton)
  //   .value("ePiZero"           , CorsikaParticle::ePiZero)
  //   .value("eRhoZero"          , CorsikaParticle::eRhoZero)
  //   .value("ePiPlus"           , CorsikaParticle::ePiPlus)
  //   .value("ePiMinus"          , CorsikaParticle::ePiMinus)
  //   .value("eRhoPlus"          , CorsikaParticle::eRhoPlus)
  //   .value("eRhoMinus"         , CorsikaParticle::eRhoMinus)
  //   .value("eEta"              , CorsikaParticle::eEta)
  //   .value("eOmegaMeson"       , CorsikaParticle::eOmegaMeson)
  //   .value("eKaon0L"           , CorsikaParticle::eKaon0L)
  //   .value("eKaon0S"           , CorsikaParticle::eKaon0S)
  //   .value("eKaonPlus"         , CorsikaParticle::eKaonPlus)
  //   .value("eKaonMinus"        , CorsikaParticle::eKaonMinus)
  //   .value("eDeltaMinus"       , CorsikaParticle::eDeltaMinus)
  //   .value("eLambda"           , CorsikaParticle::eLambda)
  //   .value("eSigmaPlus"        , CorsikaParticle::eSigmaPlus)
  //   .value("eSigmaZero"        , CorsikaParticle::eSigmaZero)
  //   .value("eSigmaMinus"       , CorsikaParticle::eSigmaMinus)
  //   .value("eAntiSigmaPlus"    , CorsikaParticle::eAntiSigmaPlus)
  //   .value("eAntiSigmaZero"    , CorsikaParticle::eAntiSigmaZero)
  //   .value("eAntiSigmaMinus"   , CorsikaParticle::eAntiSigmaMinus)
  //   .value("eXiZero"           , CorsikaParticle::eXiZero)
  //   .value("eXiMinus"          , CorsikaParticle::eXiMinus)
  //   .value("eAntiXiZero"       , CorsikaParticle::eAntiXiZero)
  //   .value("eAntiXiMinus"      , CorsikaParticle::eAntiXiMinus)
  //   .value("eOmegaMinus"       , CorsikaParticle::eOmegaMinus)
  //   .value("eAntiOmegaMinus"   , CorsikaParticle::eAntiOmegaMinus)
  //   .value("eLambdac"          , CorsikaParticle::eLambdac)
  //   .value("eNeutron"          , CorsikaParticle::eNeutron)
  //   .value("eAntiNeutron"      , CorsikaParticle::eAntiNeutron)
  //   .value("eDeltaZero"        , CorsikaParticle::eDeltaZero)
  //   .value("eProton"           , CorsikaParticle::eProton)
  //   .value("eAntiProton"       , CorsikaParticle::eAntiProton)
  //   .value("eDeltaPlus"        , CorsikaParticle::eDeltaPlus)
  //   .value("eDeltaPlusPlus"    , CorsikaParticle::eDeltaPlusPlus)
  //   .value("eDecayedMuon"      , CorsikaParticle::eDecayedMuon)
  //   .value("eDecayedAntiMuon"  , CorsikaParticle::eDecayedAntiMuon)
  //   .value("eIron"             , CorsikaParticle::eIron)
  //   ;

}
