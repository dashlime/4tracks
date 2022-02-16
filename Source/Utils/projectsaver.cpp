#include "projectsaver.h"

ProjectSaver::ProjectSaver(QSharedPointer<Audio::Project> project)
    : mProject(project)
{}

void ProjectSaver::initProjectStructure(const QDir &projectDir)
{
    QDir::root().mkpath(projectDir.absolutePath() + "/Resources");

    mProject->getProjectProperties()->setProjectName(projectDir.dirName());
}

void ProjectSaver::saveToDirectory(const QDir &dir)
{
    QDir resourcesDir(dir.absolutePath() + "/Resources");
    QFile projectFile(dir.absoluteFilePath(dir.dirName() + ".4tpro"));
    initProjectStructure(dir);

    QString result;
    QXmlStreamWriter writer(&result);

    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("project");
    writer.writeAttribute("projectname", mProject->getProjectProperties()->getProjectName());
    writer.writeAttribute("bpm", QString::number(mProject->getProjectProperties()->getBpm()));

    for (const auto &track: mProject->getTracks()) {
        writer.writeStartElement("track");
        writer.writeAttribute("index", QString::number(track->getTrackProperties()->getIndex()));
        writer.writeAttribute("name", track->getTrackProperties()->getName());
        writer.writeAttribute("type", QString::number(track->getType()));

        int fileId = 0;
        for (const auto &clip: track->getClips()) {
            QString newFilePath;
            if (clip->getType() == Audio::Clip::AUDIO_CLIP) {
                QFile clipFile
                    (qSharedPointerDynamicCast<Audio::AudioClip>(clip)->getClipProperties()->getSourceFilePath());
                newFilePath = resourcesDir
                    .absoluteFilePath(QString::number(fileId) + " - " + QFileInfo(clipFile.fileName()).fileName());
                QFile targetFile(newFilePath);
                targetFile.remove();

                if (clipFile.exists())
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

    projectFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&projectFile);

    writer.writeEndElement();

    writer.writeEndDocument();
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
            int id = -1;
            if (trackType == Audio::Track::AUDIO_TRACK)
                id = mProject->createAudioClip(track, domClip.attribute("path"));
            else if (trackType == Audio::Track::MIDI_TRACK)
                id = mProject->createMIDIClip(track);

            if (id != -1) {
                auto clip = mProject->getClips().at(id);

                clip->getClipProperties()->setName(domClip.attribute("name"));
                clip->getClipProperties()->setPositionInSamples(domClip.attribute("position").toInt());
            }

            domClip = domClip.nextSiblingElement();
        }

        domTrack = domTrack.nextSiblingElement();
    }
}
