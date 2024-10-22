import json

# 保存先のファイル名
CHAT_HISTORY_FILE = "user_messages.json"

# メッセージを保持するリスト
messages = []

# ファイルから履歴を読み込む（初期化）
def load_chat_history():
    try:
        with open(CHAT_HISTORY_FILE, 'r', encoding='utf-8') as f:
            return json.load(f)
    except FileNotFoundError:
        return []  # ファイルがなければ空のリストを返す

# ファイルに履歴を保存する
def save_chat_history(messages):
    with open(CHAT_HISTORY_FILE, 'w', encoding='utf-8') as f:
        json.dump(messages, f, ensure_ascii=False, indent=4)

def main():
    # 過去の履歴を読み込み
    messages = load_chat_history()

    while True:
        # ユーザーからの入力を受け取る
        user_input = input("メッセージを入力してください (終了するには 'q' を入力): ")

        # 'q' が入力された場合、ループを終了
        if user_input.lower() == 'q':
            print("終了します...")
            break

        # ユーザーのメッセージを追加
        new_message = {"role": "user", "content": user_input}
        messages.append(new_message)

        # 最新のメッセージリストをファイルに保存
        save_chat_history(messages)

        # 現在のメッセージ履歴を表示
        print("現在のメッセージ履歴:", messages)

if __name__ == "__main__":
    main()
