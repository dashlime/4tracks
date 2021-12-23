#ifndef PROJECTSAVER_H
#define PROJECTSAVER_H

#include "Audio/project.h"
#include <QFile>
#include <QXmlStreamWriter>

class ProjectSaver
{
public:
    ProjectSaver(std::shared_ptr<Audio::Project>);

    void saveToFile(QFile file);

private:
    std::shared_ptr<Audio::Project> mProject;
};

#endif // PROJECTSAVER_H
