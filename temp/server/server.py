from flask import Flask, request

app = Flask(__name__)

@app.route('/postData', methods=['POST'])
def receive_mp3():
    with open("received_file.mp3", "wb") as f:
        f.write(request.data)
    return "File received", 200

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=10080)
