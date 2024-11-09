from flask import Flask, request
import os

app = Flask(__name__)
UPLOAD_FOLDER = 'received_files'

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return "ファイルが見つかりません", 400

    file = request.files['file']
    if file.filename == '':
        return "ファイルが選択されていません", 400

    # 保存先ディレクトリが存在しない場合は作成
    if not os.path.exists(UPLOAD_FOLDER):
        os.makedirs(UPLOAD_FOLDER)

    save_path = os.path.join(UPLOAD_FOLDER, file.filename)
    file.save(save_path)
    return "ファイルのアップロードに成功しました", 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
