#include "timelinescrollwidget.h"

namespace Graphics
{

TimelineScrollWidget::TimelineScrollWidget(const QPointer<Timeline> &timeline, const QSharedPointer<Audio::Project> &project)
    : QScrollBar(Qt::Horizontal), mTimeline(timeline), mProject(project)
{
    setMinimum(0);

    connect(this, &QAbstractSlider::valueChanged, [=](int value)
    {
        update();

        mTimeline->setNewScrollPosition(value);

        emit scrollValueChanged(value);
    });

    connect(mTimeline->getClipsGrid().get(), &ClipsGrid::sizeChanged, [=]()
    {
        refresh();
    });
}

void TimelineScrollWidget::refresh()
{
    int viewPortSize = mTimeline->getClipsGridWidth();
    int widgetSize = mTimeline->getClipsGrid()->minimumWidth();

    setRange(0, widgetSize - viewPortSize);
    setPageStep(viewPortSize);

    update();
}

void TimelineScrollWidget::resizeEvent(QResizeEvent *)
{
    refresh();
}

}