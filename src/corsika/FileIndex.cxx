#include <corsika/FileIndex.h>
#include <sstream>

using namespace corsika;

std::string FileIndex::String()
{
    std::ostringstream str;
    str << "file index" << std::endl;
    for (unsigned int i = 0; i != eventHeaders.size(); ++i)
    {
        str << i << ", header " << eventHeaders[i] << ", trailer " << eventTrailers[i];
        if (longBlocks.size())
            str << ", long " << longBlocks[i];
        str<< std::endl;
    }
    return str.str();
}



template <class Thinning> void Scan(FileIndex& index, RawStream& stream, bool force)
{
    if (!force && !stream.IsSeekable()) return;
    
    size_t startingBlockNumber = stream.GetNextPosition();
    if (stream.IsSeekable())
        stream.SeekTo(0);
    
    int eventsSoFar = 0;
    
    unsigned int blockIndex = 0;
    bool foundEventHeader = false;
    bool foundRunHeader = false;
    bool foundLongBlock = false;
    
    Block<Thinning> blockUnth;
    while (stream.GetNextBlock(blockUnth) && !blockUnth.IsRunTrailer())
    {
        ++blockIndex;
        if (blockUnth.IsEventHeader())
        {
            foundEventHeader = true;
            foundLongBlock = false;
            size_t rawPosition = stream.GetNextPosition();
            index.eventHeaders.push_back(rawPosition - 1);
            index.IDToPosition[int(blockUnth.AsEventHeader.fEventNumber)] = eventsSoFar;
            ++eventsSoFar;
        }
        else if (blockUnth.IsEventTrailer())
        {
            index.eventTrailers.push_back(stream.GetNextPosition() - 1);
        }
        else if (blockUnth.IsRunHeader())
        {
            foundRunHeader = true;
            index.runNumber = int(blockUnth.AsRunHeader.fRunNumber);
        }
        else if (!foundLongBlock && blockUnth.IsLongitudinal())
        {
            foundLongBlock = true;
            index.longBlocks.push_back(stream.GetNextPosition() - 1);
        }
        if (blockIndex >400 && !foundRunHeader)
            throw IOException("Error scanning Corsika ground file: could not find run header");
        
        if (blockIndex >400 && !foundEventHeader) break;
    }
    
    if (!blockUnth.IsRunTrailer())
    {
        std::ostringstream msg;
        msg << "Error scanning Corsika ground file: could not find run end.";
        if (foundEventHeader)
            msg << std::endl << "Found " << eventsSoFar << " event headers.";
        
        throw IOException(msg.str());
    }
    
    if (index.eventHeaders.size() != index.eventTrailers.size())
        throw IOException("Found different number of event-headers and -trailers");
    
    if (index.longBlocks.size() > 0 && index.eventHeaders.size() != index.longBlocks.size())
    {
        std::ostringstream msg;
        msg << "Found different number of event-headers and longitudinal blocks ("
        << index.eventHeaders.size() << " != " << index.longBlocks.size() << ")";
        throw IOException(msg.str());
    }
    
    if (stream.IsSeekable())
        stream.SeekTo(startingBlockNumber);
}

void FileIndex::Scan(RawStream& stream, bool force)
{
    if (stream.IsThinned()) ::Scan<Thinned>(*this, stream, force);
    else ::Scan<NotThinned>(*this, stream, force);
}

