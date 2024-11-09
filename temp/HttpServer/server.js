// HTTPモジュールを読み込む
const http = require('http');

// サーバーを作成
const server = http.createServer((req, res) => {
  // レスポンスヘッダーを設定
  res.statusCode = 200; // ステータスコード: OK
  res.setHeader('Content-Type', 'text/plain');

  // レスポンスボディを送信
  res.end('Hello, World!\n');
});

// ポート番号を指定
const port = 3000;

// サーバーをポート3000で起動
server.listen(port, () => {
  console.log(`Server running at http://localhost:${port}/`);
});
