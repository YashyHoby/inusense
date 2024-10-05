# InuSense

## 環境構築（書きなぐり）
### Spresense SDK
[Spresense SDK スタートガイド] (https://developer.sony.com/spresense/development-guides/sdk_set_up_ide_ja.html)に従い、Spresense VSCodeIDEのセットアップとワークスペースの作成を行う。

開発環境のファイル構造。(プロジェクトフォルダ名:inusense)
``` tree
user/
 ├spresenseenv/
 │ └ windowでsdkを動かすためのあれこれ？
 ├spresense/
 │ └ spresense SDKのツール達
 ├inusense/
 │ └ プロジェクトフォルダ（削除してよい）
 ├inusense.code-workspase
 ├
```

### Git連携
spresenseのプロジェクトフォルダを共有するためのGit連携手順。
#### userディレクトリ下でGit初期化
```
git init
```
#### リモートリポジトリをローカルに追加（SSHでも可）
```
git clone https://github.com/YashyHoby/InuSense.git
```
#### クローンしたプロジェクトフォルダ（InuSense）をworkspaceに追加
エクスプローラでフォルダを開き、vscodeで開いているworkspaceにドラックドロップ

