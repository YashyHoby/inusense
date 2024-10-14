from gtts import gTTS
from playsound import playsound
import os

FILE_NAME = "speech.mp3"

def create_speech_mp3(text):
    if os.path.exists(FILE_NAME):
        os.remove(FILE_NAME)

    tts = gTTS(text, lang='ja')
    tts.save(FILE_NAME)

def play_audio():
    playsound(FILE_NAME)

def main():
    text = ""
    while True:
        text = input("Enter text to read or press q to exit: ") 
        if text.lower() == "q":
            break
        create_speech_mp3(text)
        play_audio()




main()