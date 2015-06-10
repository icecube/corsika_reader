#include <boost/python.hpp>
#include <corsika/CorsikaBlock.h>
#include <vector>

using namespace boost::python;
using namespace corsika;



template <class Thinning>
struct register_block_helper {
  static dict
  BlockBuffer_array_interface(Corsika::Block<Thinning>& self) {
    dict d;
    const int s = Thinning::kWordsPerSubBlock;
    d["shape"] = make_tuple(s);
    d["typestr"] = str("<f4");
    d["data"] = make_tuple((long long)(self.Data()), false);
    return d;
  }
};


struct expose_arrays {
  static
  object
  EventHeader_observation_levels(Corsika::EventHeader& self)
  {
    std::vector<double> a(self.fObservationLevels);
    for (int i = 0; i < self.fObservationLevels; ++i) {
      a[i] = self.fObservationHeight[i];
    }
    object numpy = import("numpy");
    object asarray = numpy.attr("asarray");
    return asarray(object(a));
  }
};


template <class Thinning>
struct register_block_data
{
  static object
  BlockBuffer_data(Corsika::Block<Thinning>& self) {
    object numpy = import("numpy");
    object asarray = numpy.attr("asarray");
    return asarray(object(self));
  }
};

template <class Thinning>
struct register_block {
  static void
  go(std::string name)
  {
    typedef Corsika::Block<Thinning> Block;
    scope block_scope =
      class_<Block >(name.c_str())
      .add_property("__array_interface__", &register_block_helper<corsika::Corsika::Thinned>::BlockBuffer_array_interface)
      .add_property("data", &register_block_data<corsika::Corsika::Thinned>::BlockBuffer_data)
      .add_property("is_run_header", &Block::IsRunHeader)
      .add_property("is_run_trailer", &Block::IsRunTrailer)
      .add_property("is_eventHeader", &Block::IsEventHeader)
      .add_property("is_event_trailer", &Block::IsEventTrailer)
      .add_property("is_longitudinal", &Block::IsLongitudinal)
      .add_property("is_control", &Block::IsControl)
      // .def("AsRunHeader", &Block::AsRunHeader)
      // .def("AsRunTrailer", &Block::AsRunTrailer)
      // .def("AsEventHeader", &Block::AsEventHeader)
      // .def("AsEventTrailer", &Block::AsEventTrailer)
      // .def("AsLongitudinal", &Block::AsLongitudinal)
      // .def("AsParticleBlock", &Block::AsParticleBlock)
      // .def("AsCherenkovBlock", &Block::AsCherenkovBlock)
      .def("ID", &Block::ID)
      ;
    typedef typename Block::ParticleData ParticleData;
    class_<ParticleData >("ParticleData")
      .def("__str__",&ParticleData::String)
      .def_readonly("description", &ParticleData::fDescription)
      .def_readonly("px", &ParticleData::fPx)
      .def_readonly("py", &ParticleData::fPy)
      .def_readonly("pz", &ParticleData::fPz)
      .def_readonly("x", &ParticleData::fX)
      .def_readonly("y", &ParticleData::fY)
      .def_readonly("t_or_z", &ParticleData::fTorZ)
      .def_readonly("weight", &ParticleData::fWeight)
      ;
  }
};

