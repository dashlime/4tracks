#include "projectsaver.h"

namespace Audio
{

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

    QVector<QSharedPointer<Audio::AudioResource>> resources;

    for (const auto &track: mProject->getTracks()) {
        writer.writeStartElement("track");
        writer.writeAttribute("index", QString::number(track->getTrackProperties()->getIndex()));
        writer.writeAttribute("name", track->getTrackProperties()->getName());
        writer.writeAttribute("type", QString::number(track->getType()));
        writer.writeAttribute("volume", QString::number(track->getTrackProperties()->getVolume()));
        writer.writeAttribute("pan", QString::number(track->getTrackProperties()->getPan()));

        int fileId = 0;
        for (const auto &clip: track->getClips()) {
            writer.writeStartElement("clip");
            writer.writeAttribute("type", QString::number(track->getType()));
            writer.writeAttribute("name", clip->getClipProperties()->getName());
            writer.writeAttribute("position", QString::number(clip->getClipProperties()->getPositionInSamples()));
            writer.writeAttribute("startOffset", QString::number(clip->getClipProperties()->getStartOffset()));
            writer.writeAttribute("endOffset", QString::number(clip->getClipProperties()->getEndOffset()));
            writer.writeAttribute("length", QString::number(clip->getClipProperties()->getLengthInSamples()));

            QString newFilePath;
            if (clip->getType() == Audio::Clip::AUDIO_CLIP) {
                auto audioClip = qSharedPointerDynamicCast<Audio::AudioClip>(clip);

                QFile clipFile(audioClip->getAudioResource()->getSourceFilePath());

                if (resources.contains(audioClip->getAudioResource()))
                    newFilePath = clipFile.fileName();
                else {
                    resources.append(audioClip->getAudioResource());

                    newFilePath = resourcesDir
                        .absoluteFilePath(QString::number(fileId) + " - " + QFileInfo(clipFile.fileName()).fileName());
                    QFile targetFile(newFilePath);
                    targetFile.remove();

                    if (clipFile.exists())
                        QFile::copy(clipFile.fileName(), targetFile.fileName());

                    audioClip->getAudioResource()->setSourceFilePath(targetFile.fileName());
                }

                writer.writeAttribute("path", newFilePath);
                fileId++;
            }
            else if (clip->getType() == Audio::Clip::MIDI_CLIP) {
                auto midiClip = qSharedPointerDynamicCast<Audio::MidiClip>(clip);

                writer.writeStartElement("midiData");
                for (const auto &midiNote: midiClip->getMidiData()->getMidiNotes()) {
                    if (midiNote->getMidiMessage().isNoteOn() && midiNote->getNoteOffObject() != nullptr) {
                        writer.writeStartElement("note");
                        writer
                            .writeAttribute("noteNumber", QString::number(midiNote->getMidiMessage().getNoteNumber()));
                        writer.writeAttribute("startSample", QString::number(midiNote->getPositionInSamples()));
                        writer.writeAttribute("endSample",
                                              QString::number(midiNote->getNoteOffObject()->getPositionInSamples()));
                        writer.writeEndElement();
                    }
                }
                writer.writeEndElement();
            }
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

bool ProjectSaver::openProject(QFile projectFile)
{
    QDomDocument xmlBOM;

    if (!projectFile.open(QIODevice::ReadOnly))
        return false;

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

        track->getTrackProperties()->setVolume(domTrack.attribute("volume").toFloat());
        track->getTrackProperties()->setPan(domTrack.attribute("pan").toFloat());

        mProject->addTrack(track);

        QDomElement domClip = domTrack.firstChild().toElement();
        while (!domClip.isNull()) {
            int id = -1;
            if (trackType == Audio::Track::AUDIO_TRACK)
                id = mProject->createAudioClip(track, domClip.attribute("path"));
            else if (trackType == Audio::Track::MIDI_TRACK) {
                id = mProject->createMIDIClip(track);
                auto clip = qSharedPointerDynamicCast<Audio::MidiClip>(mProject->getClips().at(id));

                QDomElement domNote = domClip.firstChildElement("midiData").firstChildElement("note");
                while (!domNote.isNull()) {
                    auto noteOnMsg =
                        juce::MidiMessage::noteOn(DEFAULT_MIDI_CHANNEL, domNote.attribute("noteNumber").toInt(), 1.f);
                    auto noteOffMsg =
                        juce::MidiMessage::noteOff(DEFAULT_MIDI_CHANNEL, domNote.attribute("noteNumber").toInt());
                    auto noteOn =
                        QSharedPointer<Audio::MidiNote>::create(domNote.attribute("startSample").toInt(), noteOnMsg);
                    auto noteOff =
                        QSharedPointer<Audio::MidiNote>::create(domNote.attribute("endSample").toInt(), noteOffMsg);
                    noteOn->setNoteOffObject(noteOff);

                    clip->getMidiData()->addNote(noteOn);
                    clip->getMidiData()->addNote(noteOff);

                    domNote = domNote.nextSiblingElement();
                }
            }

            if (id != -1) {
                auto clip = mProject->getClips().at(id);

                clip->getClipProperties()->setName(domClip.attribute("name"));
                clip->getClipProperties()->setPositionInSamples(domClip.attribute("position").toInt());
                clip->getClipProperties()->setStartOffset(domClip.attribute("startOffset").toInt());
                clip->getClipProperties()->setEndOffset(domClip.attribute("endOffset").toInt());
                clip->getClipProperties()->setLengthInSamples(domClip.attribute("length").toInt());
            }

            domClip = domClip.nextSiblingElement();
        }

        domTrack = domTrack.nextSiblingElement();
    }

    return true;
}

}