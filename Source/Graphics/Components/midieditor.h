#ifndef MIDIEDITOR_H
#define MIDIEDITOR_H

#include <QtWidgets>

#include "Audio/Project/project.h"
#include "Constants.h"
#include "Utils/utils_functions.h"
#include "Audio/Clip/midiclip.h"

namespace Graphics
{

class MidiEditor: public QWidget
{
    Q_OBJECT
public:
    explicit MidiEditor(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

private:
    QSharedPointer<Audio::MidiClip> mClip;
};

} // Graphics

#endif //MIDIEDITOR_H
