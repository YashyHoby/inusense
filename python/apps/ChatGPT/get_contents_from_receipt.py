# %%
import base64
import openai
from PIL import Image
import matplotlib.pyplot as plt
import PySimpleGUI as sg
from PIL import Image, ImageTk
import os
import tkinter as tk
from tkinter import filedialog
import json
from datetime import datetime

CHATGPT_API_KEY_PATH = "../../../config/openai/chatGPT_api.txt"
FORMAT = """
Read the receipt image and return the information in the following format:
{"items":[{"name":, "quantity":, "price":, "total":}, ...], "summary": {"amount":, "tax":, "tax amount":, "total tax":} }
If the image is not a readable receipt, return {}. Do not include any other text in the response.
"""

# ChatGPTのAPIキーを取得
def read_keyFile():
    try:
        with open(CHATGPT_API_KEY_PATH, 'r', encoding='utf-8') as file:
            key = file.readline().strip()
        return key
    except FileNotFoundError:
        return "ファイルが見つかりません。"
    except Exception as e:
        return f"エラーが発生しました: {e}"

# 画像をbase64にエンコードする関数
def encode_image(image_path):
    with open(image_path, "rb") as image_file:
        return base64.b64encode(image_file.read()).decode("utf-8")

# ファイルを選択する
def select_file():
    # メインウィンドウを隠す
    root = tk.Tk()
    root.withdraw()

    # ファイルダイアログを開く
    file_path = filedialog.askopenfilename(
        initialdir="../images",
        title="ファイルを選択",
        filetypes=[("すべてのファイル", "*.*"), ("画像ファイル", "*.jpg;*.jpeg;*.png")]
    )

    if file_path:
        return file_path
    else:
        print("ファイルが選択されませんでした")
        os.sys.exit()

def show_image(file_path):
    # 新しいウィンドウを作成
    window = tk.Toplevel()
    window.title("確認")

    # ウィンドウが閉じられたときにプログラム全体を終了するように設定
    window.protocol("WM_DELETE_WINDOW", lambda: os.sys.exit())

    # キャンバスを作成
    canvas = tk.Canvas(window, width=500, height=800)
    canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

    # 垂直スクロールバーを作成
    v_scrollbar = tk.Scrollbar(window, orient=tk.VERTICAL, command=canvas.yview)
    v_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

    canvas.configure(yscrollcommand=v_scrollbar.set)

    # 画像を開く
    img = Image.open(file_path)
    width, height = img.size
    img = img.resize((400, int((400 * height) / width)))
    img_tk = ImageTk.PhotoImage(img)

    # キャンバスに画像を表示
    canvas.create_image(0, 0, anchor=tk.NW, image=img_tk)
    canvas.config(scrollregion=canvas.bbox(tk.ALL))

    # ボタンフレームを作成
    button_frame = tk.Frame(window)
    button_frame.pack()

    # OKボタンを作成
    ok_button = tk.Button(button_frame, text="OK", command=window.quit)
    ok_button.pack(side=tk.LEFT)

    # キャンセルボタンを作成
    cancel_button = tk.Button(button_frame, text="キャンセル", command=lambda: os.sys.exit())
    cancel_button.pack(side=tk.LEFT)

    window.mainloop()



# ChatGPTでレシートを解析。指定の書式で出力
def analysis_receiptImage_byChatGPT(receiptImage):
    # 画像をbase64にエンコードする
    base64_image = encode_image(receiptImage)

    # OpenAI APIのクライアントを作成する
    openai.api_key = read_keyFile()

    # チャットの応答を生成する
    try:
        response = openai.chat.completions.create(
            model="gpt-4o",
            messages=[
                {
                    "role": "user",
                    "content": [
                        {"type": "text", "text": FORMAT}, 
                        {
                            "type": "image_url",
                            "image_url": {
                                "url": f"data:image/jpeg;base64,{base64_image}"
                            }
                        }
                    ]
                }
            ],
            max_tokens=1000,
        )
        #print("APIが例外なく動作しました")
        return response.choices[0].message.content

    except openai.AuthenticationError:
        print("APIキーが無効です。正しいAPIキーを設定してください。")
        os.sys.exit()
    except openai.RateLimitError:
        print("レートリミットに達しました。後でもう一度試してください。")
        os.sys.exit()
    except openai.OpenAIError as e:
        print(f"OpenAIのエラーが発生しました: {e}")
        os.sys.exit()

# JSONデータをファイルに保存
def save_jsonData(data):
    now = datetime.now()
    timestamp = now.strftime("%Y%m%d_%H%M%S")
    filename = f"./json/receipt_{timestamp}.json"
    with open(filename, 'w', encoding='utf-8') as file:
        json.dump(str(data), file, ensure_ascii=False, indent=4)

def main():
    # 画像のパス
    original_image_path = select_file()

    # 画像確認画面
    show_image(original_image_path)

    # レシート読み取り
    product_data = analysis_receiptImage_byChatGPT(original_image_path)

    save_jsonData(product_data)

if __name__ == "__main__":
    main()
