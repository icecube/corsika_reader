#include <corsika/CorsikaShowerFileParticleIterator.h>
#include <corsika/particle/ParticleList.h>

using namespace corsika;

CorsikaShowerFileParticleIterator::
CorsikaShowerFileParticleIterator(RawStreamPtr stream, size_t start, double timeOffset, int observationLevel, bool keepMuProd):
    iterator_(VRawParticleIterator::Create(stream, start)), fTimeOffset(timeOffset),
    fObservationLevel(observationLevel), fKeepMuProd(keepMuProd)
{
    Rewind();
}

boost::optional<CorsikaParticle> CorsikaShowerFileParticleIterator::NextParticle()
{
    boost::optional<CorsikaParticle> parent;
    boost::optional<CorsikaParticle> grandparent;
    boost::optional<CorsikaParticle> muaddi;
    while((value_ = iterator_->GetCorsikaParticle()))
    {
        int corsika_particle_id = int(value_->fDescription/1000);
        int particleId = ParticleList::CorsikaToPDG(corsika_particle_id);
        int obsLevel = (unsigned)value_->fDescription % 10;
        if (value_->fDescription < 0) // history particle
        {
            if (!parent) parent = value_;
            else grandparent = value_;
            continue;
        }
        
        if ((corsika_particle_id == 75 || corsika_particle_id == 76)  && !muaddi)
        {
            muaddi = value_;
            continue;
        }
        
        if (particleId == CorsikaParticle::eUndefined || (!fKeepMuProd && (particleId == CorsikaParticle::eDecayedMuon || particleId == CorsikaParticle::eDecayedAntiMuon)) || obsLevel != fObservationLevel)
        {
            // reset and continue
            parent = boost::none;
            grandparent = boost::none;
            muaddi = boost::none;
            continue;
        }
        
        if (grandparent && parent)
        {
            value_->SetParent(parent.get());
            value_->SetGrandParent(grandparent.get());
        }
        if (muaddi) value_->SetMuonInfo(muaddi.get());
        
        // deal with corsika's idiosyncrasies here
        value_->fTorZ -= fTimeOffset;
        
        break;
    }
    return value_;
}
