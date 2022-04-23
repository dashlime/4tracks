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
    mMixerAudioSource.addInputSource(newTrack.get());

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

    mProjectProperties->updateSavedState(ProjectProperties::UNSAVED);
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

        mProjectProperties->updateSavedState(ProjectProperties::UNSAVED);
    }

    return result;
}

QVector<QSharedPointer<Clip>> Project::getClips() const
{
    return mClips;
}

QSharedPointer<AudioResource> Project::getAlreadyLoadedResource(const QString &sourcePath) const
{
    if (sourcePath == "")
        return {};

    for (const auto &clip: mClips)
        if (clip->getType() == Clip::AUDIO_CLIP)
            if (qSharedPointerCast<AudioClip>(clip)->getAudioResource()->getSourceFilePath() == sourcePath)
                return qSharedPointerCast<AudioClip>(clip)->getAudioResource();

    return {};
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

    bool result;

    auto resource = getAlreadyLoadedResource(filePath);
    if (resource)
        result = addClip(QSharedPointer<Audio::AudioClip>::create(resource, parentTrack.get()));
    else
        result = addClip(QSharedPointer<Audio::AudioClip>::create(filePath, parentTrack.get()));

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
    for (auto &track: mTracks) {
        if (track == trackToRemove) {
            for (auto &clip: trackToRemove->getClips()) {
                removeClip(clip);
            }

            int trackIndex = trackToRemove->getTrackProperties()->getIndex();

            mMixerAudioSource.removeInputSource(trackToRemove.get());
            trackToRemove.reset();
            mTracks.remove(mTracks.indexOf(track));

            rearrangeTrackIndexes();

            emit trackRemoved(trackIndex);
            mProjectProperties->updateSavedState(ProjectProperties::UNSAVED);

            return;
        }
    }
}

void Project::removeClip(QSharedPointer<Clip> clipToRemove)
{
    for (auto &clip: mClips) {
        if (clip == clipToRemove) {
            int clipID = (int) mClips.indexOf(clip);

            clipToRemove->getClipProperties()->getParentTrack()->removeClip(clipToRemove);
            clipToRemove.reset();

            mClips.remove(clipID);

            emit clipRemoved(clipID);
            mProjectProperties->updateSavedState(ProjectProperties::UNSAVED);
            return;
        }
    }
}

void Project::removeArea(int startTrack, int nbTracks, juce::int64 startSample, juce::int64 nbSamples)
{
    if (nbSamples < 0) {
        startSample = startSample + nbSamples;
        nbSamples = -nbSamples;
    }

    if (nbTracks < 0) {
        // add 1 because track indexes starts with 0 and not with 1 :
        //    ex: startTrack(1) + tracksNumber(-2) + 1 = 0
        startTrack = startTrack + nbTracks + 1;
        nbTracks = -nbTracks;
    }

    for (int i = startTrack; i < startTrack + nbTracks; i++) {
        for (auto &clip: mTracks.at(i)->getClips()) {
            juce::int64 clipPosition = clip->getClipProperties()->getPositionInSamples();
            juce::int64 clipStartOffset = clip->getClipProperties()->getStartOffset();
            juce::int64 clipEndOffset = clip->getClipProperties()->getEndOffset();

            if (startSample <= clipPosition + clipStartOffset
                && startSample + nbSamples >= clipPosition + clipEndOffset) {
                removeClip(clip);
            }
            else if (startSample > clipPosition + clipStartOffset
                && startSample + nbSamples < clipPosition + clipEndOffset) {
                juce::int64 baseEndOffset = clip->getClipProperties()->getEndOffset();

                clip->getClipProperties()->setEndOffset(startSample - clipPosition);
                int newClipID = duplicateClip(clip);

                mClips.at(newClipID)->getClipProperties()->setStartOffset(startSample + nbSamples - clipPosition);
                mClips.at(newClipID)->getClipProperties()->setEndOffset(baseEndOffset);
                mClips.at(newClipID)->getClipProperties()
                    ->setPositionInSamples(clip->getClipProperties()->getPositionInSamples());
            }
            else {
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
}

int Project::duplicateClip(const QSharedPointer<Clip> &clipToDuplicate)
{
    juce::int64 oldClipPosition = clipToDuplicate->getClipProperties()->getPositionInSamples();
    juce::int64 oldClipStartOffset = clipToDuplicate->getClipProperties()->getStartOffset();
    juce::int64 oldClipEndOffset = clipToDuplicate->getClipProperties()->getEndOffset();
    juce::int64 oldClipLength = clipToDuplicate->getClipProperties()->getLengthInSamples();

    QSharedPointer<Clip> newClip;

    if (clipToDuplicate->getType() == Clip::AUDIO_CLIP) {
        newClip = QSharedPointer<AudioClip>::create(qSharedPointerCast<AudioClip>(clipToDuplicate)->getAudioResource(),
                                                    clipToDuplicate->getClipProperties()->getParentTrack());
    }
    else if (clipToDuplicate->getType() == Clip::MIDI_CLIP) {
        auto midiClip = QSharedPointer<MidiClip>::create(clipToDuplicate->getClipProperties()->getParentTrack());
        newClip = midiClip;
        midiClip->setMidiData(qSharedPointerDynamicCast<MidiClip>(clipToDuplicate)->getMidiData());
    }

    newClip->getClipProperties()->setLengthInSamples(oldClipLength);
    newClip->getClipProperties()->setStartOffset(oldClipStartOffset);
    newClip->getClipProperties()->setEndOffset(oldClipEndOffset);
    newClip->getClipProperties()->setPositionInSamples(oldClipPosition + oldClipEndOffset - oldClipStartOffset);

    addClip(newClip);

    return (int) mClips.size() - 1;
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

void Project::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mMixerAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Project::releaseResources()
{
    mPlaying = false;
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