template<>
struct register_block<corsika::Corsika::NotThinned> {
  static void
  go(std::string name)
  {
    typedef Corsika::Block<corsika::Corsika::NotThinned> Block;
    scope block_scope =
      class_<Block >(name.c_str())
      .add_property("__array_interface__", &register_block_helper<corsika::Corsika::NotThinned>::BlockBuffer_array_interface)
      .add_property("data", &register_block_data<corsika::Corsika::NotThinned>::BlockBuffer_data)
      .add_property("is_run_header", &Block::IsRunHeader)
      .add_property("is_run_trailer", &Block::IsRunTrailer)
      .add_property("is_eventHeader", &Block::IsEventHeader)
      .add_property("is_event_trailer", &Block::IsEventTrailer)
      .add_property("is_longitudinal", &Block::IsLongitudinal)
      .add_property("is_control", &Block::IsControl)
      // .def("AsRunHeader", &Block::AsRunHeader)
      // .def("AsRunTrailer", &Block::AsRunTrailer)
      // .def("AsEventHeader", &Block::AsEventHeader)
      // .def("AsEventTrailer", &Block::AsEventTrailer)
      // .def("AsLongitudinal", &Block::AsLongitudinal)
      // .def("AsParticleBlock", &Block::AsParticleBlock)
      // .def("AsCherenkovBlock", &Block::AsCherenkovBlock)
      .add_property("ID", &Block::ID)
      ;
    typedef typename Block::ParticleData ParticleData;
    class_<ParticleData >("ParticleData")
      .def("__str__",&ParticleData::String)
      .add_property("is_particle", &ParticleData::IsParticle)
      .add_property("is_nucleus", &ParticleData::IsNucleus)
      .add_property("is_cherenkov", &ParticleData::IsCherenkov)
      .def_readonly("description", &ParticleData::fDescription)
      .def_readonly("px", &ParticleData::fPx)
      .def_readonly("py", &ParticleData::fPy)
      .def_readonly("pz", &ParticleData::fPz)
      .def_readonly("x", &ParticleData::fX)
      .def_readonly("y", &ParticleData::fY)
      .def_readonly("t_or_z", &ParticleData::fTorZ)
      ;
  }
};

