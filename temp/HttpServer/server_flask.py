from flask import Flask,request,Response, jsonify
import os
import time
from send_message import send_message_to_user
# import sys
# sys.path.append(os.getcwd())
from generate_response_from_audio import m

from dotenv import load_dotenv
load_dotenv()

LINE_API_TOKEN_PATH = os.environ["CHANNEL_ACCESS_TOKEN"]
CHANNEL_SECRET = os.environ["CHANNEL_SECRET"]
USER_ID = os.environ["USER_ID"]


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
@app.route('/getLINE', methods=['GET'])
def sendLINE():
    send_message_to_user(LINE_API_TOKEN_PATH, USER_ID, "警戒通知")

@app.route('/getReply', methods=['GET'])
def generate_reply():
    try:
        text = m()  # 音声の処理やChatGPTへのリクエストを実行する関数
        f = open('answer.txt', 'w')
        f.write(text)
        f.close()
        return jsonify({"message": "処理が成功しました"})  # 成功レスポンスを返す
    except Exception as e:
        print(f"Error occurred: {e}")
        return jsonify({"error": str(e)}), 500  # エラーをJSONレスポンスとして返す
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
#     send_message_to_user(LINE_API_TOKEN_PATH, USER_ID, "警戒通知")
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
        save_dir_path = './'
        file_name = 'question.mp3'
        if not os.path.exists(save_dir_path+file_name):
            print("saving...")
            with open(save_dir_path+file_name, 'wb') as f:
                for packets in received_packets:
                    f.write(packets)
            print("DONE!!")
        else:
            print("file already exists.")
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
    app.run(host='0.0.0.0', port=3000)
