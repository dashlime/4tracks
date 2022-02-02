#include "projectproperties.h"

namespace Audio
{

ProjectProperties::ProjectProperties(const QString &name)
    : mProjectName(name)
{}

void ProjectProperties::setProjectName(const QString &newProjectName)
{
    mProjectName = newProjectName;

    emit projectNameChanged();
    updateSavedState(UNSAVED);
}

QString ProjectProperties::getProjectName() const
{
    return mProjectName;
}

void ProjectProperties::setBpm(double newBpm, bool propagateEvent)
{
    mBpm = newBpm;

    if (propagateEvent)
        emit bpmChanged();

    updateSavedState(UNSAVED);
}

double ProjectProperties::getBpm() const
{
    return mBpm;
}

void ProjectProperties::updateSavedState(ProjectProperties::SavedState newSavedState)
{
    mSavedState = newSavedState;

    emit savedStateChanged();
}

ProjectProperties::SavedState ProjectProperties::getSavedState() const
{
    return mSavedState;
}

void ProjectProperties::updateTotalLength()
{
    emit totalLengthChanged();
}

}