
set (Corsika_PACKAGE_NAME "corsika")

i3_add_pybindings(${Corsika_PACKAGE_NAME}
  RawCorsikaFile.cxx
  RawParticleIterator.cxx
  CorsikaBlock.cxx
  CorsikaIOException.cxx
  CorsikaUnits.cxx
  MathConstants.cxx
  PhysicalConstants.cxx
  CorsikaUtilities.cxx
  CorsikaParticle.cxx
  CorsikaShower.cxx
  CorsikaShowerFile.cxx
  #GaisserHillas6Parameter.cxx
  CorsikaShowerFileParticleIterator.cxx
  ShowerParticleList.cxx
  ParticleList.cxx
  ParticleProperties.cxx
  Histogram.cxx
  Histogramer.cxx
  MuonHistogramer.cxx
  module.cxx
  USE_TOOLS boost python
  USE_PROJECTS icetop_corsika icetray dataclasses dataio MuonGun
  )
