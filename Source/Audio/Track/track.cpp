#include "track.h"

namespace Audio
{

Track::Track(const QString &name, int index, Project *parentProject)
    : mProperties(new TrackProperties(parentProject, name, index))
{}

QSharedPointer<TrackProperties> Track::getTrackProperties() const
{
    return mProperties;
}

int Track::getType() const
{
    if (mClips.empty())
        return ANY_TRACK;

    return mClips.at(0)->getType() == Clip::AUDIO_CLIP ? AUDIO_TRACK : MIDI_TRACK;
}

bool Track::addClip(const QSharedPointer<Clip> &clip)
{
    int newClipIndex = (int) mClips.size();
    int type = getType();

    if (clip->getType() == Clip::AUDIO_CLIP && type == AUDIO_TRACK) {
        mClips.push_back(clip);
        resizeClipsWhenClipAdded(newClipIndex);
    }
    else if (clip->getType() == Clip::MIDI_CLIP && type == MIDI_TRACK) {
        mClips.push_back(clip);
        resizeClipsWhenClipAdded(newClipIndex);
    }
    else if (type == ANY_TRACK)
        mClips.push_back(clip);
    else
        return false;

    emit clipAdded(newClipIndex);
    connect(clip->getClipProperties().get(), &ClipProperties::trackLengthChanged, [=]()
    {
        emit mProperties->lengthChanged();
    });

    return true;
}

QVector<QSharedPointer<Clip>> Track::getClips()
{
    return mClips;
}

void Track::removeClip(QSharedPointer<Clip> clipToRemove)
{
    auto it = mClips.begin();

    for (auto &clip: mClips) {
        if (clip.get() == clipToRemove.get()) {
            clipToRemove.reset();

            mClips.erase(it);
            mClipPlaying = nullptr;

            emit clipRemoved((int) std::distance(mClips.begin(), it));
        }
        it++;
    }
}

void Track::resizeClipsWhenClipAdded(int newClipIndex)
{

}

void Track::setNextReadPosition(juce::int64 newPosition)
{
    mPositionInSamples = newPosition;
    mClipPlaying = nullptr;

    for (const auto &clip: mClips) {
        if (clip->getClipProperties()->getPositionInSamples() <= mPositionInSamples
            && clip->getClipProperties()->getPositionInSamples() + clip->getClipProperties()->getLengthInSamples()
                >= mPositionInSamples) {
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
    for (const auto &clip: mClips) {
        max = juce::jmax(
            clip->getClipProperties()->getPositionInSamples() + clip->getClipProperties()->getLengthInSamples(), max);
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
        for (const auto &clip: mClips) {
            qSharedPointerDynamicCast<AudioClip>(clip)->prepareToPlay(samplesPerBlockExpected, sampleRate);
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
            qSharedPointerDynamicCast<AudioClip>(mClipPlaying)->getNextAudioBlock(bufferToFill);
        }

        bufferToFill.buffer->applyGain(mProperties->getVolume());

        // pan
        if (mProperties->getPan() != 0) {
            bufferToFill.buffer->applyGain(0, 0, bufferToFill.buffer->getNumSamples(), 1 - mProperties->getPan());
            bufferToFill.buffer->applyGain(1, 0, bufferToFill.buffer->getNumSamples(), 1 + mProperties->getPan());
        }
    }

    setNextReadPosition(getNextReadPosition() + bufferToFill.numSamples);
}

}
