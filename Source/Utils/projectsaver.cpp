#include "projectsaver.h"

ProjectSaver::ProjectSaver(QSharedPointer<Audio::Project> project)
    : mProject(project)
{}

void ProjectSaver::saveToDirectory(QDir dir)
{
    QDir::root().mkpath(dir.absolutePath() + "/Resources");

    mProject->getProjectProperties()->setProjectName(dir.dirName());

    QDir resourcesDir(dir.absolutePath() + "/Resources");
    QFile projectFile(dir.absoluteFilePath(dir.dirName() + ".4tpro"));

    QString result;
    QXmlStreamWriter writer(&result);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("project");
    writer.writeAttribute("projectname", mProject->getProjectProperties()->getProjectName());
    writer.writeAttribute("bpm", QString::number(mProject->getProjectProperties()->getBpm()));

    for (auto track: mProject->getTracks()) {
        writer.writeStartElement("track");
        writer.writeAttribute("index", QString::number(track->getTrackProperties()->getIndex()));
        writer.writeAttribute("name", track->getTrackProperties()->getName());
        writer.writeAttribute("type", QString::number(track->getType()));

        int fileId = 0;
        for (auto clip: track->getClips()) {
            QString newFilePath;
            if (clip->getType() == Audio::Clip::AUDIO_CLIP) {
                QFile clipFile
                    (qSharedPointerDynamicCast<Audio::AudioClip>(clip)->getClipProperties()->getSourceFilePath());
                newFilePath = resourcesDir
                    .absoluteFilePath(QString::number(fileId) + " - " + QFileInfo(clipFile.fileName()).fileName());
                QFile targetFile(newFilePath);
                targetFile.remove();

                QFile::copy(clipFile.fileName(), targetFile.fileName());
            }
            fileId++;

            writer.writeStartElement("clip");
            writer.writeAttribute("type", QString::number(track->getType()));
            writer.writeAttribute("path", newFilePath);
            writer.writeAttribute("name", clip->getClipProperties()->getName());
            writer.writeAttribute("position", QString::number(clip->getClipProperties()->getPositionInSamples()));
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
    mProject->getProjectProperties()->setProjectName(project.attribute("projectname"));
    mProject->getProjectProperties()->setBpm(project.attribute("bpm").toDouble());

    QDomElement domTrack = project.firstChild().toElement();

    while (!domTrack.isNull()) {
        int trackType = domTrack.attribute("type").toInt();
        auto track =
            QSharedPointer<Audio::Track>::create(domTrack.attribute("name"),
                                                 domTrack.attribute("index").toInt(),
                                                 mProject.get());
        mProject->addTrack(track);

        QDomElement domClip = domTrack.firstChild().toElement();
        while (!domClip.isNull()) {
            if (trackType == Audio::Track::AUDIO_TRACK) {
                auto clip = QSharedPointer<Audio::AudioClip>::create(domClip.attribute("path"), track.get());
                track->addClip(clip);

                clip->getClipProperties()->setName(domClip.attribute("name"));
                clip->getClipProperties()->setPositionInSamples(domClip.attribute("position").toInt());
            }

            domClip = domClip.nextSiblingElement();
        }

        domTrack = domTrack.nextSiblingElement();
    }
}
