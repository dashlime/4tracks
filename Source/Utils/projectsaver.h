#ifndef PROJECTSAVER_H
#define PROJECTSAVER_H

#include "Audio/Project/project.h"
#include <QDir>
#include <QtXml>
#include <QXmlStreamWriter>

class ProjectSaver
{
public:
    ProjectSaver(QSharedPointer<Audio::Project>);

    void saveToDirectory(QDir dir);
    void openProject(QFile projectFile);

private:
    QSharedPointer<Audio::Project> mProject;
};

#endif // PROJECTSAVER_H
