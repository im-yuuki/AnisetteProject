import json
import os
import zipfile
import logging
import shutil
from typing import Optional
from random import randint

WORK_DIR = "tmp/"
OUT_DIR = "out/"

class OsuFileSection(enumerate):
    NOT_DEFINED = -1
    GENERAL = 0
    METADATA = 1
    DIFFICULTY = 2
    EVENTS = 3
    HIT_OBJECTS = 4


class Note:
    __slots__ = ["start", "end", "channel", "hold"]

    def __init__(self):
        self.start = 0
        self.end = 0
        self.channel = 4
        self.hold = False


    @property
    def __dict__(self):
        return {
            "start": self.start,
            "end": self.end,
            "channel": self.channel,
            "hold": self.hold
        }


class NoteCollection:
    def __init__(self):
        self.notes = []
        self.channels = 0
        self.single_note_count = 0
        self.hold_note_count = 0

        # just keep only 6 channels in the output
        self.channel_0 = []
        self.channel_1 = []
        self.channel_2 = []
        self.channel_3 = []
        self.channel_4 = []
        self.channel_5 = []

    def append(self, note: Note):
        self.notes.append(note)
        if note.hold:
            self.hold_note_count += 1
        else:
            self.single_note_count += 1

    def build(self) -> bool:
        if self.channels < 4 or self.channels > 6:
            logging.error(f"Invalid original channel count: {self.channels}")
            return False
        logging.debug(f"Single note count: {self.single_note_count}")
        logging.debug(f"Hold note count: {self.hold_note_count}")
        
        if self.channels == 4:
            # original channel 1 will random go to channel 1 or 2
            # original channel 2 will go to channel 3
            # original channel 3 will go to channel 4
            # original channel 4 will random go to channel 5 or 6
            for note in self.notes:
                if note.channel == 0:
                    if randint(0, 1) == 0:
                        self.channel_0.append(note)
                    else:
                        self.channel_1.append(note)
                elif note.channel == 1:
                    self.channel_2.append(note)
                elif note.channel == 2:
                    self.channel_3.append(note)
                elif note.channel == 3:
                    if randint(0, 1) == 0:
                        self.channel_4.append(note)
                    else:
                        self.channel_5.append(note)
        
        elif self.channels == 5:
            # 1 to 1, 2 to 2, 3 to random 3 or 4, 4 to 5, 5 to 6
            for note in self.notes:
                if note.channel == 0:
                    self.channel_0.append(note)
                elif note.channel == 1:
                    self.channel_1.append(note)
                elif note.channel == 2:
                    if randint(0, 1) == 0:
                        self.channel_2.append(note)
                    else:
                        self.channel_3.append(note)
                elif note.channel == 3:
                    self.channel_4.append(note)
                elif note.channel == 4:
                    self.channel_5.append(note)
        
        elif self.channels == 6:
            # keep the original layout
            for note in self.notes:
                if note.channel == 0:
                    self.channel_0.append(note)
                elif note.channel == 1:
                    self.channel_1.append(note)
                elif note.channel == 2:
                    self.channel_2.append(note)
                elif note.channel == 3:
                    self.channel_3.append(note)
                elif note.channel == 4:
                    self.channel_4.append(note)
                elif note.channel == 5:
                    self.channel_5.append(note)

        # sort the notes by start time
        self.channel_0.sort(key=lambda x: x.start)
        self.channel_1.sort(key=lambda x: x.start)
        self.channel_2.sort(key=lambda x: x.start)
        self.channel_3.sort(key=lambda x: x.start)
        self.channel_4.sort(key=lambda x: x.start)
        self.channel_5.sort(key=lambda x: x.start)

        return True
    
    @property
    def __dict__(self):
        return {
            "single_note_count": self.single_note_count,
            "hold_note_count": self.hold_note_count,
            "channel_0": [note.__dict__ for note in self.channel_0],
            "channel_1": [note.__dict__ for note in self.channel_1],
            "channel_2": [note.__dict__ for note in self.channel_2],
            "channel_3": [note.__dict__ for note in self.channel_3],
            "channel_4": [note.__dict__ for note in self.channel_4],
            "channel_5": [note.__dict__ for note in self.channel_5]
        }


class MapData:
    id: int = 0
    title: str = ""
    artist: str = ""
    thumbnail: str = ""
    music: str = ""
    preview_point: int = 0
    difficulty: int = 10
    hp_drain: int = 10
    notes: NoteCollection = NoteCollection()

    @property
    def __dict__(self):
        return {
            "$schema": "https://raw.githubusercontent.com/im-yuuki/AnisetteProject/master/scripts/beatmap.schema.json",
            "version": 1,
            "id": self.id,
            "title": self.title,
            "artist": self.artist,
            "thumbnail": self.thumbnail,
            "music": self.music,
            "preview_point": self.preview_point,
            "difficulty": self.difficulty,
            "hp_drain": self.hp_drain,
            "notes": self.notes.__dict__,
        }


    def export_json(self, file_path: str) -> None:
        with open(file_path, "w", encoding="utf-8") as f:
            json.dump(self.__dict__, f, indent=2, ensure_ascii=False)
            logging.info(f"Exported data to {file_path}")
    

