# ピストン

PTTUNEおよび PTCOPファイル形式で音楽を再生および/または編集するためのライブラリ。

Pixelが開発したライブラリ: https://pxtone.org/developer/

# これは何ですか？
このライブラリは、いくつかの新機能を追加し、ビッグエンディアンプロセッサ（PowerPC や MIPS など）のサポートを提供するために修正されたバージョン 220910a です。

# どのような変更が行われていますか?
## バグの修正
* **[pxtnPulse_Noise::Compare の比較コードを修正しました](https://github.com/Wohlstand/libpxtone/commit/d0c363d1f57ef2318bb91f2f72382d2dfed44e1f)**
  * すべてのイベント リストが自分自身と比較されているために「常に等しい」状態になってしまうバグがありました。
* **[pxtoneNoise::init() でのオブジェクトの削除を修正しました](https://github.com/Wohlstand/libpxtone/commit/3513ff46789a873357557f6dd93f34dcc0c79611)**
  * 「`new`」演算子で作成されたオブジェクトはすべて「`delete`」演算子で削除する必要があります。　そうしないと、メモリ リークが発生する可能性があります。

## 研磨
* **[末尾の空白をすべて削除しました](https://github.com/Wohlstand/libpxtone/commit/7cc9a465b960abc4b8d44d30c345d452c51437bb)**
  * 行末にスペースがあります。　基本的には、git の違いを混乱させ、ファイル サイズを肥大化させる役に立たないバラストです。　多くの IDE やテキスト エディタには、末尾のスペースを削除する機能などがあります。
* **修正された警告**
  * [第一](https://github.com/Wohlstand/libpxtone/commit/f30e17df4602df032679d5a3e66d6edc8c772738)
    * 列挙型の末尾のカンマは一部のコンパイラで適切に処理されず、警告が発生します。
    * 符号付きと符号なしの比較。
    * 未使用の引数。
    * 「switch」にデフォルトのブランチがありません。
  * [GCC は、括弧を追加せずに「if」句の後に複数のステートメントがある場合、誤解を招くインデントについて警告を出します](https://github.com/Wohlstand/libpxtone/commit/8c71dbccf593905b9024f79024e3c3de0b66934a)
  * [より安全な方法を使用して浮動小数点数を符号なし整数としてキャストする](https://github.com/Wohlstand/libpxtone/commit/775b8e6221a14b08324e142b77da90c0dea54872)
    * 最初の方法では、「type-punned pointer will Break a strict-aliasing rules」という警告が表示されます。 新しい方法では「union」を使用することで、同じことをより安全に行うことができます :smile:.

## ハードウェアサポート
* **[プレーンな「int」を「int32_t」に置き換えました。](https://github.com/Wohlstand/libpxtone/commit/4c2b7c2a680a150107d370fb257c6b2265aa80b7)**
  * 一部のツールチェーンでは、int32_t は実際には「long int」であるため、コンパイル エラーが発生します。
* **[ビッグエンディアンプロセッサのサポートを追加](https://github.com/Wohlstand/libpxtone/commit/7314fc157ad55940e40bc62ad301eb11e72dd395)**
  * PowerPCプロセッサを搭載した古い Macなど、ビッグ エンディアン バイト オーダーを持つプロセッサ上でライブラリが動作できるようにします。　ビッグ エンディアンでプロジェクトをコンパイルする場合は、「`-Dpx_BIG_ENDIAN`」マクロを指定する必要があります。 ライブラリを正しく動作させるためのハードウェア。

## 特徴
* **[テンポマルチプライヤパラメータを追加](https://github.com/Wohlstand/libpxtone/commit/f0b2118deda068f86edf7076ff60ab3d5fa652f8)**
  * これは、現在再生中の曲のテンポをその場で変更するために使用できる要素です。 この機能はさまざまなアクションに使用されます。
* **[再生するループ数を指定するオプションを追加しました](https://github.com/Wohlstand/libpxtone/commit/314a09d02ea8c7674dff73535a58ea307ba77b01)**
  * 曲をループする回数を指定できるようになりました。
* **[ループの開始点 (「repeat」) を取得するオプションを追加しました](https://github.com/Wohlstand/libpxtone/commit/e36599684189a0b23a714f936a711a102436b280)**
  * 音楽ファイルの再生時にループ範囲を指定できます。

## OGG Vorbis サポートの変更
* [OGG Vorbis での静的コールバックを除外する](https://github.com/Wohlstand/libpxtone/commit/b3e130dcaf637a6d5b18ce416ed5f357a611be6d)
  * これにより、Vorbis ヘッダーでの未使用の静的コールバックによって発生する別の警告が修正されます。
* [Tremor のサポートを追加しました](https://github.com/Wohlstand/libpxtone/commit/d8010e7b5019865dbe0413590a432cd016f88f54)
  * これは OGG Vorbis の整数のみの実装であり、FPU がまったくないハードウェア、または効率の悪い FPU を備えたハードウェア向けに設計されています。
* [stb_vorbis の使用を追加](https://github.com/Wohlstand/libpxtone/commit/9d3ff1e96c042050420e02f890fc7e9c39c11d62)
  * これは、OGG Vorbis のヘッダーのみの実装です。 　OGG Vorbis ライブラリの完全なセットがなくても使用できます。
* [A不足しているエラーコード割り当てを追加しました](https://github.com/Wohlstand/libpxtone/commit/29365516f993fc3871aaf83dd6e9bfc1ba38305a)
  * この変更により、実際のエラー理由ではなく「VOID」エラーが返される問題が修正されました。

「`-DpxINCLUDE_OGGVORBIS`」とともに、「`-DpxINCLUDE_OGGVORBIS_TREMOR`」マクロを指定してTremor(整数のみ)実装を使用することも、「`-DpxINCLUDE_OGGVORBIS_STB`」マクロを指定して、　ここのプロジェクトに含まれているヘッダーのみの stb_vorbis 実装を使用することもできます。
