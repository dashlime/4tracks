#include "projectsaver.h"

ProjectSaver::ProjectSaver(std::shared_ptr<Audio::Project> project) : mProject(project)
{

}

void ProjectSaver::saveToDirectory(QDir dir)
{
    QDir::root().mkpath(dir.absolutePath() + "/Resources");

    mProject->setProjectName(dir.dirName());

    QDir resourcesDir(dir.absolutePath() + "/Resources");
    QFile projectFile(dir.absoluteFilePath(dir.dirName() + ".4tpro"));

    QString result;
    QXmlStreamWriter writer(&result);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("project");
    writer.writeAttribute("projectname", mProject->getProjectName());
    writer.writeAttribute("bpm", QString::number(mProject->getBpm()));

    for (auto track : mProject->getTracks())
    {
        writer.writeStartElement("track");
        writer.writeAttribute("index", QString::number(track->getIndex()));
        writer.writeAttribute("name", track->getName());
        writer.writeAttribute("type", QString::number(track->getType()));

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
            writer.writeAttribute("type", QString::number(track->getType()));
            writer.writeAttribute("path", newFilePath);
            writer.writeAttribute("name", clip->getName());
            writer.writeAttribute("position", QString::number(clip->getPositionInSamples()));
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

void ProjectSaver::openProject(QFile projectFile)
{
    QDomDocument xmlBOM;

    if (!projectFile.open(QIODevice::ReadOnly))
        return;

    xmlBOM.setContent(&projectFile);
    projectFile.close();

    QDomElement project = xmlBOM.documentElement();
    mProject->setProjectName(project.attribute("projectname"));
    mProject->setBpm(project.attribute("bpm").toDouble());

    QDomElement domTrack = project.firstChild().toElement();

    while (!domTrack.isNull())
    {
        int trackType = domTrack.attribute("type").toInt();
        std::shared_ptr<Audio::AudioTrack> track = std::make_shared<Audio::AudioTrack>(domTrack.attribute("name"), domTrack.attribute("index").toInt());
        mProject->addTrack(track);

        QDomElement domClip = domTrack.firstChild().toElement();
        while (!domClip.isNull())
        {
            if (trackType == Audio::AudioTrack::AUDIO_TRACK)
            {
                std::shared_ptr<Audio::AudioClip> clip = std::make_shared<Audio::AudioClip>(track, domClip.attribute("path"));
                track->addClip(clip);

                clip->setName(domClip.attribute("name"));
                clip->setClipPositionInSamples(domClip.attribute("position").toInt());
            }

            domClip = domClip.nextSiblingElement();
        }

        domTrack = domTrack.nextSiblingElement();
    }
}
