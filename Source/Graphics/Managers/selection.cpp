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

void Selection::setSelectionCallback(Callback *callback)
{
    mCallback = callback;
}

void Selection::setSelectionType(SelectionType type)
{
    clearSelection();
    mSelectionType = type;

    if (mCallback != nullptr)
        mCallback->selectionChanged();
}

Selection::SelectionType Selection::getSelectionType() const
{
    return mSelectionType;
}

void Selection::objectSelected(SelectableObject* object, QMouseEvent *event)
{
    objectSelected(object, generateSelectionModifiers(event));
}

void Selection::objectSelected(SelectableObject* object, QFlags<Modifier> modifiers)
{
    if (object->getType() == SelectableObject::Track && mSelectionType != TracksSelected)
        setSelectionType(TracksSelected);
    if (object->getType() == SelectableObject::Clip && mSelectionType != ClipsSelected)
        setSelectionType(ClipsSelected);

    QPointer<SelectableObject> ptr(object);

    if (modifiers.testFlag(RightButtonModifier)) {
        if (!mSelectedObjects.contains(ptr)) {
            setSelectionType(TracksSelected);
            mSelectedObjects.push_back(ptr);
            ptr->setSelectedState(true);
        }
    } else {
        if (modifiers.testFlag(CtrlModifier)) {
            if (mSelectedObjects.contains(ptr)) {
                mSelectedObjects.remove(mSelectedObjects.indexOf(ptr));
                ptr->setSelectedState(false);
            } else {
                mSelectedObjects.push_back(ptr);
                ptr->setSelectedState(true);
            }
        } else {
            setSelectionType(TracksSelected);
            mSelectedObjects.push_back(ptr);
            ptr->setSelectedState(true);
        }
    }

    if (mCallback != nullptr)
        mCallback->selectionChanged();
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

    if (mCallback != nullptr)
        mCallback->selectionChanged();
}

void Selection::setSelectedArea(SelectionArea area)
{
    if (mSelectionType != AreaSelected)
        return;

    mSelectedArea = area;

    if (mCallback != nullptr)
        mCallback->selectionChanged();
}

Selection::SelectionArea Selection::getSelectedArea() const
{
    return mSelectedArea;
}

void Selection::clearSelection()
{
    for (const auto& obj: mSelectedObjects) {
        obj->setSelectedState(false);
    }

    mSelectedObjects.clear();

    mSelectedArea = SelectionArea();
    mSelectionType = NoSelection;
}

} // namespace Graphics
