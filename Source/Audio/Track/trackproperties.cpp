#include "trackproperties.h"

namespace Audio
{

TrackProperties::TrackProperties(Project *parentProject, const QString &trackName, int index)
    : mParentProject(parentProject), mIndex(index), mName(trackName)
{}

Project *TrackProperties::getParentProject() const
{
    return mParentProject;
}

void TrackProperties::setName(const QString &name)
{
    mName = name;

    emit trackNameChanged();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

QString TrackProperties::getName() const
{
    return mName;
}

void TrackProperties::setIndex(int newPosition)
{
    mIndex = newPosition;

    emit indexChanged();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

int TrackProperties::getIndex() const
{
    return mIndex;
}

void TrackProperties::setVolume(float newVolume)
{
    mVolume = newVolume;

    emit volumeChanged();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

float TrackProperties::getVolume() const
{
    return mVolume;
}

void TrackProperties::setPan(float newPan)
{
    mPan = newPan;

    emit panChanged();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

float TrackProperties::getPan() const
{
    return mPan;
}

}