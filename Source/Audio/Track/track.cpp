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
    int clipID = (int) mClips.size();
    int type = getType();

    if (clip->getType() == Clip::AUDIO_CLIP && type == AUDIO_TRACK) {
        mClips.push_back(clip);
        resizeClipsWhenClipAdded(clipID);
    }
    else if (clip->getType() == Clip::MIDI_CLIP && type == MIDI_TRACK) {
        mClips.push_back(clip);
        resizeClipsWhenClipAdded(clipID);
    }
    else if (type == ANY_TRACK)
        mClips.push_back(clip);
    else
        return false;

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

void Track::removeClip(const QSharedPointer<Clip> &clipToRemove)
{
    auto it = mClips.begin();

    for (auto &clip: mClips) {
        if (clip == clipToRemove) {
            mClips.erase(it);
            mClipPlaying = nullptr;
        }
        it++;
    }
}

void Track::removeArea(int startSample, int nbSamples)
{
    for (auto &clip: mClips) {
        juce::int64 clipPosition = clip->getClipProperties()->getPositionInSamples();
        juce::int64 clipStartOffset = clip->getClipProperties()->getStartOffset();
        juce::int64 clipEndOffset = clip->getClipProperties()->getEndOffset();

        if (startSample <= clipPosition + clipStartOffset
            && startSample + nbSamples >= clipPosition + clipEndOffset) {
            removeClip(clip);
            emit(clipRemoved(clip));
        } else {
            // set end offset
            if (startSample > clipPosition + clipStartOffset
                && startSample < clipPosition + clipEndOffset) {
                clip->getClipProperties()->setEndOffset(startSample - clipPosition);
            }
            // set start offset
            else if (startSample + nbSamples > clipPosition + clipStartOffset
                && startSample + nbSamples < clipPosition + clipEndOffset) {
                clip->getClipProperties()->setStartOffset(startSample + nbSamples - clipPosition);
            }
        }
    }
}

void Track::applyGainToBuffer(juce::AudioSampleBuffer *buffer)
{
    buffer->applyGain(mProperties->getVolume());
}

void Track::applyPanToBuffer(juce::AudioSampleBuffer *buffer)
{
    if (mProperties->getPan() != 0) {
        buffer->applyGain(0, 0, buffer->getNumSamples(), 1 - mProperties->getPan());
        buffer->applyGain(1, 0, buffer->getNumSamples(), 1 + mProperties->getPan());
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

        applyPanToBuffer(bufferToFill.buffer);
        applyGainToBuffer(bufferToFill.buffer);
    }

    setNextReadPosition(getNextReadPosition() + bufferToFill.numSamples);
}

}
