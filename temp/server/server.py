from flask import Flask, request
import os

app = Flask(__name__)
UPLOAD_FOLDER = 'uploads'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.route('/upload', methods=['POST'])
def upload_file():
    if request.method == 'POST':
        file_data = request.data
        file_path = os.path.join(UPLOAD_FOLDER, 'received_file.mp3')
        with open(file_path, 'wb') as f:
            f.write(file_data)
        return 'File received successfully', 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
