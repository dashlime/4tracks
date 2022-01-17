#ifndef GRAPHICS_SELECTION_H
#define GRAPHICS_SELECTION_H

#include "track.h"
#include "clip.h"
#include "selectionoverlay.h"

namespace Graphics
{

class Selection
{
public:
    enum SelectionType
    {
        NoSelection, TracksSelected, ClipsSelected, AreaSelected
    };
    class Callback
    {
    public:
        Callback() = default;

        virtual void selectionChanged() = 0;
    };

    Selection();

    void setSelectionCallback(Callback *callback);

    void setSelectionType(SelectionType type);
    [[nodiscard]] SelectionType getSelectionType() const;

    void addTrackToSelection(QPointer<Track> newTrack);
    [[nodiscard]] std::vector<QPointer<Track>> getSelectedTracks() const;

    void addClipToSelection(QPointer<Clip> newClip);
    [[nodiscard]] std::vector<QPointer<Clip>> getSelectedClips() const;

    struct SelectionArea
    {
        int startTrackIndex;
        int startSample;
        int nbTracks;
        int nbSamples;
    };

    void setSelectedArea(int startTrackIndex, int startSample, int nbTracks, int nbSamples);
    void setSelectedArea(SelectionArea area);

    [[nodiscard]] SelectionArea getSelectedArea() const;

    void clearSelection();

private:
    SelectionType mSelectionType;
    std::vector<QPointer<Track>> mTracksSelected;
    std::vector<QPointer<Clip>> mClipsSelected;
    SelectionArea mSelectedArea;
    Callback *mCallback = nullptr;
};

} // namespace Graphics

#endif // GRAPHICS_SELECTION_H
