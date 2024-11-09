from flask import Flask, request
import os

app = Flask(__name__)

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return "ファイルが見つかりません", 400

    file = request.files['file']
    if file.filename == '':
        return "ファイルが選択されていません", 400

    save_path = os.path.join('received_files', file.filename)
    file.save(save_path)
    return "ファイルのアップロードに成功しました", 200

if __name__ == '__main__':
    if not os.path.exists('received_files'):
        os.makedirs('received_files')
    app.run(host='0.0.0.0', port=5000)
