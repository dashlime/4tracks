#include "selectionmanager.h"

namespace Graphics
{

SelectionManager::Modifiers SelectionManager::generateSelectionModifiers(QMouseEvent *event)
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

SelectionManager::SelectionManager()
{}

SelectionManager::SelectionType SelectionManager::getSelectionTypeForObject(SelectableObject *object)
{
    if (object->getType() == SelectableObject::Track)
        return TracksSelected;
    if (object->getType() == SelectableObject::Clip)
        return ClipsSelected;
    if (object->getType() == SelectableObject::MidiNote)
        return MidiNotesSelected;

    return NoSelection;
}

void SelectionManager::setSelectionType(SelectionType type)
{
    clearSelection();
    mSelectionType = type;

    emit selectionChanged();
}

SelectionManager::SelectionType SelectionManager::getSelectionType() const
{
    return mSelectionType;
}

void SelectionManager::handleMousePressEvent(const QPointer<SelectableObject>& object, QMouseEvent *event)
{
    handleMousePressEvent(object, generateSelectionModifiers(event));
}

void SelectionManager::handleMousePressEvent(const QPointer<SelectableObject>& object, QFlags<Modifier> modifiers)
{
    if (getSelectionTypeForObject(object) != mSelectionType)
        setSelectionType(getSelectionTypeForObject(object));

    if (modifiers.testFlag(RightButtonModifier)) {
        if (!mSelectedObjects.contains(object)) {
            setSelectionType(getSelectionTypeForObject(object));
            mSelectedObjects.push_back(object);
            object->setSelectedState(true);
        }
    }
    else {
        if (modifiers.testFlag(CtrlModifier)) {
            if (!mSelectedObjects.contains(object)) {
                mSelectedObjects.push_back(object);
                object->setSelectedState(true);
            } else {
                mPendingObject = object;
                mPendingObjectModifiers = modifiers;
            }
        }
        else {
            if (mSelectedObjects.contains(object) && mSelectedObjects.size() > 1) {
                mPendingObject = object;
                mPendingObjectModifiers = modifiers;
            } else {
                setSelectionType(getSelectionTypeForObject(object));
                mSelectedObjects.push_back(object);
                object->setSelectedState(true);
            }
        }
    }

    emit selectionChanged();
}

void SelectionManager::handleMouseReleaseEvent()
{
    if (mPendingObject != nullptr) {
        if (mPendingObjectModifiers.testFlag(CtrlModifier) && mSelectedObjects.contains(mPendingObject)) {
            mSelectedObjects.remove(mSelectedObjects.indexOf(mPendingObject));
            mPendingObject->setSelectedState(false);
        } else {
            setSelectionType(getSelectionTypeForObject(mPendingObject));
            mSelectedObjects.push_back(mPendingObject);
            mPendingObject->setSelectedState(true);
        }

        mPendingObject = nullptr;

        emit selectionChanged();
    }
}

void SelectionManager::clearPendingEvent()
{
    mPendingObject = nullptr;
}

void SelectionManager::setSelectedObjects(const QVector<QPointer<SelectableObject>>& objects)
{
    for (const auto& object: objects) {
        if (mSelectionType != getSelectionTypeForObject(object))
            setSelectionType(getSelectionTypeForObject(object));

        if (!mSelectedObjects.contains(object)) {
            object->setSelectedState(true);
        }
    }

    for (const auto& object: mSelectedObjects) {
        if (!objects.contains(object)) {
            object->setSelectedState(false);
        }
    }

    mSelectedObjects.clear();
    mSelectedObjects.append(objects);

    emit selectionChanged();
}

QVector<QPointer<SelectableObject>> SelectionManager::getSelectedObjects() const
{
    return mSelectedObjects;
}

void SelectionManager::setSelectedArea(SelectionArea area)
{
    if (mSelectionType != AreaSelected)
        return;

    mSelectedArea = area;

    emit selectionChanged();
}

SelectionManager::SelectionArea SelectionManager::getSelectedArea() const
{
    return mSelectedArea;
}

void SelectionManager::clearSelection()
{
    for (const auto &obj: mSelectedObjects) {
        obj->setSelectedState(false);
    }

    mSelectedObjects.clear();

    mSelectedArea = SelectionArea();
    mSelectionType = NoSelection;
}

} // namespace Graphics
