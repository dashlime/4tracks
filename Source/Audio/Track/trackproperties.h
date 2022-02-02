#ifndef TRACKPROPERTIES_H
#define TRACKPROPERTIES_H

#include <QString>
#include <QObject>

#include "Audio/Project/projectproperties.h"

namespace Audio
{

class Project;

class TrackProperties: public QObject
{
Q_OBJECT

public:
    TrackProperties(Project *parentProject, const QString &trackName, int index);

    [[nodiscard]] Project *getParentProject() const;

    void setName(const QString &name);
    [[nodiscard]] QString getName() const;

    void setIndex(int newPosition);
    [[nodiscard]] int getIndex() const;

    void setVolume(float newVolume);
    [[nodiscard]] float getVolume() const;

    void setPan(float newPan);
    [[nodiscard]] float getPan() const;

signals:
    void trackNameChanged();
    void indexChanged();
    void volumeChanged();
    void panChanged();
    void savedStateChanged(ProjectProperties::SavedState savedState);
    void lengthChanged();

private:
    Project *mParentProject;

    QString mName;
    int mIndex;

    float mVolume = 1.f;
    float mPan = 0.f;
};

}

#endif //TRACKPROPERTIES_H
