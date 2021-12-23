#include "projectsaver.h"

ProjectSaver::ProjectSaver(std::shared_ptr<Audio::Project> project) : mProject(project)
{

}

void ProjectSaver::saveToDirectory(QDir dir)
{
    QDir::root().mkpath(dir.absolutePath() + "/Resources");

    QDir resourcesDir(dir.absolutePath() + "/Resources");
    QFile projectFile(dir.absoluteFilePath(dir.dirName() + ".4tpro"));

    QString result;
    QXmlStreamWriter writer(&result);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("project");
    writer.writeAttribute("projectname", mProject->getProjectName());

    for (auto track : mProject->getTracks())
    {
        writer.writeStartElement("track");
        writer.writeAttribute("name", track->getName());
        writer.writeAttribute("type", track->getType() == Audio::AudioTrack::AUDIO_TRACK ? XML_AUDIO : XML_MIDI);

        int fileId = 0;
        for (auto clip : track->getClips())
        {
            QString newFilePath;
            if (clip->getType() == Audio::Clip::AUDIO_CLIP)
            {
                QFile clipFile(std::dynamic_pointer_cast<Audio::AudioClip>(clip)->getPath());
                newFilePath = resourcesDir.absoluteFilePath(QString::number(fileId) + " - " + QFileInfo(clipFile.fileName()).fileName());
                QFile targetFile(newFilePath);
                targetFile.remove();

                QFile::copy(clipFile.fileName(), targetFile.fileName());
            }
            fileId ++;

            writer.writeStartElement("clip");
            writer.writeAttribute("type", track->getType() == Audio::AudioTrack::AUDIO_TRACK ? XML_AUDIO : XML_MIDI);
            writer.writeAttribute("path", newFilePath);
            writer.writeAttribute("name", clip->getName());
            writer.writeEndElement();
        }

        writer.writeEndElement();
    }

    writer.writeEndElement();

    writer.writeEndDocument();

    projectFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&projectFile);

    stream << result;

    projectFile.close();
}
