#include "projectsaver.h"

ProjectSaver::ProjectSaver(std::shared_ptr<Audio::Project> project) : mProject(project)
{

}

void ProjectSaver::saveToFile(QFile file)
{
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
        writer.writeEndElement();
    }

    writer.writeEndElement();

    writer.writeEndDocument();

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);

    stream << result;

    file.close();
}
