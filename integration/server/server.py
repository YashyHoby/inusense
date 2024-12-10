from http.server import HTTPServer
from http.server import BaseHTTPRequestHandler


class CustomHTTPRequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/plain; charset=utf-8')
        self.end_headers()
        self.wfile.write('server.py:GET requested'.encode())

    def do_POST(self):
        self.send_response(200)
        self.send_header('Content-Type', 'text/plain; charset=utf-8')
        self.end_headers()
        print(self.rfile.read(int(self.headers['content-length'])).decode('utf-8'))
        html_context = "送信完了"
        self.wfile.write(html_context.encode())

port = 3000
server_address = ('localhost', port)
httpd = HTTPServer(server_address, CustomHTTPRequestHandler)
httpd.serve_forever()