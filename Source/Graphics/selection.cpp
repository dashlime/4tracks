#include "selection.h"

namespace Graphics
{

Selection::Selection()
{}

void Selection::setSelectionCallback(Callback *callback)
{
    mCallback = callback;
}

void Selection::setSelectionType(SelectionType type)
{
    mSelectionType = type;

    clearSelection();

    if (mCallback != nullptr)
        mCallback->selectionChanged();
}

Selection::SelectionType Selection::getSelectionType() const
{
    return mSelectionType;
}

void Selection::addTrackToSelection(QPointer<Track> newTrack)
{
    if (mSelectionType != TracksSelected)
        return;

    mTracksSelected.push_back(newTrack);
    newTrack->setSelected(true);

    if (mCallback != nullptr)
        mCallback->selectionChanged();
}

std::vector<QPointer<Track>> Selection::getSelectedTracks() const
{
    return mTracksSelected;
}

void Selection::addClipToSelection(QPointer<Clip> newClip)
{
    if (mSelectionType != ClipsSelected)
        return;

    mClipsSelected.push_back(newClip);
    newClip->setSelected(true);

    if (mCallback != nullptr)
        mCallback->selectionChanged();
}

std::vector<QPointer<Clip>> Selection::getSelectedClips() const
{
    return mClipsSelected;
}

void Selection::setSelectedArea(int startTrackIndex, int startSample, int nbTracks, int nbSamples)
{
    if (mSelectionType != AreaSelected)
        return;

    mSelectedArea.startTrackIndex = startTrackIndex;
    mSelectedArea.startSample = startSample;
    mSelectedArea.nbTracks = nbTracks;
    mSelectedArea.nbSamples = nbSamples;

    if (mCallback != nullptr)
        mCallback->selectionChanged();
}

void Selection::setSelectedArea(SelectionArea area)
{
    if (mSelectionType != AreaSelected)
        return;

    mSelectedArea = area;

    if (mCallback != nullptr)
        mCallback->selectionChanged();
}

Selection::SelectionArea Selection::getSelectedArea() const
{
    return mSelectedArea;
}

void Selection::clearSelection()
{
    for (auto track : mTracksSelected)
        track->setSelected(false);

    mTracksSelected.clear();

    for (auto clip : mClipsSelected)
        clip->setSelected(false);

    mClipsSelected.clear();

    mSelectedArea = SelectionArea();
}

} // namespace Graphics
