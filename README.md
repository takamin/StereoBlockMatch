# StereoBlockMatch

2台のWEBカメラの画像で、ステレオマッチングを行い、結果をリアルタイム表示するプログラム。
生の結果画像に加え、過去数フレームの平均画像も表示します。

ステレオマッチングには OpenCV の StereoSGBM を使用。

## ビルド・実行に必要なもの

* USBカメラ2台（同機種が望ましい）
* [cvImagePipeline](https://github.com/takamin/cvImagePipeline)
* OpenCV 2.4
* cmake 2.8

## ビルド方法

ビルドはcmakeで行います。

1. cvImagePipelineのビルドと出力ファイルの配置
2. cmakeでビルド

### cvImagePipelineの出力ファイルの配置

(以下、Windowsの場合の説明です)

cvImagePipelineのRelease版の出力ファイルやヘッダファイルを以下の用に配置してください。
また、dllを配置したフォルダにPATHを通しておいてください。

```
C:/cvImagePipeline/
 +- include/
 |   +- cvImagePipeline.h
 |   |  ...
 |   |  ...
 |   +- ...
 +- cvImagePipeline.dll
 +- cvImagePipeline.lib
```

### cmakeでビルド

Windowsなら、以下のような感じでソリューションファイルが出来上がります。

```
mkdir build
cd build
cmake -D OpenCV_DIR="C:/opencv" ..
```

Visual C++ でソリューションを開いて、__Release版でビルド__してください。Debug版では動作しません。

## 動作確認した環境

動作確認は以下の環境で行いました。

* Windows 7 (32bit) SP1
* Microsoft Visual C++ 2010 Express
* 百均で買った2台のUSBカメラ
* OpenCV 2.4.10
* cvImagePipeline 1.0

