#ifndef GRAPHICS_SELECTION_H
#define GRAPHICS_SELECTION_H

#include <QFlags>

#include "Graphics/Overlays/selectionoverlay.h"
#include "Audio/JuceIncludes.h"

namespace Graphics
{

class SelectionManager: public QObject
{
Q_OBJECT

public:
    enum SelectionType
    {
        NoSelection, TracksSelected, ClipsSelected, AreaSelected, MidiNotesSelected
    };
    enum Modifier
    {
        CtrlModifier = 0x1, ShiftModifier = 0x2, RightButtonModifier = 0x4
    };
    Q_DECLARE_FLAGS(Modifiers, Modifier)

    static Modifiers generateSelectionModifiers(QMouseEvent *event);

    class SelectableObject: public QWidget
    {
    public:
        explicit SelectableObject(QWidget *parent = nullptr)
            : QWidget(parent)
        {};
        virtual ~SelectableObject()
        {};

        virtual void setSelectedState(bool isSelected) = 0;

        enum Type
        {
            Track, Clip, MidiNote
        };
        [[nodiscard]] virtual Type getType() const = 0;
    };

    SelectionManager();

    static SelectionType getSelectionTypeForObject(SelectableObject *object);

    void setSelectionType(SelectionType type);
    [[nodiscard]] SelectionType getSelectionType() const;

    void objectSelected(SelectableObject *object, QMouseEvent *event);
    void objectSelected(SelectableObject *object, QFlags<Modifier> modifiers);

    [[nodiscard]] QVector<QPointer<SelectableObject>> getSelectedObjects() const;

    struct SelectionArea
    {
        int startTrackIndex;
        juce::int64 startSample;
        int nbTracks;
        juce::int64 nbSamples;
    };

    void setSelectedArea(int startTrackIndex, int startSample, int nbTracks, int nbSamples);
    void setSelectedArea(SelectionArea area);

    [[nodiscard]] SelectionArea getSelectedArea() const;

signals:
    void selectionChanged();

private:
    void clearSelection();

    SelectionType mSelectionType = NoSelection;
    QVector<QPointer<SelectableObject>> mSelectedObjects;
    SelectionArea mSelectedArea = SelectionArea();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SelectionManager::Modifiers)

} // namespace Graphics

#endif // GRAPHICS_SELECTION_H
