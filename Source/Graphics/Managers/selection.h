#ifndef GRAPHICS_SELECTION_H
#define GRAPHICS_SELECTION_H

#include <QFlags>

#include "Graphics/Overlays/selectionoverlay.h"

namespace Graphics
{

class Selection
{
public:
    enum SelectionType
    {
        NoSelection, TracksSelected, ClipsSelected, AreaSelected
    };
    enum Modifier
    {
        CtrlModifier = 0x1, ShiftModifier = 0x2, RightButtonModifier = 0x4
    };
    Q_DECLARE_FLAGS(Modifiers, Modifier)

    static Modifiers generateSelectionModifiers(QMouseEvent *event);

    class Callback
    {
    public:
        Callback() = default;

        virtual void selectionChanged() = 0;
    };

    class SelectableObject: public QWidget
    {
    public:
        explicit SelectableObject(QWidget *parent = nullptr) : QWidget(parent)
        {};
        virtual ~SelectableObject()
        {};

        virtual void setSelectedState(bool isSelected) = 0;

        enum Type
        {
            Track, Clip
        };
        [[nodiscard]] virtual Type getType() const = 0;
    };

    Selection();

    void setSelectionCallback(Callback *callback);

    void setSelectionType(SelectionType type);
    [[nodiscard]] SelectionType getSelectionType() const;

    void objectSelected(SelectableObject* object, QMouseEvent *event);
    void objectSelected(SelectableObject* object, QFlags<Modifier> modifiers);

    [[nodiscard]] QVector<QPointer<SelectableObject>> getSelectedObjects() const;

    struct SelectionArea
    {
        int startTrackIndex;
        int startSample;
        int nbTracks;
        int nbSamples;
    };

    void setSelectedArea(int startTrackIndex, int startSample, int nbTracks, int nbSamples);
    void setSelectedArea(SelectionArea area);

    [[nodiscard]] SelectionArea getSelectedArea() const;


private:
    void clearSelection();

    SelectionType mSelectionType = NoSelection;
    QVector<QPointer<SelectableObject>> mSelectedObjects;
    SelectionArea mSelectedArea = SelectionArea();
    Callback *mCallback = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Selection::Modifiers)

} // namespace Graphics

#endif // GRAPHICS_SELECTION_H
