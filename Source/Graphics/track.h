#ifndef TRACK_H
#define TRACK_H

#include <QtWidgets>

#include "Audio/track.h"
#include "Audio/project.h"

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
    explicit Track(const std::shared_ptr<Audio::Track> &track, QWidget *parent = nullptr);
    ~Track() override;

    void setSelected(bool isSelected);
    [[nodiscard]] bool isSelected() const;

    void paintEvent(QPaintEvent *) override;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    Ui::Track *ui;
    std::shared_ptr<Audio::Track> mAudioTrack;
    bool mSelected = false;
};
}

#endif // TRACK_H
