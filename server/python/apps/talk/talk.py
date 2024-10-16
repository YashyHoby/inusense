import os
import time
import pyaudio
import keyboard
from google.cloud import speech
from gtts import gTTS
from playsound import playsound
import openai
from openai import RateLimitError, Timeout, APIError, APIConnectionError, OpenAIError
import datetime


# Google Cloud APIキーの設定
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "../../../../config/google/notion-calendar-manager-bc1ebc32bf22.json"
client = speech.SpeechClient()
# 音声録音の設定
RATE = 16000
CHUNK = int(RATE / 10)  # 100ms


CHATGPT_API_KEY_PATH = "../../../../config/openai/chatGPT_api.txt"
FORMAT = """
        speak japanese.
        speak 北摂弁.
        your name is さこちゃん.
        """

# 読み上げ音声ファイル
FILE_NAME = "speech.mp3"

def check_microphone():
    audio = pyaudio.PyAudio()
    print("Available audio devices:")
    for i in range(audio.get_device_count()):
        device_info = audio.get_device_info_by_index(i)
        print(f"Device {i}: {device_info['name']} (Input channels: {device_info['maxInputChannels']})")
    
    audio.terminate()

def record_audio():
    audio = pyaudio.PyAudio()
    
    # 入力デバイスの確認
    input_device_index = None
    for i in range(audio.get_device_count()):
        device_info = audio.get_device_info_by_index(i)
        if device_info['maxInputChannels'] > 0:
            input_device_index = i
            break
    
    if input_device_index is None:
        print("No microphone device found.")
        return None

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
    if audio_data is None:
        print("No audio data to transcribe.")
        return
    
    audio = speech.RecognitionAudio(content=audio_data)
    config = speech.RecognitionConfig(
        encoding=speech.RecognitionConfig.AudioEncoding.LINEAR16,
        sample_rate_hertz=RATE,
        language_code='ja-JP'
    )

    response = client.recognize(config=config, audio=audio)

    text = ""
    for result in response.results:
        text = text + result.alternatives[0].transcript

    return text

def read_keyFile():
    try:
        with open(CHATGPT_API_KEY_PATH, 'r', encoding='utf-8') as file:
            key = file.readline().strip()
        return key
    except FileNotFoundError:
        return "ファイルが見つかりません。"
    except Exception as e:
        return f"エラーが発生しました: {e}"

def receive_GPT_response(question):
    openai.api_key = read_keyFile()

    dt_now = str(datetime.datetime.now())
    
    answer = ""
    try:
        # 正しいAPIエンドポイントを使用してAPIキーの検証を行います
        res = openai.chat.completions.create(
            model="gpt-4o-mini",
            messages=[
                {"role": "system", "content": FORMAT},
                {"role": "user", "content": question}
            ]
        )
        #print("APIキーが正しく認識されています。")
        answer = res.choices[0].message.content
    except openai.AuthenticationError:
        print("APIキーが無効です。正しいAPIキーを設定してください。")
    except openai.RateLimitError:
        print("レートリミットに達しました。後でもう一度試してください。")
    except openai.OpenAIError as e:
        print(f"OpenAIのエラーが発生しました: {e}")

    return answer

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
    while True:
        command = input("Enter 'r' to start recording, 'c' to check microphone, 'q' to quit: ")
        if command.lower() == 'q':
            print("Exiting...")
            break

        if command.lower() == 'c':
            check_microphone()

        if command.lower() == 'r':
            audio_data = record_audio()
            text = transcribe_speech(audio_data)
            answer = receive_GPT_response(text)
            create_speech_mp3(answer)
            print(answer)
            play_audio()
            
            time.sleep(1)  # 録音後、再度待機状態に戻るための一時停止

main()
