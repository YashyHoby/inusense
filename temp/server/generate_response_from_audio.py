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
import json
import wave
from dotenv import load_dotenv
load_dotenv()
import tempfile
import shutil

import pydub
import pykakasi

from dotenv import load_dotenv
load_dotenv("../../.hisaenv")

chatgpt_api = os.getenv('CHATGPT_API')

# Google Cloud APIキーの設定
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = os.getenv('GOOGLE_CLOUD_API_PATH')
client = speech.SpeechClient()

# 音声録音の設定
RATE = 16000
CHUNK = int(RATE / 10)  # 100ms

# ChatGPT APIの設定
#CHATGPT_API = os.environ["CHATGPT_API"]

FORMAT = """
        Your name is Sako-chan. Speak in Japanese, using Hokusei dialect.
        Answer in one sentence.
        """

# 利用ファイル
RESPONSE_AUDIO = "response.mp3" # テキスト読み上げ音声ファイル
QUESTION_AUDIO = "question2.mp3" # 録音音声保存用ファイル
CHAT_HISTORY_FILE = "chat_history.json"  # 履歴を保存するファイル

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

    # 録音データをmp3形式で保存
    with wave.open(QUESTION_AUDIO, 'wb') as wf:
        wf.setnchannels(1)
        wf.setsampwidth(audio.get_sample_size(pyaudio.paInt16))
        wf.setframerate(RATE)
        wf.writeframes(b''.join(frames))

# 音声からテキスト生成
# def transcribe_speech(audio):
#     try:
#         with open(QUESTION_AUDIO, 'rb') as f:
#             content = f.read()
        
#         audio = speech.RecognitionAudio(content=content)
#         config = speech.RecognitionConfig(
#             encoding=speech.RecognitionConfig.AudioEncoding.LINEAR16,
#             sample_rate_hertz=RATE,
#             language_code='ja-JP'
#         )
#         response = client.recognize(config=config, audio=audio)

#         text = ""
#         for result in response.results:
#             text += result.alternatives[0].transcript

#         return text

#     except FileNotFoundError:
#         print("Audio file not found.")
#         return None
    
def transcribe_speech(audio_input):
    try:
        # `response.mp3` を WAV に変換し、サンプルレートを統一
        audio = pydub.AudioSegment.from_mp3(audio_input)
        audio = audio.set_frame_rate(RATE).set_channels(1)
        temp_wav = tempfile.NamedTemporaryFile(delete=False, suffix=".wav")
        audio.export(temp_wav.name, format="wav")


        with open(temp_wav.name, 'rb') as f:
            content = f.read()

            config = speech.RecognitionConfig(
                encoding=speech.RecognitionConfig.AudioEncoding.LINEAR16,
                sample_rate_hertz=RATE,
                language_code='ja-JP'
            )

            response = client.recognize(config=config, audio=speech.RecognitionAudio(content=content))
            
            text = ""
            for result in response.results:
                text += result.alternatives[0].transcript

            #os.remove(temp_wav.name)  # 一時ファイルを削除
            return text

    except FileNotFoundError:
        print("Audio file not found.")
        return None


# ChatGPTのAPIキー取得
# def read_keyFile():
#     try:
#         with open(CHATGPT_API_KEY_PATH, 'r', encoding='utf-8') as f:
#             key = f.readline().strip()
#         return key
#     except FileNotFoundError:
#         return "ファイルが見つかりません。"
#     except Exception as e:
#         return f"エラーが発生しました: {e}"

# ファイルから会話履歴を読み込む
def load_chat_history():
    try:
        with open(CHAT_HISTORY_FILE, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        return []  # ファイルがなければ空のリストを返す

# 会話履歴をファイルに保存する

def save_chat_history(messages):
    try:
        # 一時ファイルを作成して安全に書き込み
        with tempfile.NamedTemporaryFile('w', delete=False, encoding='utf-8') as temp_file:
            json.dump(messages, temp_file, ensure_ascii=False, indent=4)
            temp_path = temp_file.name

        # 書き込みが成功したら置き換え
        shutil.move(temp_path, CHAT_HISTORY_FILE)

    except Exception as e:
        print(f"Error while saving chat history: {e}")
# 会話履歴を初期化する
def initialize_chat_history():
    empty_history = []  # 空のリストで初期化
    with open(CHAT_HISTORY_FILE, 'w', encoding='utf-8') as f:
        json.dump(empty_history, f, ensure_ascii=False, indent=4)
    print("Chat history has been initialized.")


# ChatGPTに質問テキストを渡して返答テキストを受け取る
def receive_GPT_response(question):
    openai.api_key = chatgpt_api

    dt_now = str(datetime.datetime.now())
    
    messages = load_chat_history()
    messages.append({"role": "user", "content": question})
    
    try:
        # 正しいAPIエンドポイントを使用してAPIキーの検証を行います
        res = openai.chat.completions.create(
            model="gpt-4o-mini",
            messages=messages
        )
        #print("APIキーが正しく認識されています。")
        answer = res.choices[0].message.content
        save_chat_history(messages)

    except openai.AuthenticationError:
        print("APIキーが無効です。正しいAPIキーを設定してください。")
    except openai.RateLimitError:
        print("レートリミットに達しました。後でもう一度試してください。")
    except openai.OpenAIError as e:
        print(f"OpenAIのエラーが発生しました: {e}")

    return answer

# テキストを渡すと読み上げ音声を生成
def create_speech_mp3(text):
    if os.path.exists(RESPONSE_AUDIO):
        os.remove(RESPONSE_AUDIO)

    tts = gTTS(text, lang='ja')
    tts.save(RESPONSE_AUDIO)

# 音声再生（非同期）
def play_audio():
    playsound(RESPONSE_AUDIO)

def convert_to_romaji(input):
    kakasi = pykakasi.kakasi()

    # モードの設定
    kakasi.setMode("H","a") # Hiragana to ascii, default: no conversion
    kakasi.setMode("K","a") # Katakana to ascii, default: no conversion
    kakasi.setMode("J","a") # Japanese to ascii, default: no conversion
    kakasi.setMode("r","Passport") # default: use Hepburn Roman table

    text = input
    conv = kakasi.getConverter()
    result = conv.do(text)
    return result  # Return the result instead of printing it

# 動作テスト（録音・生成・再生）
# def test():
#     while True:
#         command = input("Enter 'r' to start recording, 'c' to check microphone, 'q' to quit: ")
#         if command.lower() == 'q':
#             print("Exiting...")
#             break

#         if command.lower() == 'r':
#             record_audio()
#             text = transcribe_speech()
#             print(text)
#             answer = receive_GPT_response(text)
#             create_speech_mp3(answer)
#             print(answer)
#             play_audio()
            
#             time.sleep(1)  # 録音後、再度待機状態に戻るための一時停止

# 録音音声（QUESTION_AUDIO）を読み込み、返答音声（RESPONSE_AUDIO）を作成
def generate_response_audio():
    text = transcribe_speech(QUESTION_AUDIO)
    answer = receive_GPT_response(text)
    #create_speech_mp3(answer)
    #input = transcribe_speech(RESPONSE_AUDIO)
    return str(convert_to_romaji(answer))

#main()
# test()