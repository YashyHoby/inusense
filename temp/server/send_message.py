import requests
import os
from dotenv import load_dotenv
load_dotenv("../../.hisaenv")

line_api_token = os.getenv('LINE_API_TOKEN')
line_user_id = os.getenv('LINE_USER_ID')

line_api_token = os.getenv('LINE_API_TOKEN')
line_user_id = os.getenv('LINE_USER_ID')

# アクセストークンが有効か確認
def check_access_token(access_token):
    headers = {'Authorization': f'Bearer {access_token}'}
    url = 'https://api.line.me/v2/bot/info'
    response = requests.get(url, headers=headers)
    
    if response.status_code == 200:
        print('アクセストークンは有効です。')
        print(response.json())  # ボットのプロファイル情報を表示
    else:
        print('アクセストークンが無効、またはリクエストが不正です。')
        print(response.status_code, response.text)  # エラーコードとメッセージを出力

# メッセージをブロードキャスト
def send_message(access_token, message):
    headers = {'Authorization': f'Bearer {access_token}',
            'Content-Type': 'application/json'
            }
    url = 'https://api.line.me/v2/bot/message/broadcast'
    response = requests.get(url, headers=headers)

    data = {
        "messages": [
            {
                "type": "text",  # メッセージのタイプを "text" に指定
                "text": message  # 送信するメッセージの内容を指定
            }
        ]
    }

    response = requests.post(url, 
                            headers=headers, 
                            json=data
                            )
    
    if response.status_code == 200:
        print("メッセージが正常に送信されました。")
    else:
        print(f"エラー: {response.status_code}, {response.text}")

# メッセージを指定したユーザに送信
def send_message_to_user(access_token, user_id, message):
    headers = {
        'Authorization': f'Bearer {access_token}',
        'Content-Type': 'application/json'
    }
    url = 'https://api.line.me/v2/bot/message/push'

    data = {
        "to": user_id,  # 指定したユーザーID
        "messages": [
            {
                "type": "text",  # メッセージのタイプを "text" に指定
                "text": message  # 送信するメッセージの内容を指定
            }
        ]
    }

    # POSTリクエストでメッセージを送信
    response = requests.post(url, headers=headers, json=data)

    if response.status_code == 200:
        print("メッセージが正常に送信されました。")
    else:
        print(f"エラー: {response.status_code}, {response.text}")

def main():
    #check_access_token(access_token)
    #send_message(access_token, "hello")
    send_message_to_user(line_api_token, line_user_id, "wa")


#main()