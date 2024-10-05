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
 │ └ プロジェクトフォルダ（gitで共有します）
 ├inusense.code-workspase
 ├
```

### Git連携
spresenseのプロジェクトフォルダを共有するためのGit連携手順。
#### プロジェクトフォルダ内でGit初期化
```
git init
```
#### Gitのユーザ名とメールアドレスを設定
```
git config --global user.name <username>
git config --global user.email <user@sample.com>
```
#### リモートリポジトリをローカルに追加
```

```

