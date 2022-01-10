#ifndef GRAPHICS_SELECTION_H
#define GRAPHICS_SELECTION_H

#include "track.h"
#include "clip.h"
#include "selectionoverlay.h"

namespace Graphics {

class Selection
{
public:
    enum SelectionType { NoSelection, TracksSelected, ClipsSelected, AreaSelected };
    class Callback
    {
    public:
        Callback() {}

        virtual void selectionChanged() = 0;
    };

    Selection();
    void setSelectionCallback(Callback *callback);

    void setSelectionType(SelectionType type);
    SelectionType getSelectionType() const;

    void addTrackToSelection(std::shared_ptr<Track> newTrack);
    std::vector<std::shared_ptr<Track>> getSelectedTracks() const;

    void addClipToSelection(std::shared_ptr<Clip> newClip);
    std::vector<std::shared_ptr<Clip>> getSelectedClips() const;

    struct SelectionArea
    {
        int startTrackIndex;
        int startSample;
        int nbTracks;
        int nbSamples;
    };

    void setSelectedArea(int startTrackIndex, int startSample, int nbTracks, int nbSamples);
    void setSelectedArea(SelectionArea area);

    SelectionArea getSelectedArea() const;

    void clearSelection();

private:
    SelectionType mSelectionType;
    std::vector<std::shared_ptr<Track>> mTracksSelected;
    std::vector<std::shared_ptr<Clip>> mClipsSelected;
    SelectionArea mSelectedArea;
    Callback *mCallback = nullptr;
};

} // namespace Graphics

#endif // GRAPHICS_SELECTION_H
