from flask import Flask,request

app = Flask(__name__)

# GETリクエストを処理するエンドポイント
@app.route('/', methods=['GET'])
def hello():
    return "Hello from Flask!"

# POSTリクエストを処理するエンドポイント
@app.route('/postData', methods=['POST'])
def receive_data():
    # クライアントから送られたデータを取得
    # data = request.get_data(as_text=True)  # 生のデータをテキスト形式で取得
    data = request.get_data()  # 生のデータをテキスト形式で取得
    data_hex = data.hex()
    print(data_hex)
    print(f"Received POST data: {data_hex}")
    
    # クライアントにレスポンスを返す
    return f"Received your data: {data}", 200

if __name__ == '__main__':
    # サーバーを0.0.0.0にバインドしてすべてのIPからの接続を許可
    app.run(host='0.0.0.0', port=3000)
