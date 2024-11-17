from flask import Flask,request,Response, abort
import os
import time
from linebot.v3 import WebhookHandler
from linebot.v3.exceptions import InvalidSignatureError
from linebot.v3.messaging import (
	ApiClient, Configuration, MessagingApi,
	ReplyMessageRequest, PushMessageRequest,
	TextMessage, PostbackAction
)
from linebot.v3.webhooks import (
	FollowEvent, MessageEvent, PostbackEvent, TextMessageContent
)
from dotenv import load_dotenv
load_dotenv()

CHANNEL_ACCESS_TOKEN = os.environ["CHANNEL_ACCESS_TOKEN"]
CHANNEL_SECRET = os.environ["CHANNEL_SECRET"]

app = Flask(__name__)

configuration = Configuration(access_token=CHANNEL_ACCESS_TOKEN)
handler = WebhookHandler(CHANNEL_SECRET)
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
# def send_chunked_file():
#     file_path = '../test.txt'  # または 'test.txt', 'test.mp3' など送信したいファイル
#     if not os.path.exists(file_path):
#         return Response("File not found", status=404)

#     def generate():
#         with open(file_path, 'rb') as f:
#             while chunk := f.read(10):  # 10バイトずつ読み込む
#                 yield chunk
#         yield b'end'  # 終了マーカー

#     return Response(generate(), content_type='application/octet-stream')
# def send_audio_file():
#     audio_file_path = '../test.mp3'
#     with open(audio_file_path, 'rb') as f:
#         while True:
#             data = f.read(10)
#             print(data)
#             if not data:
#                 break
#             yield data
#     return Response(data, content_type='application/octet-stream')
# def send_audio_file():
#     audio_file_path = 'random_bytes.bin'
#     def generate():
#         with open(audio_file_path, 'rb') as f:
#             while True:
#                 data = f.read(10)
#                 print(data)
#                 if not data:
#                     break
#                 yield data
#     return Response(generate(), content_type='application/octet-stream')
# def send_binary_file():
#     binary_data = b'\x52\x49\x46\x46\x24\x08\x00\x00\x57\x41\x56\x45'
#     def generate():
#         for i in range(0, len(binary_data), 10):
#             chunk = binary_data[i:i + 10]
#             yield chunk.hex()  # 16進文字列にエンコードして送信
#     return Response(generate(), content_type='application/octet-stream')


# パケットを一時的に保存するリスト
received_packets = []

def body_to_bin(data_b: bytes):
    data_s = data_b.decode()  # bin to ASCII文字列(hex表記)
    print(f"decoded: {data_s}")
    
    if len(data_s) % 2 != 0:
        data_s = '0' + data_s  # 奇数長の場合、先頭に0を補完
    
    chunks = [data_s[i:i+2] for i in range(0, len(data_s), 2)]  # 2文字ごとに区切る
    print(f"chunks: {chunks}")
    
    data_bin_list = []
    for s in chunks:
        try:
            data_bin_list.append(
                int(s, 16).to_bytes((int(s, 16).bit_length() + 7) // 8, byteorder='big')
            )
        except ValueError:
            print(f"Invalid chunk: {s}")
            data_bin_list.append(b'\x00')  # 無効なデータを0x00に置き換える
    
    data_bin_row = b''.join(data_bin_list)
    return data_bin_row


isReceiving = False
start_t = time.time()
# POSTリクエストを処理するエンドポイント
@app.route('/postData', methods=['POST'])
@app.route("/callback", methods=['POST'])
def callback():
	# get X-Line-Signature header value
	signature = request.headers['X-Line-Signature']

	# get request body as text
	body = request.get_data(as_text=True)
	app.logger.info("Request body: " + body)

	# handle webhook body
	try:
		handler.handle(body, signature)
	except InvalidSignatureError:
		app.logger.info("Invalid signature. Please check your channel access token/channel secret.")
		abort(400)

	return 'OK'

@handler.add(FollowEvent)
def handle_follow(event):
	## APIインスタンス化
	with ApiClient(configuration) as api_client:
		line_bot_api = MessagingApi(api_client)

	## 返信
	line_bot_api.reply_message(ReplyMessageRequest(
		replyToken=event.reply_token,
		messages=[TextMessage(text='Thank You!')]
	))
     

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
        save_dir_path = 'C:/MyProgram/ArduinoPrograms/inusense/server/python/apps/HttpServer/'
        file_name = 'recv_nyan.jpg'
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


if __name__ == '__main__':
    # サーバーを0.0.0.0にバインドしてすべてのIPからの接続を許可
    app.run(debug=True, host='0.0.0.0', port=3000)