struct register_subblocks {
  static void
  go()
  {
    typedef Corsika::EventHeader EventHeader;
    class_<EventHeader>("EventHeader")
      .def_readonly("shower_number", &EventHeader::fEventNumber)
      .def_readonly("primary_id", &EventHeader::fParticleId)
      .def_readonly("energy", &EventHeader::fEnergy)
      .def_readonly("start_depth", &EventHeader::fStartingAltitude)
      .def_readonly("first_target", &EventHeader::fFirstTarget)
      .def_readonly("z_first_interaction", &EventHeader::fZFirst)
      .def_readonly("Px", &EventHeader::fPx)
      .def_readonly("Py", &EventHeader::fPy)
      .def_readonly("Pz", &EventHeader::fPz)
      .def_readonly("zenith", &EventHeader::fTheta)
      .def_readonly("azimuth", &EventHeader::fPhi)
      // FieldType fRandomSequences;
      // struct {
      // 	FieldType fSeed;
      // 	FieldType fInitialCallsMod;
      // 	FieldType fInitialCallsDiv;
      // } fSeeds[10];
      .def_readonly("run_number", &EventHeader::fRunNumber)
      .def_readonly("date_start", &EventHeader::fDateStart)
      .def_readonly("version", &EventHeader::fVersion)
      .def_readonly("n_observation_levels", &EventHeader::fObservationLevels)
      .add_property("observation_levels", &expose_arrays::EventHeader_observation_levels)
      .def_readonly("spectral_slope", &EventHeader::fSpectralSlope)
      .def_readonly("E_min", &EventHeader::fEMin)
      .def_readonly("E_max", &EventHeader::fEMax)
      .def_readonly("cutoff_hadrons", &EventHeader::fCutoffHadrons)
      .def_readonly("cutoff_muons", &EventHeader::fCutoffMuons)
      .def_readonly("cutoff_electrons", &EventHeader::fCutoffElectrons)
      .def_readonly("cutoff_photons", &EventHeader::fCutoffPhotons)
      .def_readonly("HDPM_NFLAIN", &EventHeader::fNFLAIN)
      .def_readonly("HDPM_NFLDIF", &EventHeader::fNFLDIF)
      .def_readonly("HDPM_NFLPI0", &EventHeader::fNFLPI0)
      .def_readonly("HDPM_NFLPIF", &EventHeader::fNFLPIF)
      .def_readonly("HDPM_NFLCHE", &EventHeader::fNFLCHE)
      .def_readonly("HDPM_NFRAGM", &EventHeader::fNFRAGM)
      .def_readonly("Bx", &EventHeader::fBx)
      .def_readonly("Bz", &EventHeader::fBz)
      .def_readonly("flag_EGS4", &EventHeader::fFlagEGS4)
      .def_readonly("flag_NKG", &EventHeader::fFlagNKG)
      .def_readonly("flag_low_energy_model", &EventHeader::fFlagLowEnergyModel)
      .def_readonly("flag_high_energy_model", &EventHeader::fFlagHighEnergyModel)
      .def_readonly("flag_cherenkov", &EventHeader::fFlagCerenkov)
      .def_readonly("flag_neutrino", &EventHeader::fFlagNeutrino)
      .def_readonly("flag_curved", &EventHeader::fFlagCurved)
      .def_readonly("flag_computer", &EventHeader::fFlagComputer)
      .def_readonly("theta_min", &EventHeader::fThetaMin)
      .def_readonly("theta_max", &EventHeader::fThetaMax)
      .def_readonly("phi_min", &EventHeader::fPhiMin)
      .def_readonly("phi_max", &EventHeader::fPhiMax)
      .def_readonly("cherenkov_bunch_size", &EventHeader::fCerenkovBunch)
      .def_readonly("cherenkov_det_x", &EventHeader::fCerenkovNumberX)
      .def_readonly("cherenkov_det_y", &EventHeader::fCerenkovNumberY)
      .def_readonly("cherenkov_det_dx", &EventHeader::fCerenkovGridX)
      .def_readonly("cherenkov_det_dy", &EventHeader::fCerenkovGridY)
      .def_readonly("cherenkov_det_size_x", &EventHeader::fCerenkovDetectorX)
      .def_readonly("cherenkov_det_size_y", &EventHeader::fCerenkovDetectorY)
      .def_readonly("flag_cherenkov_output", &EventHeader::fCerenkovOutputFlag)
      .def_readonly("array_rotation", &EventHeader::fArrayRotation)
      .def_readonly("flag_muaddi", &EventHeader::fFlagExtraMuonInformation)
      .def_readonly("egs4_step", &EventHeader::fMultipleScatteringStep)
      .def_readonly("cherenkov_wavelength_min", &EventHeader::fCerenkovBandwidthMin)
      .def_readonly("cherenkov_wavelength_max", &EventHeader::fCerenkovBandwidthMax)
      .def_readonly("cherenkov_event_uses", &EventHeader::fUsersOfEvent)
      // .def_readonly("", &EventHeader::fCoreX[20])
      // .def_readonly("", &EventHeader::fCoreY[20])
      .def_readonly("flag_sibyll", &EventHeader::fFlagSIBYLL)
      .def_readonly("flag_sibyll_xsection", &EventHeader::fFlagSIBYLLCross)
      .def_readonly("flag_qgsjet", &EventHeader::fFlagQGSJET)
      .def_readonly("flag_qgsjet_xsection", &EventHeader::fFlagQGSJETCross)
      .def_readonly("flag_dpmjet", &EventHeader::fFlagDPMJET)
      .def_readonly("flag_dpmjet_xsection", &EventHeader::fFlagDPMJETCross)
      .def_readonly("flag_other_x_sect", &EventHeader::fFlagVENUSCross)
      .def_readonly("flag_muon_mult_scat", &EventHeader::fFlagMuonMultiple)
      .def_readonly("nkg_radial_range", &EventHeader::fNKGRadialRange)
      .def_readonly("thinning_had", &EventHeader::fEFractionThinningH)
      .def_readonly("thinning_em", &EventHeader::fEFractionThinningEM)
      .def_readonly("thinning_w_max_had", &EventHeader::fWMaxHadronic)
      .def_readonly("thinning_w_max_em", &EventHeader::fWMaxEM)
      .def_readonly("thinning_r_max", &EventHeader::fRMaxThinning)
      .def_readonly("viewcone_min_angle", &EventHeader::fInnerAngle)
      .def_readonly("viewcone_max_angle", &EventHeader::fOuterAngle)
      .def_readonly("high_low_transition", &EventHeader::fHighLowEnergyTransition)
      .def_readonly("skimming_incidence", &EventHeader::fSkimmingIncidence)
      .def_readonly("skimming_altitude", &EventHeader::fSkimmingAltitude)
      .def_readonly("starting_height", &EventHeader::fStartingHeight)
      .def_readonly("flag_charm", &EventHeader::fCharmON)
      ;
  }

  };


void register_CorsikaBlock()
{
  register_block<Corsika::NotThinned>::go("Block");
  register_block<Corsika::Thinned>::go("Block_thin");

  register_subblocks::go();
}
