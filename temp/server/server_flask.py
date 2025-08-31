from flask import Flask,request,Response, jsonify
import os
import time
from send_message import send_message_to_user
# import sys
# sys.path.append(os.getcwd())
from generate_response_from_audio import generate_response_audio

from dotenv import load_dotenv
load_dotenv("../../.hisaenv")

line_api_token = os.getenv('LINE_API_TOKEN')
line_user_id = os.getenv('LINE_USER_ID')

# テスト用
TEXT_FILE_PATH = "answer.txt"
MP3_FILE_PATH = "response.mp3"

app = Flask(__name__)

# GETリクエストを処理するエンドポイント
@app.route('/getData', methods=['GET'])
def send_audio_file():
    audio_file_path = '../test.mp3'
    def generate():
        with open(audio_file_path, 'rb') as f:
            while True:
                data = f.read(10)
                print(data)
                if not data:
                    break
                yield data
    return Response(generate(), content_type='application/octet-stream')

# LINE通知
@app.route('/getLINE', methods=['GET'])
def sendLINE():
    print("line request")
    send_message_to_user(line_api_token, line_user_id, "警戒通知")
    return Response("line okutta", status=200, mimetype='text/plain')

# 返答を生成して送信
@app.route('/getReply', methods=['GET'])
def generate_reply():
    print("generate response request")
    try:
        text = generate_response_audio()  # 音声の処理やChatGPTへのリクエストを実行する関数
        print(text)
        f = open('answer.txt', 'w')
        f.write(text+"./r")
        f.close()
        #return jsonify({"message": "処理が成功しました"})  # 成功レスポンスを返す
        return Response(text, status=200, mimetype='text/plain')
    
    except Exception as e:
        print(f"Error occurred: {e}")
        #return jsonify({"error": str(e)}), 500  # エラーをJSONレスポンスとして返す
        return Response("error", status=200, mimetype='text/plain')

# テキストファイル送信（テスト用）
@app.route('/getTextRes', methods=['GET'])
def send_answer_text():
    print("generate response")
    try:
        text_data = generate_response_audio()
        return Response(text_data, status=200, mimetype='text/plain')
    
    except Exception as e:
        print(f"Error occurred: {e}")
        return Response("error", status=200, mimetype='text/plain')

# テキストファイル送信（テスト用）
@app.route('/getText', methods=['GET'])
def send_text():
    print("send text to spresense")
    try:
        with open(TEXT_FILE_PATH, "r", encoding="utf-8") as file:
            text_data = file.read()
        print("ファイルの内容:")
        print(text_data)
        return Response(text_data, status=200, mimetype='text/plain')
    
    except Exception as e:
        print(f"Error occurred: {e}")
        #return jsonify({"error": str(e)}), 500  # エラーをJSONレスポンスとして返す
        return Response("error", status=200, mimetype='text/plain')


# オーディオファイル送信（テスト用）
@app.route('/getAudio', methods=['GET'])
def send_answer_audio():
    print("Sending MP3 audio to Spresense")
    try:
        # MP3ファイルをバイナリモードで読み込む
        with open(MP3_FILE_PATH, "rb") as file:
            audio_data = file.read()
        
        print("MP3ファイルを正常に読み込みました")
        
        # 音声データをレスポンスとして返す
        return Response(audio_data, status=200, mimetype='audio/mpeg')
    
    except Exception as e:
        print(f"Error occurred: {e}")
        # エラーが発生した場合、エラーメッセージを返す
        return Response("error", status=500, mimetype='text/plain')
    
"""def send_text_file():
    text_data = "This is a simple text string for testing purposes. Let's see if we can transfer it correctly."

    def generate():
        for i in range(0, len(text_data), 10):
            data = text_data[i:i + 10].encode()
            print(data)
            yield data # 10バイトずつエンコードして送信

    return Response(text_data, mimetype='text/plain')"""



# パケットを一時的に保存するリスト
received_packets = []

def body_to_bin(data_b:bytes):
    data_s = data_b.decode()#bin to ascii文字列(hex表記).ex,35323439...→"5249..."
    print(f"decoded:{data_s}")
    chunks = [data_s[i:i+2] for i in range(0, len(data_s), 2)]# ascii文字列(hex表記)を2文字ごとに区切る．ex,"5249..."→"52","49",...
    data_bin_list = [int(s,16).to_bytes((int(s,16).bit_length() + 7) // 8, byteorder='big') for s in chunks]# ascii文字列(hex表記)をintに変換してbytes型に変換.ex,"52","49",...→52,49,...
    data_bin_row = b''.join([b if b != b'' else b'\x00' for b in data_bin_list])# バイナリデータ列にする．0x00が空バイトとしてエンコードされているので修正.ex,52,49,...→5249...
    return data_bin_row


isReceiving = False
start_t = time.time()
# POSTリクエストを処理するエンドポイント
# 分岐1: LINE送信
# if COMMAND == 0:
# @app.route('/postData', methods=['POST'])
# def sendLINE():
#     send_message_to_user(LINE_API_TOKEN, LINE_USER_ID, "警戒通知")
#     return f"", 200
# # 分岐2: ChatGPT
# elif COMMAND == 1:
@app.route('/postData', methods=['POST'])
def receive_data():
    global isReceiving
    global start_t

    if isReceiving == False:
        start_t = time.time()
        isReceiving = True

    # クライアントから送られたデータを取得
    data_b = request.get_data()  # 生データ
    print(f"Received {len(data_b)} B data")
    data_b_hex = data_b.hex()#生データ(hex表記文字列).
    print(data_b_hex)# ex,35323439...
    print(f"Received POST data(hex): {data_b_hex}")

    if(data_b.decode() == "end"):
        passed_time = time.time()-start_t
        print(f"TIME:{passed_time}")
        isReceiving = False
        # バイナリデータをファイルに書き込む
        print(os.getcwd())
        save_dir_path = ''
        file_name = MP3_FILE_PATH
        print("saving...")
        with open(os.path.join(save_dir_path, file_name), 'wb') as f:
            for packets in received_packets:
                f.write(packets)
        print("DONE!!")
        received_packets.clear()

    else:
        data_bin_row = body_to_bin(data_b)#変換
        received_packets.append(data_bin_row)
        print(data_bin_row)
    

    # クライアントにレスポンスを返す
    # return f"Received your data: {data_b}", 200
    return f"", 200


# 分岐3: なし(タイムアウト)

if __name__ == '__main__':
    # サーバーを0.0.0.0にバインドしてすべてのIPからの接続を許可
    app.run(host='0.0.0.0', port=5000)