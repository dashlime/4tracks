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
        ~SelectableObject() override = default;

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

    void handleMousePressEvent(const QPointer<SelectableObject>& object, QMouseEvent *event);
    void handleMousePressEvent(const QPointer<SelectableObject>& object, QFlags<Modifier> modifiers);

    void handleMouseReleaseEvent();

    void clearPendingEvent();

    void setSelectedObjects(const QVector<QPointer<SelectableObject>>& objects);
    [[nodiscard]] QVector<QPointer<SelectableObject>> getSelectedObjects() const;

    struct SelectionArea
    {
        int startTrackIndex {};
        juce::int64 startSample {};
        int tracksNumber {};
        juce::int64 samplesNumber {};
    };

    void setSelectedArea(SelectionArea area);

    [[nodiscard]] SelectionArea getSelectedArea() const;

signals:
    void selectionChanged();

private:
    void clearSelection();

    SelectionType mSelectionType = NoSelection;
    QVector<QPointer<SelectableObject>> mSelectedObjects;
    SelectionArea mSelectedArea = SelectionArea();

    QPointer<SelectableObject> mPendingObject;
    QFlags<Modifier> mPendingObjectModifiers;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SelectionManager::Modifiers)

} // namespace Graphics

#endif // GRAPHICS_SELECTION_H
