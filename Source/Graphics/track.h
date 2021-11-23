#ifndef TRACK_H
#define TRACK_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "Audio/audiotrack.h"

namespace Ui {
class Track;
}

namespace Graphics {
    class Track : public QWidget
    {
        Q_OBJECT

    public:
        explicit Track(std::shared_ptr<Audio::AudioTrack> track, QWidget *parent = nullptr);
        ~Track();

        void paintEvent(QPaintEvent *) override;

    private:
        Ui::Track *ui;
        std::shared_ptr<Audio::AudioTrack> mAudioTrack;
    };
}

#endif // TRACK_H
