#include "timeline.h"

#include "QDebug"

namespace Audio {

Timeline::Timeline() : mMixerAudioSource(DEFAULT_SAMPLE_RATE)
{

}

void Timeline::addTrack(std::shared_ptr<AudioTrack> newTrack)
{
    mTracks.append(newTrack);
    mMixerAudioSource.addInputSource(newTrack.get(), false);
}

void Timeline::play()
{
    mPlaying = true;
}

void Timeline::pause()
{
    mPlaying = false;
}

void Timeline::stop()
{
    mPlaying = false;
    mMixerAudioSource.setNextReadPosition(0);
}

void Timeline::setBpm(double newBpm)
{
    mBpm = newBpm;
}

double Timeline::getBpm() const
{
    return mBpm;
}

QVector<std::shared_ptr<AudioTrack>> Timeline::getTracks()
{
    return mTracks;
}

bool Timeline::isPlaying() const
{
    return mPlaying;
}

void Timeline::setNextReadPosition(juce::int64 newPosition)
{
    mMixerAudioSource.setNextReadPosition(newPosition);
}

juce::int64 Timeline::getNextReadPosition() const
{
    return mMixerAudioSource.getNextReadPosition();
}

juce::int64 Timeline::getTotalLength() const
{
    return mMixerAudioSource.getTotalLength();
}

bool Timeline::isLooping() const
{
    return false;
}

void Timeline::setLooping(bool shouldLoop) {}

void Timeline::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mMixerAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Timeline::releaseResources()
{
    mMixerAudioSource.releaseResources();
}

void Timeline::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    if (isPlaying())
        mMixerAudioSource.getNextAudioBlock(bufferToFill);

    else
        bufferToFill.clearActiveBufferRegion();
}

}
