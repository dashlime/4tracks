#ifndef TRACK_H
#define TRACK_H

#include <QtWidgets>

#include "Audio/Track/track.h"
#include "Audio/Track/trackproperties.h"
#include "Audio/Project/project.h"

namespace Graphics
{

namespace Ui
{
class Track;
}

class Track: public QWidget
{
Q_OBJECT

public:
    explicit Track(QSharedPointer<Audio::Track> track, QWidget *parent = nullptr);
    ~Track() override;

    void setupComponents();
    void setupCallbacks();

    void setSelected(bool isSelected);
    [[nodiscard]] bool isSelected() const;

    void paintEvent(QPaintEvent *) override;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    Ui::Track *ui;
    QSharedPointer<Audio::Track> mAudioTrack;
    bool mSelected = false;
};
}

#endif // TRACK_H
