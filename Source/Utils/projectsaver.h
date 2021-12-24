#ifndef PROJECTSAVER_H
#define PROJECTSAVER_H

#include "Audio/project.h"
#include <QDir>
#include <QtXml>
#include <QXmlStreamWriter>

class ProjectSaver
{
public:
    ProjectSaver(std::shared_ptr<Audio::Project>);

    void saveToDirectory(QDir dir);
    void openProject(QFile projectFile);

private:
    std::shared_ptr<Audio::Project> mProject;
};

#endif // PROJECTSAVER_H
