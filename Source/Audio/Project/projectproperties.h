#ifndef PROJECTPROPERTIES_H
#define PROJECTPROPERTIES_H

#include <QString>
#include <QObject>

namespace Audio
{

class ProjectProperties: public QObject
{
Q_OBJECT

public:
    ProjectProperties(const QString &name);

    void setProjectName(const QString &newProjectName);
    [[nodiscard]] QString getProjectName() const;

    void setBpm(double newBpm, bool propagateEvent = true);
    [[nodiscard]] double getBpm() const;

    enum SavedState
    {
        SAVED, UNSAVED
    };

    void updateSavedState(SavedState newSavedState);
    [[nodiscard]] SavedState getSavedState() const;

    void updateTotalLength();

signals:
    void projectNameChanged();
    void bpmChanged();
    void savedStateChanged();
    void totalLengthChanged();

private:
    QString mProjectName;
    double mBpm = 0;
    SavedState mSavedState = SAVED;
};

}

#endif //PROJECTPROPERTIES_H
