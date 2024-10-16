import os
import time
import pyaudio
import keyboard
from google.cloud import speech

# Google Cloud APIキーの設定
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "../../../config/google/notion-calendar-manager-bc1ebc32bf22.json"

client = speech.SpeechClient()

# 音声録音の設定
RATE = 16000
CHUNK = int(RATE / 10)  # 100ms

def record_audio():
    audio = pyaudio.PyAudio()
    stream = audio.open(format=pyaudio.paInt16,
                        channels=1,
                        rate=RATE,
                        input=True,
                        frames_per_buffer=CHUNK)
    print("Recording... Press 'Enter' to stop.")
    
    frames = []

    # Enterが押されるまで録音を続ける
    while True:
        data = stream.read(CHUNK)
        frames.append(data)
        if keyboard.is_pressed('Enter'):  # Enterキーが押されたら録音停止
            break

    print("Recording finished.")
    stream.stop_stream()
    stream.close()
    audio.terminate()

    return b''.join(frames)

def transcribe_speech(audio_data):
    audio = speech.RecognitionAudio(content=audio_data)
    config = speech.RecognitionConfig(
        encoding=speech.RecognitionConfig.AudioEncoding.LINEAR16,
        sample_rate_hertz=RATE,
        language_code='ja-JP'
    )

    response = client.recognize(config=config, audio=audio)
    for result in response.results:
        print('Transcript: {}'.format(result.alternatives[0].transcript))

def main():
    while True:
        command = input("Enter 'r' to start recording, 'q' to quit: ")
        if command.lower() == 'q':
            print("Exiting...")
            break
        
        if command.lower() == 'r':
            audio_data = record_audio()
            transcribe_speech(audio_data)
            time.sleep(1)  # 録音後、再度待機状態に戻るための一時停止

main()
