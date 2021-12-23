#include "graphics_timeline.h"

namespace Graphics {

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
    setLayout(&mMainLayout);
    layout()->setSpacing(1);

    setStyleSheet(QString::fromUtf8("QScrollArea {"
                                    "   border: none;"
                                    "}"
                                    "QScrollBar:vertical {"
                                    "   background:rgba(0,0,0,0);"
                                    "   width:8px;    "
                                    "   margin: 0px 0px 0px 0px;"
                                    "}"
                                    "QScrollBar:horizontal {"
                                    "   background:rgba(0,0,0,0);"
                                    "   height:8px;    "
                                    "   margin: 0px 0px 0px 0px;"
                                    "}"
                                    "QScrollBar::handle {"
                                    "   background: #37474F;"
                                    "   border-radius: 4px;"
                                    "}"
                                    "QScrollBar::add-line, QScrollBar::sub-line {"
                                    "   background: none;"
                                    "}"
                                    "QScrollBar::left-arrow, QScrollBar::right-arrow, QScrollBar::down-arrow, QScrollBar::up-arrow, QScrollBar::add-page {"
                                    "   background: none;"
                                    "}"
    ));
}

void Timeline::setProject(std::shared_ptr<Audio::Project> project)
{
    mProject = project;

    displayTracks();
}

void Timeline::displayTracks()
{
    Utils::clearLayout(&mMainLayout);
    int i = 0;

    for (auto track : mProject->getTracks())
    {
        mMainLayout.addWidget(new Graphics::Track(track), i, 0, Qt::AlignTop);
        i++;
    }

    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mMainLayout.addWidget(spacerWidget, i, 0);

    mMainLayout.addWidget(&mScrollArea, 0, 1, i+1, 1);
    mScrollArea.setWidget(&mClipsGrid);
    mScrollArea.setWidgetResizable(true);
    mScrollArea.setFocusPolicy(Qt::FocusPolicy::NoFocus);

    mClipsGrid.refreshBpm(mProject->getBpm());
    mClipsGrid.setProject(mProject);

    refreshClipsGridGeometry();
}

void Timeline::refreshZoomLevel(double newZoomLevel)
{
    mZoomLevel = newZoomLevel;
    mClipsGrid.refreshZoomLevel(mZoomLevel);

    refreshClipsGridGeometry();
}

double Timeline::getZoomLevel() const
{
    return mZoomLevel;
}

void Timeline::refreshClipsGridGeometry()
{
    mClipsGrid.updateGeometry();
    mClipsGrid.setGeometry(0, 0, mClipsGrid.minimumWidth(), mClipsGrid.minimumHeight());
    if (mScrollArea.width() > mClipsGrid.minimumWidth())
        mClipsGrid.setGeometry(0, 0, mScrollArea.width(), mClipsGrid.height());

    if (mScrollArea.height() > mClipsGrid.minimumHeight())
        mClipsGrid.setGeometry(0, 0, mClipsGrid.width(), mScrollArea.height());

    mScrollArea.updateGeometry();
}

void Timeline::refreshBpm()
{
    if (mProject.get() != nullptr)
        mClipsGrid.refreshBpm(mProject->getBpm());
}

double Timeline::getDivision() const
{
    return mClipsGrid.getDivision();
}

void Timeline::resizeEvent(QResizeEvent *event)
{
    refreshClipsGridGeometry();
}

}
