#include "project.h"

namespace Audio
{

Project::Project(const QString &projectName)
    : mMixerAudioSource(DEFAULT_SAMPLE_RATE), mProjectProperties(QSharedPointer<ProjectProperties>::create(projectName))
{}

QSharedPointer<ProjectProperties> Project::getProjectProperties() const
{
    return mProjectProperties;
}

void Project::addTrack(const QSharedPointer<Track> &newTrack)
{
    mTracks.push_back(newTrack);
    mMixerAudioSource.addInputSource(newTrack.get(), false);

    connect(newTrack->getTrackProperties().get(),
            &TrackProperties::savedStateChanged,
            [=](ProjectProperties::SavedState newState)
            {
                mProjectProperties->updateSavedState(newState);
            });

    connect(newTrack->getTrackProperties().get(), &TrackProperties::lengthChanged, [=]()
    {
        mProjectProperties->updateTotalLength();
    });

    connect(newTrack.get(), &Track::clipRemoved, [=](const QSharedPointer<Clip> &clipRemoved)
    {
        removeClip(clipRemoved);
    });

    emit trackAdded(newTrack->getTrackProperties()->getIndex());
}

QVector<QSharedPointer<Track>> Project::getTracks() const
{
    return mTracks;
}

bool Project::addClip(const QSharedPointer<Clip> &newClip)
{
    bool result = newClip->getClipProperties()->getParentTrack()->addClip(newClip);
    if (result) {
        mClips.append(newClip);
        emit clipAdded((int) mClips.size() - 1);
    }

    return result;
}

QVector<QSharedPointer<Clip>> Project::getClips() const
{
    return mClips;
}

int Project::createTrack(const QString &trackName)
{
    rearrangeTrackIndexes();

    int newTrackIndex = (int) mTracks.size();

    addTrack(QSharedPointer<Audio::Track>::create(trackName, newTrackIndex, QPointer<Audio::Project>(this)));
    return newTrackIndex;
}

int Project::createAudioClip(const QSharedPointer<Track> &parentTrack, const QString &filePath)
{
    if (parentTrack == nullptr)
        return -1;

    bool result = addClip(QSharedPointer<Audio::AudioClip>::create(filePath, parentTrack.get()));

    if (!result)
        return -1;

    return (int) mClips.size() - 1;
}

int Project::createMIDIClip(const QSharedPointer<Track> &parentTrack)
{
    if (parentTrack == nullptr)
        return -1;

    bool result = addClip(QSharedPointer<Audio::MidiClip>::create(parentTrack.get()));

    if (!result)
        return -1;

    return (int) mClips.size() - 1;
}

QSharedPointer<Track> Project::getTrackByIndex(int trackIndex) const
{
    for (auto track: mTracks)
        if (track->getTrackProperties()->getIndex() == trackIndex)
            return track;

    return {};
}

bool Project::canRemoveTrack() const
{
    return mTracks.size() > 1;
}

bool Project::canRemoveTracks(int tracksNumber) const
{
    return mTracks.size() > tracksNumber;
}

void Project::removeTrack(QSharedPointer<Track> trackToRemove)
{
    auto it = mTracks.begin();
    for (auto &track: mTracks) {
        if (track == trackToRemove) {
            for (auto &clip: trackToRemove->getClips()) {
                removeClip(clip);
            }

            int trackIndex = trackToRemove->getTrackProperties()->getIndex();

            mMixerAudioSource.removeInputSource(trackToRemove.get());
            trackToRemove.reset();
            mTracks.erase(it);

            rearrangeTrackIndexes();

            emit trackRemoved(trackIndex);
            mProjectProperties->updateSavedState(ProjectProperties::UNSAVED);

            return;
        }
        it++;
    }
}

void Project::removeClip(QSharedPointer<Clip> clipToRemove)
{
    auto it = mClips.begin();
    for (auto &clip: mClips) {
        if (clip == clipToRemove) {
            int clipID = (int) std::distance(mClips.begin(), it);

            clipToRemove->getClipProperties()->getParentTrack()->removeClip(clipToRemove);
            clipToRemove.reset();

            mClips.erase(it);

            emit clipRemoved(clipID);
            return;
        }
        it++;
    }
}

void Project::clearAllTracks()
{
    mMixerAudioSource.removeAllInputs();
    for (const auto &track: mTracks) {
        removeTrack(track);
    }
}

void Project::rearrangeTrackIndexes()
{
    int max = 0;
    for (const auto &track: mTracks)
        if (track->getTrackProperties()->getIndex() > max)
            max = track->getTrackProperties()->getIndex();

    int lastTrackIndex = -1;

    while (lastTrackIndex < (signed) mTracks.size() - 1) {
        int min = max;
        for (const auto &track: mTracks)
            if (track->getTrackProperties()->getIndex() < min
                && (signed) track->getTrackProperties()->getIndex() > lastTrackIndex)
                min = track->getTrackProperties()->getIndex();

        getTrackByIndex(min)->getTrackProperties()->setIndex(lastTrackIndex + 1);
        lastTrackIndex++;
    }
}

void Project::play()
{
    mPlaying = true;
}

void Project::pause()
{
    mPlaying = false;
}

void Project::stop()
{
    mPlaying = false;
    mMixerAudioSource.setNextReadPosition(0);
}

bool Project::isPlaying() const
{
    return mPlaying;
}

void Project::setNextReadPosition(juce::int64 newPosition)
{
    mMixerAudioSource.setNextReadPosition(newPosition);
}

juce::int64 Project::getNextReadPosition() const
{
    return mMixerAudioSource.getNextReadPosition();
}

juce::int64 Project::getTotalLength() const
{
    return mMixerAudioSource.getTotalLength();
}

bool Project::isLooping() const
{
    return false;
}

void Project::setLooping(bool)
{}

void Project::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mMixerAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Project::releaseResources()
{
    mMixerAudioSource.releaseResources();
}

void Project::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    if (isPlaying())
        mMixerAudioSource.getNextAudioBlock(bufferToFill);

    else
        bufferToFill.clearActiveBufferRegion();
}

}
