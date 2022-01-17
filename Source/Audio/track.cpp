#include "track.h"

namespace Audio
{

Track::Track(const QString& name, int index)
    : mName(name), mIndex(index)
{}

QString Track::getName() const
{
    return mName;
}

int Track::getType() const
{
    if (mClips.empty())
        return ANY_TRACK;

    return mClips.at(0)->getType() == Clip::AUDIO_CLIP ? AUDIO_TRACK : MIDI_TRACK;
}

bool Track::addClip(const std::shared_ptr<Clip>& clip)
{
    if (clip->getType() == Clip::AUDIO_CLIP && getType() == AUDIO_TRACK) {
        mClips.push_back(clip);
        resizeClipsWhenClipAdded((int) mClips.size() - 1);
    } else if (clip->getType() == Clip::MIDI_CLIP && getType() == MIDI_TRACK) {
        mClips.push_back(clip);
        resizeClipsWhenClipAdded((int) mClips.size() - 1);
    } else if (getType() == ANY_TRACK)
        mClips.push_back(clip);
    else
        return false;

    if (clipAdded != nullptr)
        clipAdded();

    return true;
}

std::vector<std::shared_ptr<Clip>> Track::getClips()
{
    return mClips;
}

int Track::getIndex() const
{
    return mIndex;
}

void Track::setIndex(int newPosition)
{
    mIndex = newPosition;
}

void Track::setVolume(double newVolume)
{
    mVolume = newVolume;
}

double Track::getVolume() const
{
    return mVolume;
}

void Track::setPan(double newPan)
{
    mPan = newPan;
}

double Track::getPan() const
{
    return mPan;
}

void Track::resizeClipsWhenClipAdded(int newClipIndex)
{

}

void Track::setNextReadPosition(juce::int64 newPosition)
{
    mPositionInSamples = newPosition;
    mClipPlaying = nullptr;

    for (const auto& clip : mClips) {
        if (clip->getPositionInSamples() <= mPositionInSamples
            && clip->getPositionInSamples() + clip->getLengthInSamples() >= mPositionInSamples) {
            mClipPlaying = clip;
            break;
        }
    }

    if (mClipPlaying != nullptr)
        mClipPlaying->setReadPosition(newPosition);
}

juce::int64 Track::getNextReadPosition() const
{
    return mPositionInSamples;
}

juce::int64 Track::getTotalLength() const
{
    juce::int64 max = 0;
    for (const auto& clip : mClips) {
        max = juce::jmax(clip->getPositionInSamples() + clip->getLengthInSamples(), max);
    }

    return max;
}

bool Track::isLooping() const
{
    return false;
}

void Track::setLooping(bool)
{}

void Track::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mPositionInSamples = 0;
    if (getType() == AUDIO_TRACK) {
        for (const auto& clip : mClips) {
            std::dynamic_pointer_cast<AudioClip>(clip)->prepareToPlay(samplesPerBlockExpected, sampleRate);
        }
    }
}

void Track::releaseResources()
{}

void Track::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    if (mClipPlaying == nullptr)
        bufferToFill.clearActiveBufferRegion();


    else {
        if (getType() == AUDIO_TRACK) {
            std::dynamic_pointer_cast<AudioClip>(mClipPlaying)->getNextAudioBlock(bufferToFill);
        }

        bufferToFill.buffer->applyGain(mVolume);

        // pan
        if (mPan != 0) {
            bufferToFill.buffer->applyGain(0, 0, bufferToFill.buffer->getNumSamples(), 1 - mPan);
            bufferToFill.buffer->applyGain(1, 0, bufferToFill.buffer->getNumSamples(), 1 + mPan);
        }
    }

    setNextReadPosition(getNextReadPosition() + bufferToFill.numSamples);
}

}
