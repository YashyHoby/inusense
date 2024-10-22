from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route("/webhook", methods=['POST'])
def webhook():
    try:
        # LINEからのリクエストデータを取得
        body = request.get_json()

        # イベントの内容をログ出力
        print(f"Received event: {body}")

        # 必要なデータを処理（例: ユーザーIDを取得して何かアクション）
        for event in body['events']:
            user_id = event['source']['userId']
            message_text = event['message']['text'] if 'message' in event else None

            print(f"User ID: {user_id}, Message: {message_text}")

        # LINEにレスポンスを返す（200 OK）
        return "OK", 200
    except Exception as e:
        print(f"Error: {e}")
        return "Internal Server Error", 500

if __name__ == "__main__":
    app.run(port=5000)
