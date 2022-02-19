#ifndef CLIPPROPERTIES_H
#define CLIPPROPERTIES_H

#include <QString>
#include <QPointer>

#include "Audio/JuceIncludes.h"
#include "Audio/Project/projectproperties.h"

namespace Audio
{

class Track;

class ClipProperties: public QObject
{
Q_OBJECT

public:
    ClipProperties(Track *parentTrack, const QString &name, const QString &sourceFilePath);

    void setParentTrack(Track *parentTrack);
    [[nodiscard]] Track *getParentTrack() const;

    void setName(const QString &name);
    [[nodiscard]] const QString &getName() const;

    void setSourceFilePath(const QString &sourceFilePath);
    [[nodiscard]] QString getSourceFilePath() const;

    void setLengthInSamples(juce::int64 lengthInSamples);
    [[nodiscard]] juce::int64 getLengthInSamples() const;

    void setPositionInSamples(juce::int64 positionInSamples);
    [[nodiscard]] juce::int64 getPositionInSamples() const;

    // all samples before this offset will not been played
    void setStartOffset(juce::int64 offsetInSamples);
    [[nodiscard]] juce::int64 getStartOffset() const;

    // all samples after this offset will not been played
    // the offset is counted from the 1st sample of the clip, not from start offset
    void setEndOffset(juce::int64 offsetInSamples);
    [[nodiscard]] juce::int64 getEndOffset() const;

signals:
    void parentTrackChanged();
    void nameChanged();
    void sourceFileChanged();
    void lengthChanged();
    void clipMoved();
    void savedStateChanged(ProjectProperties::SavedState savedState);
    void trackLengthChanged();
    void startOffsetChanged();
    void endOffsetChanged();

private:
    Track *mParentTrack;

    QString mName;
    QString mSourceFilePath;

    juce::int64 mPositionInSamples = 0;
    juce::int64 mLengthInSamples = 0;
    juce::int64 mStartOffset = 0;
    juce::int64 mEndOffset = 0;
};

}

#endif //CLIPPROPERTIES_H
