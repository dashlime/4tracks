#ifndef TRACK_H
#define TRACK_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "Audio/track.h"


namespace Graphics {

namespace Ui {
class Track;
}

class Track : public QWidget
{
    Q_OBJECT

public:
    explicit Track(std::shared_ptr<Audio::AudioTrack> track, QWidget *parent = nullptr);
    ~Track();

    void setSelected(bool isSelected);
    bool isSelected() const;

    void paintEvent(QPaintEvent *) override;

private:
    Ui::Track *ui;
    std::shared_ptr<Audio::AudioTrack> mAudioTrack;
    bool mSelected = false;
};
}

#endif // TRACK_H
