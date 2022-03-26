#include "selection.h"

namespace Graphics
{

Selection::Modifiers Selection::generateSelectionModifiers(QMouseEvent *event)
{
    auto flags = Modifiers();
    if (event->modifiers().testFlag(Qt::ControlModifier))
        flags = flags | Modifier::CtrlModifier;

    if (event->modifiers().testFlag(Qt::ShiftModifier))
        flags = flags | Modifier::ShiftModifier;

    if (event->buttons().testFlag(Qt::RightButton))
        flags = flags | Modifier::RightButtonModifier;

    return flags;
}

Selection::Selection()
{}

Selection::SelectionType Selection::getSelectionTypeForObject(Selection::SelectableObject* object)
{
    if (object->getType() == SelectableObject::Track)
        return TracksSelected;
    if (object->getType() == SelectableObject::Clip)
        return ClipsSelected;
    if (object->getType() == SelectableObject::MidiNote)
        return MidiNotesSelected;
}

void Selection::setSelectionType(SelectionType type)
{
    clearSelection();
    mSelectionType = type;

    emit selectionChanged();
}

Selection::SelectionType Selection::getSelectionType() const
{
    return mSelectionType;
}

void Selection::objectSelected(SelectableObject *object, QMouseEvent *event)
{
    objectSelected(object, generateSelectionModifiers(event));
}

void Selection::objectSelected(SelectableObject *object, QFlags<Modifier> modifiers)
{
    if (getSelectionTypeForObject(object) != mSelectionType)
        setSelectionType(getSelectionTypeForObject(object));

    QPointer<SelectableObject> ptr(object);

    if (modifiers.testFlag(RightButtonModifier)) {
        if (!mSelectedObjects.contains(ptr)) {
            setSelectionType(object->getType() == SelectableObject::Clip ? ClipsSelected : TracksSelected);
            mSelectedObjects.push_back(ptr);
            ptr->setSelectedState(true);
        }
    }
    else {
        if (modifiers.testFlag(CtrlModifier)) {
            if (mSelectedObjects.contains(ptr)) {
                mSelectedObjects.remove(mSelectedObjects.indexOf(ptr));
                ptr->setSelectedState(false);
            }
            else {
                mSelectedObjects.push_back(ptr);
                ptr->setSelectedState(true);
            }
        }
        else {
            setSelectionType(getSelectionTypeForObject(object));
            mSelectedObjects.push_back(ptr);
            ptr->setSelectedState(true);
        }
    }

    emit selectionChanged();
}

QVector<QPointer<Selection::SelectableObject>> Selection::getSelectedObjects() const
{
    return mSelectedObjects;
}

void Selection::setSelectedArea(int startTrackIndex, int startSample, int nbTracks, int nbSamples)
{
    if (mSelectionType != AreaSelected)
        return;

    mSelectedArea.startTrackIndex = startTrackIndex;
    mSelectedArea.startSample = startSample;
    mSelectedArea.nbTracks = nbTracks;
    mSelectedArea.nbSamples = nbSamples;

    emit selectionChanged();
}

void Selection::setSelectedArea(SelectionArea area)
{
    if (mSelectionType != AreaSelected)
        return;

    mSelectedArea = area;

    emit selectionChanged();
}

Selection::SelectionArea Selection::getSelectedArea() const
{
    return mSelectedArea;
}

void Selection::clearSelection()
{
    for (const auto &obj: mSelectedObjects) {
        obj->setSelectedState(false);
    }

    mSelectedObjects.clear();

    mSelectedArea = SelectionArea();
    mSelectionType = NoSelection;
}

} // namespace Graphics
