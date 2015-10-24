# StereoBlockMatch

2台のWEBカメラの画像で、ステレオマッチングを行い、結果をリアルタイム表示するプログラム。
生の結果画像に加え、過去数フレームの平均画像も表示します。

ステレオマッチングには OpenCV の StereoSGBM を使用。

[![StereoBlockMatch](http://cdn-ak.f.st-hatena.com/images/fotolife/t/takamints/20151013/20151013115218.png)](http://takamints.hatenablog.jp/entry/result-of-opencv-stereo-block-matching-with-calibrated-cameras)

## 実行モジュール

1. StereoBlockMatch - カメラ2台のステレオマッチング結果をリアルタイム表示
2. stereoShot - ステレオカメラのキャリブレーション用画像保存ツール

### 1. StereoBlockMatch

```
$ StereoBlockMatch [-R <devIndex>(=1)] [-L <devIndex>(=2)] [-C <calPrefix>]
```

#### オプション

* `-R <devIndex>` - 右側カメラのデバイスインデックス。既定値は1。0以上の数値を指定します。
* `-L <devIndex>`- 左側カメラのデバイスインデックス。既定値は2。0以上の数値を指定します。
* `-C <calPrefix>` - stereoShotを使用して保存した、キャリブレーション用の画像ファイル群のプレフィックス(ファイル名の先頭文字列)を指定します。このオプションを指定しなければキャリブレーションは行われません。既定値はありません。ファイル名のフォーマットについては、stereoShotの説明を参照してください。

以下の例では、デバイスインデックスを、右を2、左を1に変更しています。

`$ ./StereoBlockMatch -R2 -L1`

#### 操作

実行すると、左右のカメラ画像と視差画像、そして視差画像の移動平均画像を表示するウィンドウが開きます。キャリブレーションは、画面表示の前に行われます。

終了するには、[Shift]+[Q]を押します。

### 2. stereoShot

```
$ stereoShot [-R <devIndex>(=1)] [-L <devIndex>(=2)] [-C <calPrefix>=("cal")]
```

#### オプション

* `-R <devIndex>` - 右側カメラのデバイスインデックス。既定値は1。0以上の数値を指定します。
* `-L <devIndex>`- 左側カメラのデバイスインデックス。既定値は2。0以上の数値を指定します。
* `-C <calPrefix>` - キャリブレーション用の画像ファイルのファイル名先頭文字を指定します。既定値は"cal"。保存されるファイル名は、`<calPrefix>_<seqno>_<RL>.jpg`。calPrefixには、ディレクトリ名を含んで良い。seqnoは0から始まる3桁の数字、RLは、'R'で右のカメラ画像をあらわし、'L'は左です。

#### 操作

実行すると、左右のカメラ画像が表示されます。キャリブレーションに必要な11x9のチェスボード画像を両方のカメラに移すと、その内部のコーナーを検出して画面に表示します。両方の画像に検出されたコーナーが表示されているときに、スペースバーを押すと、左右の画像に連番をつけて保存します。

終了するには、[Shift]+[Q]を押します。

## ビルド・実行に必要なもの

* USBカメラ2台（同機種が望ましい）
* OpenCV 2.4
* cmake 2.8 / 3.3
* [cvImagePipeline](https://github.com/takamin/cvImagePipeline) (次項参照)
* opencv 2.4 ※cmakeのfind\_packageで検索されます。

## ビルド

### cvImagePipeline について

OpenCVを利用した画像処理ライブラリ [cvImagePipeline](https://github.com/takamin/cvImagePipeline)
を使用しています。

このリポジトリに Gitサブモジュールとして追加していますので、このリポジトリを clone したのち、
`git submodule update cvImagePipeline` で更新してください。


### ビルド方法

ビルドはcmakeで行います。
リポジトリのルートディレクトリで以下を実行。

```
mkdir build
cd build
cmake ..
```

### 出力フォルダ

Windowsの場合、cmakeが終了した後、出力されたソリューションファイルを Visual C++ で開き、
__Release版でINSTALLターゲットをビルド__してください。
おそらくDebug版では動作しません。

実行モジュールは、
プログラムフォルダ
（`C:\\Program Files` または、
`C:\\Program Files (x86)`）の
`StereoBlockMatch`へ出力されています。

## 動作確認した環境

動作確認は以下の環境で行いました。

* Windows 7 (32bit) SP1
* Microsoft Visual C++ 2010 Express
* Microsoft Visual Studio Community 2015 Visual C++ 2015
* 百均で買った2台のUSBカメラ
* OpenCV 2.4.10
* cvImagePipeline 1.0

