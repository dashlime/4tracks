#include "audiotrack.h"

#include "QDebug"

namespace Audio {

AudioTrack::AudioTrack(QString name, int index) : mName(name), mIndex(index)
{

}

int AudioTrack::getType() const
{
    if (mClips.size() == 0)
        return ANY_TRACK;

    return mClips.at(0)->getType() == Clip::AUDIO_CLIP ? AUDIO_TRACK : MIDI_TRACK;
}

bool AudioTrack::addClip(std::shared_ptr<Clip> clip)
{
    if (clip->getType() == Clip::AUDIO_CLIP && getType() == AUDIO_TRACK)
    {
        mClips.append(clip);
        resizeClipsWhenClipAdded(mClips.size() - 1);
    }
    else if (clip->getType() == Clip::MIDI_CLIP && getType() == MIDI_TRACK)
    {
        mClips.append(clip);
        resizeClipsWhenClipAdded(mClips.size() - 1);
    }
    else if (getType() == ANY_TRACK)
        mClips.append(clip);
    else
        return false;

    return true;
}

QString AudioTrack::getName() const
{
    return mName;
}

int AudioTrack::getIndex() const
{
    return mIndex;
}

void AudioTrack::setIndex(int newPosition)
{
    mIndex = newPosition;
}

void AudioTrack::resizeClipsWhenClipAdded(int newClipIndex)
{

}

QVector<std::shared_ptr<Clip>> AudioTrack::getClips()
{
    return mClips;
}

void AudioTrack::setNextReadPosition(juce::int64 newPosition)
{
    mPositionInSamples = newPosition;
    mClipPlaying = nullptr;

    for (auto clip : mClips) {
        if (clip->getPositionInSamples() <= mPositionInSamples && clip->getPositionInSamples() + clip->getLengthInSamples() >= mPositionInSamples) {
            mClipPlaying = clip;
            break;
        }
    }
}

juce::int64 AudioTrack::getNextReadPosition() const
{
    return mPositionInSamples;
}

juce::int64 AudioTrack::getTotalLength() const
{
    int max = 0;
    for (auto clip : mClips) {
        max = juce::jmax(clip->getPositionInSamples() + clip->getLengthInSamples(), max);
    }

    return max;
}

bool AudioTrack::isLooping() const
{
    return false;
}

void AudioTrack::setLooping(bool shouldLoop) {}

void AudioTrack::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mPositionInSamples = 0;
    if (getType() == AUDIO_TRACK)
    {
        for (auto clip : mClips)
        {
            std::dynamic_pointer_cast<AudioClip>(clip)->prepareToPlay(samplesPerBlockExpected, sampleRate);
        }
    }
}

void AudioTrack::releaseResources()
{}

void AudioTrack::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    if (mClipPlaying == nullptr)
        bufferToFill.clearActiveBufferRegion();


    else {
        if (getType() == AUDIO_TRACK)
        {
            std::dynamic_pointer_cast<AudioClip>(mClipPlaying)->getNextAudioBlock(bufferToFill);
        }
    }

    setNextReadPosition(getNextReadPosition() + bufferToFill.numSamples);
}

}
