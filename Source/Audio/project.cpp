#include "project.h"

namespace Audio
{

Project::Project(const QString &projectName)
    : mMixerAudioSource(DEFAULT_SAMPLE_RATE), mProjectName(projectName)
{}

QString Project::getProjectName() const
{
    return mProjectName;
}

void Project::setProjectName(const QString &newProjectName)
{
    mProjectName = newProjectName;
    projectNameChanged();
}

void Project::addTrack(const std::shared_ptr<Track> &newTrack)
{
    mTracks.push_back(newTrack);
    mMixerAudioSource.addInputSource(newTrack.get(), false);

    if (trackAdded != nullptr)
        trackAdded();
}

std::vector<std::shared_ptr<Track>> Project::getTracks() const
{
    return mTracks;
}

std::shared_ptr<Track> Project::getTrackByIndex(int trackIndex) const
{
    for (auto track: mTracks)
        if (track->getIndex() == trackIndex)
            return track;

    return {};
}

bool Project::canRemoveTrack() const
{
    return mTracks.size() > 1;
}

void Project::removeTrack(std::shared_ptr<Track> &trackToRemove)
{
    if (!canRemoveTrack())
        return;

    auto it = mTracks.begin();
    for (auto& track: mTracks) {
        if (track == trackToRemove) {
            mMixerAudioSource.removeInputSource(trackToRemove.get());
            trackToRemove.reset();

            mTracks.erase(it);

            rearrangeTrackIndexes();

            if (trackRemoved != nullptr)
                trackRemoved();

            return;
        }
        it++;
    }
}

void Project::clearAllTracks()
{
    mMixerAudioSource.removeAllInputs();
    mTracks.clear();
}

void Project::rearrangeTrackIndexes()
{
    int max = 0;
    for (const auto &track: mTracks)
        if (track->getIndex() > max)
            max = track->getIndex();

    int lastTrackIndex = -1;

    while (lastTrackIndex < (signed) mTracks.size() - 1) {
        int min = max;
        for (const auto &track: mTracks)
            if (track->getIndex() < min && (signed) track->getIndex() > lastTrackIndex)
                min = track->getIndex();

        getTrackByIndex(min)->setIndex(lastTrackIndex + 1);
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

void Project::updateSavedState(int newSavedState)
{
    mSavedState = newSavedState;
    savedStateChanged();
}

int Project::getSavedState() const
{
    return mSavedState;
}

void Project::setBpm(double newBpm, bool propagateEvent)
{
    mBpm = newBpm;
    if (propagateEvent)
        bpmChanged();
}

double Project::getBpm() const
{
    return mBpm;
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
