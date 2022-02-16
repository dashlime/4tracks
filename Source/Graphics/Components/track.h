#ifndef TRACK_H
#define TRACK_H

#include <QtWidgets>

#include "Audio/Track/track.h"
#include "Audio/Track/trackproperties.h"
#include "Audio/Project/project.h"
#include "Graphics/Managers/selection.h"

namespace Graphics
{

namespace Ui
{
class Track;
}

class Track: public Selection::SelectableObject
{
Q_OBJECT

public:
    explicit Track(QSharedPointer<Audio::Track> track, QWidget *parent = nullptr);
    ~Track() override;

    void setupComponents();
    void setupCallbacks();

    void deleteTrack();

    void paintEvent(QPaintEvent *) override;

    void setSelectedState(bool isSelected) override;
    [[nodiscard]] Selection::SelectableObject::Type getType() const override;

private:
    Ui::Track *ui;
    QSharedPointer<Audio::Track> mAudioTrack;
};
}

#endif // TRACK_H
