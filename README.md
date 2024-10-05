# InuSense

## 環境構築（書きなぐり）
### Spresense SDK開発環境の設定
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
spresenseのプロジェクトフォルダを共有するためのGit連携手順。[Spresense SDK開発環境の設定](# Spresense SDK開発環境の設定)に従って作成したワークスペースに、このプロジェクトを読み込ませるだけでよい。
wsl環境へのGitのアカウント紐づけは各自で行ってください。
#### userディレクトリ下でリモートリポジトリをローカルに追加（SSHでも可）
```
git clone https://github.com/YashyHoby/inusense.git
```
#### クローンしたプロジェクトフォルダ（InuSense）をworkspaceに追加
エクスプローラでフォルダを開き、vscodeで開いているworkspaceにドラックドロップ

### 作業ブランチの設定
開発用ブランチdevelopから作業ブランチを作成して開発を進めてください
#### developブランチに切り替え
```
git checkout develop
```
#### 作業ブランチ作成
```
git branch feature/*****
```
#### 作業ブランチに切り替え
```
git checkout feature/*****
```

### メモ
#### ブランチ削除
```
git branch -d feature/*****
```
