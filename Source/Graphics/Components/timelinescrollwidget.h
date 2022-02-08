#ifndef TIMELINESCROLLWIDGET_H
#define TIMELINESCROLLWIDGET_H

#include <QtWidgets>

#include "timeline.h"

namespace Graphics
{

class TimelineScrollWidget: public QScrollBar
{
Q_OBJECT
public:
    TimelineScrollWidget(const QPointer<Timeline> &timeline, const QSharedPointer<Audio::Project> &project);

    void refresh();

    void resizeEvent(QResizeEvent *) override;
signals:
    void scrollValueChanged(int newScrollBarValue);

private:
    QPointer<Timeline> mTimeline;
    QSharedPointer<Audio::Project> mProject;
};

}

#endif //TIMELINESCROLLWIDGET_H
