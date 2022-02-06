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

    emit trackAdded(newTrack->getTrackProperties()->getIndex());
}

QVector<QSharedPointer<Track>> Project::getTracks() const
{
    return mTracks;
}

int Project::createTrack(const QString &trackName)
{
    rearrangeTrackIndexes();

    int newTrackIndex = (int) mTracks.size();

    addTrack(QSharedPointer<Audio::Track>::create(trackName, newTrackIndex, QPointer<Audio::Project>(this)));
    return newTrackIndex;
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

void Project::removeTrack(QSharedPointer<Track> trackToRemove)
{
    auto it = mTracks.begin();
    for (auto &track: mTracks) {
        if (track == trackToRemove) {
            int trackIndex = trackToRemove->getTrackProperties()->getIndex();

            mMixerAudioSource.removeInputSource(trackToRemove.get());
            trackToRemove.reset();

            mTracks.erase(it);

            rearrangeTrackIndexes();

            emit trackRemoved(trackIndex);

            return;
        }
        it++;
    }
}

void Project::clearAllTracks()
{
    mMixerAudioSource.removeAllInputs();
    for (const auto& track : mTracks) {
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
