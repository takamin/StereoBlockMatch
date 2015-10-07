# StereoBlockMatch

2台のWEBカメラの画像で、ステレオマッチングを行い、結果をリアルタイム表示するプログラム。
生の結果画像に加え、過去数フレームの平均画像も表示します。

ステレオマッチングには OpenCV の StereoSGBM を使用。

opencvのキャプチャーデバイスインデックスは、既定では、右のカメラが1、左のカメラが2です。それぞれ、-R、-Lのオプションで変更できます。

以下の例では、デバイスインデックスを、右を2、左を1に変更しています。

`$ ./StereoBlockMatch -R2 -L1`


## ビルド・実行に必要なもの

* USBカメラ2台（同機種が望ましい）
* OpenCV 2.4
* cmake 2.8 / 3.3
* [cvImagePipeline](https://github.com/takamin/cvImagePipeline) (次項参照)
* opencv 2.4 ※cmakeのfind\_packageで検索されます。

## cvImagePipeline

OpenCVを利用した画像処理ライブラリ [cvImagePipeline](https://github.com/takamin/cvImagePipeline)
を使用しています。

このリポジトリに Gitサブモジュールとして追加していますので、このリポジトリを clone したのち、
`git submodule update cvImagePipeline` で更新してください。


## ビルド方法

ビルドはcmakeで行います。
リポジトリのルートディレクトリで以下を実行。

```
mkdir build
cd build
cmake ..
```

## 出力フォルダ

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

