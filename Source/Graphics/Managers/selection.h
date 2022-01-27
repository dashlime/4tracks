#ifndef GRAPHICS_SELECTION_H
#define GRAPHICS_SELECTION_H

#include "Graphics/Components/track.h"
#include "Graphics/Components/clip.h"
#include "Graphics/Overlays/selectionoverlay.h"

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
    [[nodiscard]] QVector<QPointer<Track>> getSelectedTracks() const;

    void addClipToSelection(QPointer<Clip> newClip);
    [[nodiscard]] QVector<QPointer<Clip>> getSelectedClips() const;

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
    QVector<QPointer<Track>> mTracksSelected;
    QVector<QPointer<Clip>> mClipsSelected;
    SelectionArea mSelectedArea;
    Callback *mCallback = nullptr;
};

} // namespace Graphics

#endif // GRAPHICS_SELECTION_H