def handle_osu_file(file_path: str) -> Optional[MapData]:
    if not os.path.exists(file_path):
        logging.error(f"File does not exist.")
        return None
    f = open(file_path, "r", encoding="utf-8")
    current_section = OsuFileSection.NOT_DEFINED
    data = MapData()
    for line in f.readlines():
        line = line.strip()
        if line.startswith("//") or line == "":
            continue
        # switch section
        if line.startswith("["):
            logging.debug(f"Switching to section {line}")
            if line.startswith("[General]"):
                current_section = OsuFileSection.GENERAL
            elif line.startswith("[Metadata]"):
                current_section = OsuFileSection.METADATA
            elif line.startswith("[Difficulty]"):
                current_section = OsuFileSection.DIFFICULTY
            elif line.startswith("[Events]"):
                current_section = OsuFileSection.EVENTS
            elif line.startswith("[HitObjects]"):
                current_section = OsuFileSection.HIT_OBJECTS
            else:
                current_section = OsuFileSection.NOT_DEFINED
            continue

        # parse data
        if current_section == OsuFileSection.GENERAL:
            if line.startswith("AudioFilename:"):
                data.music = line.split(":")[1].strip()
                logging.debug(f"Music file: {data.music}")
            elif line.startswith("PreviewTime:"):
                data.preview_point = int(line.split(":")[1].strip())
                logging.debug(f"Preview point: {data.preview_point}")
            elif line.startswith("Mode:"):
                mode: int = int(line.split(":")[1].strip())
                logging.debug(f"Mode: {mode}")
                if mode != 3:
                    logging.error(f"File is not a osu!mania map.")
                    return None

        elif current_section == OsuFileSection.METADATA:
            if line.startswith("Title:"):
                data.title = line.split(":")[1].strip()
                logging.debug(f"Title: {data.title}")
            elif line.startswith("Artist:"):
                data.artist = line.split(":")[1].strip()
                logging.debug(f"Artist: {data.artist}")
            elif line.startswith("BeatmapID:"):
                data.id = int(line.split(":")[1].strip())
                logging.debug(f"Beatmap ID: {data.id}")

        elif current_section == OsuFileSection.DIFFICULTY:
            if line.startswith("OverallDifficulty:"):
                data.difficulty = 100 - int(float(line.split(":")[1].strip()) * 10)
                logging.debug(f"Overall difficulty: {data.difficulty}")
            elif line.startswith("HPDrainRate:"):
                data.hp_drain = 100 - int(float(line.split(":")[1].strip()) * 10)
                logging.debug(f"HP drain: {data.hp_drain}")
            elif line.startswith("CircleSize:"):
                data.notes.channels = int(line.split(":")[1].strip())
                if data.notes.channels > 6 or data.notes.channels < 4:
                    logging.error("Only 4K, 5K and 6K beatmaps supported.")
                    return None
                logging.debug(f"Column count: {data.notes.channels}")

        elif current_section == OsuFileSection.EVENTS:
            if line.startswith("0,0"):
                data.thumbnail = line.split(",")[2].strip()
                # if quoted, remove the quotes
                if data.thumbnail.startswith('"') and data.thumbnail.endswith('"'):
                    data.thumbnail = data.thumbnail[1:-1]
                logging.debug(f"Thumbnail: {data.thumbnail}")

        elif current_section == OsuFileSection.HIT_OBJECTS:
            # x,y,time,type,hitSound,endTime:hitSample
            # we just need x, time, type, and endTime
            parts = line.split(",")
            if len(parts) < 6:
                continue
            note = Note()
            type: int = int(parts[3])
            
            if type & 0b00000001:
                # single note
                note.start = int(parts[2])
                note.channel = int(int(parts[0]) * data.notes.channels / 512)
                data.notes.append(note)
                logging.debug(f"Single note: start {note.start}, channel {note.channel}")
            elif type & 0b10000000:
                # hold note
                note.hold = True
                note.start = int(parts[2])
                note.channel = int(int(parts[0]) * data.notes.channels / 512)
                note.end = int(parts[5].split(":")[0])
                data.notes.append(note)
                logging.debug(f"Hold note: start {note.start}, end {note.end}, channel {note.channel}")

    f.close()
    logging.info(f"File parsed successfully.")
    return data
    

def extract_osz_file(file_name: str) -> str:
    if not os.path.exists(file_name):
        logging.error(f"File {file_name} does not exist.")
        return
    dir_name = WORK_DIR + file_name[:-4] + "/"
    logging.info(f"Creating directory [{dir_name}]")
    os.makedirs(dir_name, exist_ok=True)
    with zipfile.ZipFile(file_name, 'r') as zip_ref:
        zip_ref.extractall(dir_name)
        logging.info(f"Extracted {file_name} files to {dir_name}")
    return dir_name


def handle_osz_file(file_name: str) -> None:
    extracted_dir = extract_osz_file(file_name)
    _out_dir: str = OUT_DIR + file_name[:-4] + "/"
    os.makedirs(_out_dir, exist_ok=True)
    for file in os.listdir(extracted_dir):
        if os.path.isdir(file):
            continue
        if file.endswith(".osu"):
            file_path = os.path.join(extracted_dir, file)
            logging.info(f"Processing OSU file: {file_path}")
            out = handle_osu_file(file_path)
            if out is None:
                continue
            if not out.notes.build():
                continue
            out.export_json(_out_dir + str(out.id) + ".json")
            # copy thumbnail and music file
            if out.thumbnail != "":
                shutil.copy(os.path.join(extracted_dir, out.thumbnail), os.path.join(_out_dir, out.thumbnail))
            if out.music != "":
                shutil.copy(os.path.join(extracted_dir, out.music), os.path.join(_out_dir, out.music))
            

if __name__ == "__main__":
    # Set up logging
    logging.basicConfig(level=logging.INFO, format='[%(levelname)s] %(message)s')
    # scan the current directory for .osz files
    for file_name in os.listdir("."):
        if not file_name.endswith(".osz"):
            continue
        if os.path.isfile(file_name):
            logging.info(f"Found OSZ file: {file_name}")
            handle_osz_file(file_name)
    # clean up the temporary directory
    shutil.rmtree(WORK_DIR, ignore_errors=True)