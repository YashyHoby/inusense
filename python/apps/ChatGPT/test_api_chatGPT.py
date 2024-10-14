import openai
from openai import RateLimitError, Timeout, APIError, APIConnectionError, OpenAIError
import datetime

CHATGPT_API_KEY_PATH = "../../../config/openai/chatGPT_api.txt"
FORMAT = """
        speak japanese
        """

def read_keyFile():
    try:
        with open(CHATGPT_API_KEY_PATH, 'r', encoding='utf-8') as file:
            key = file.readline().strip()
        return key
    except FileNotFoundError:
        return "ファイルが見つかりません。"
    except Exception as e:
        return f"エラーが発生しました: {e}"

def main():
    openai.api_key = read_keyFile()

    question = input("質問内容：")
    dt_now = str(datetime.datetime.now())
    
    try:
        # 正しいAPIエンドポイントを使用してAPIキーの検証を行います
        response = openai.chat.completions.create(
            model="gpt-4o-mini",
            messages=[
                {"role": "system", "content": FORMAT},
                {"role": "user", "content": question}
            ]
        )
        print("APIキーが正しく認識されています。")
        print(response.choices[0].message.content)
    except openai.AuthenticationError:
        print("APIキーが無効です。正しいAPIキーを設定してください。")
    except openai.RateLimitError:
        print("レートリミットに達しました。後でもう一度試してください。")
    except openai.OpenAIError as e:
        print(f"OpenAIのエラーが発生しました: {e}")


main()
