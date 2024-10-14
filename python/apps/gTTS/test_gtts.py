from gtts import gTTS
from playsound import playsound
import os

# 読み上げ音声ファイル
FILE_NAME = "speech.mp3"

# テキストを渡すと読み上げ音声を生成
def create_speech_mp3(text):
    if os.path.exists(FILE_NAME):
        os.remove(FILE_NAME)

    tts = gTTS(text, lang='ja')
    tts.save(FILE_NAME)

# 音声再生（非同期）
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