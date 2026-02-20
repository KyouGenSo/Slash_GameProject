#!/usr/bin/env python3
"""
fix_jp_en_spacing.py - C++ コメント内の日本語↔英語境界にスペースを自動挿入

VS2022 のスペルチェッカーが日本語と英語の隣接（例: "のfar", "はtrue"）を
1トークンとして誤認識し偽警告を出す問題を解決するスクリプト。

和欧混植の慣習に従い、コメント部分のみ境界スペースを挿入する。
コード部分（変数名・文字列リテラル等）は一切変更しない。
"""

import argparse
import io
import os
import re
import sys
from pathlib import Path

# Windows コンソール (cp932) で UTF-8 出力を保証
if sys.stdout.encoding and sys.stdout.encoding.lower() != 'utf-8':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', errors='replace')

# ===== 日本語・英語文字クラス定義 =====
# ひらがな + カタカナ + カタカナ長音 + 漢字繰り返し記号 + CJK統合漢字
JP = r'[\u3041-\u3093\u30A1-\u30F6\u30FC\u3005\u4E00-\u9FFF]'
# ASCII英字のみ（数字は対象外 — "256個" や "フェーズ2" はそのまま）
EN = r'[a-zA-Z]'


def find_comment_ranges(line, in_block_comment):
    """行内のコメント範囲を状態機械で特定する。

    文字列リテラル ("...") と文字リテラル ('.') をスキップし、
    // ラインコメントと /* */ ブロックコメントを正確に検出する。

    Args:
        line: 解析対象の行文字列
        in_block_comment: 前行から継続中のブロックコメントがあるか

    Returns:
        (ranges, in_block_comment):
            ranges - コメント部分の (start, end) インデックスリスト
            in_block_comment - 次行へのブロックコメント継続状態
    """
    ranges = []
    i = 0
    n = len(line)

    # 前行からブロックコメント継続中なら、閉じるまでがコメント
    if in_block_comment:
        start = 0
        while i < n - 1:
            if line[i] == '*' and line[i + 1] == '/':
                ranges.append((start, i + 2))
                i += 2
                in_block_comment = False
                break
            i += 1
        else:
            # 行末まで閉じなかった — 行全体がコメント
            if in_block_comment:
                ranges.append((start, n))
                return ranges, True

    while i < n:
        c = line[i]

        # 文字列リテラルをスキップ
        if c == '"':
            i += 1
            while i < n:
                if line[i] == '\\':
                    i += 2  # エスケープシーケンスをスキップ
                    continue
                if line[i] == '"':
                    i += 1
                    break
                i += 1
            continue

        # 文字リテラルをスキップ
        if c == "'":
            i += 1
            while i < n:
                if line[i] == '\\':
                    i += 2
                    continue
                if line[i] == "'":
                    i += 1
                    break
                i += 1
            continue

        # ラインコメント // → 行末まで
        if c == '/' and i + 1 < n and line[i + 1] == '/':
            ranges.append((i, n))
            return ranges, in_block_comment

        # ブロックコメント開始 /*
        if c == '/' and i + 1 < n and line[i + 1] == '*':
            start = i
            i += 2
            while i < n - 1:
                if line[i] == '*' and line[i + 1] == '/':
                    ranges.append((start, i + 2))
                    i += 2
                    break
                i += 1
            else:
                # 行末まで閉じなかった
                if not (i < n and i > start + 1 and line[i - 1] == '*' and line[i] == '/'):
                    ranges.append((start, n))
                    in_block_comment = True
            continue

        i += 1

    return ranges, in_block_comment


def insert_jp_en_spaces(text):
    """日本語と英語の境界にスペースを挿入する。

    パス1: 日本語→英語 （例: "のfar" → "の far"）
    パス2: 英語→日本語 （例: "FOV値" → "FOV 値"）

    数字 (0-9) は対象外。既にスペースがある箇所は自然にスキップされる。

    Args:
        text: 変換対象のテキスト

    Returns:
        スペース挿入後のテキスト
    """
    # パス1: JP→EN
    text = re.sub(f'({JP})({EN})', r'\1 \2', text)
    # パス2: EN→JP
    text = re.sub(f'({EN})({JP})', r'\1 \2', text)
    return text


def process_line(line, in_block_comment):
    """行内のコメント部分のみにスペース挿入を適用する。

    コード部分はそのまま保持し、コメント範囲のみ insert_jp_en_spaces を適用。
    範囲は後ろから処理することでインデックスのズレを回避する。

    Args:
        line: 処理対象の行
        in_block_comment: ブロックコメント継続状態

    Returns:
        (new_line, in_block_comment): 変換後の行と次行への状態
    """
    ranges, new_block_state = find_comment_ranges(line, in_block_comment)

    if not ranges:
        return line, new_block_state

    # 後ろの範囲から処理してインデックスのズレを防ぐ
    for start, end in reversed(ranges):
        comment_text = line[start:end]
        new_comment = insert_jp_en_spaces(comment_text)
        if comment_text != new_comment:
            line = line[:start] + new_comment + line[end:]

    return line, new_block_state


def process_file(filepath, dry_run=False, verbose=False):
    """ファイルを処理し、コメント内の日英境界にスペースを挿入する。

    CRLF/LF を保持するため newline='' で読み書きする。

    Args:
        filepath: 処理対象ファイルパス
        dry_run: True なら変更せず差分表示のみ
        verbose: True なら詳細ログ出力

    Returns:
        変更があった行数
    """
    try:
        with open(filepath, 'r', encoding='utf-8', newline='') as f:
            original_content = f.read()
    except UnicodeDecodeError:
        # UTF-8 でデコードできないファイルはスキップ
        if verbose:
            print(f"  SKIP (not UTF-8): {filepath}")
        return 0

    lines = original_content.split('\n')
    new_lines = []
    in_block_comment = False
    changed_count = 0

    for line_num, line in enumerate(lines, 1):
        new_line, in_block_comment = process_line(line, in_block_comment)
        if new_line != line:
            changed_count += 1
            if dry_run or verbose:
                rel_path = os.path.relpath(filepath)
                print(f"  {rel_path}:{line_num}")
                print(f"    - {line.rstrip()}")
                print(f"    + {new_line.rstrip()}")
        new_lines.append(new_line)

    if changed_count > 0 and not dry_run:
        new_content = '\n'.join(new_lines)
        with open(filepath, 'w', encoding='utf-8', newline='') as f:
            f.write(new_content)

    return changed_count


def collect_files(paths, exclude_patterns):
    """対象ディレクトリから .cpp/.h ファイルを収集する。

    Args:
        paths: 検索対象ディレクトリのリスト
        exclude_patterns: 除外パターン文字列のリスト

    Returns:
        対象ファイルパスのリスト
    """
    files = []
    for base_path in paths:
        base = Path(base_path)
        if not base.exists():
            print(f"WARNING: Path does not exist: {base_path}")
            continue
        # 個別ファイルが渡された場合
        if base.is_file():
            if base.suffix in ('.cpp', '.h'):
                filepath_str = str(base)
                if not any(pat in filepath_str for pat in exclude_patterns):
                    files.append(base)
            continue
        # ディレクトリの場合は再帰探索
        for ext in ('*.cpp', '*.h'):
            for filepath in base.rglob(ext):
                filepath_str = str(filepath)
                if any(pat in filepath_str for pat in exclude_patterns):
                    continue
                files.append(filepath)
    return sorted(files)


def main():
    parser = argparse.ArgumentParser(
        description='C++ コメント内の日本語↔英語境界にスペースを自動挿入'
    )
    parser.add_argument(
        '--dry-run',
        action='store_true',
        help='変更を書き込まず差分を表示するのみ'
    )
    parser.add_argument(
        '--path',
        nargs='+',
        default=['GameProject', 'Engine/TakoEngine/project/engine'],
        help='対象ディレクトリ（複数指定可）'
    )
    parser.add_argument(
        '--exclude',
        nargs='+',
        default=['externals'],
        help='除外パターン（パス内にこの文字列を含むディレクトリをスキップ）'
    )
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='詳細ログを出力'
    )

    args = parser.parse_args()

    print(f"=== fix_jp_en_spacing.py ===")
    print(f"Mode: {'DRY-RUN (変更なし)' if args.dry_run else 'APPLY (変更を書き込み)'}")
    print(f"Paths: {args.path}")
    print(f"Exclude: {args.exclude}")
    print()

    files = collect_files(args.path, args.exclude)
    print(f"Found {len(files)} files to process.")
    print()

    total_changes = 0
    changed_files = 0

    for filepath in files:
        count = process_file(filepath, dry_run=args.dry_run, verbose=args.verbose)
        if count > 0:
            total_changes += count
            changed_files += 1

    print()
    print(f"=== Summary ===")
    print(f"Files scanned:  {len(files)}")
    print(f"Files changed:  {changed_files}")
    print(f"Lines changed:  {total_changes}")
    if args.dry_run:
        print(f"(dry-run mode — no files were modified)")


if __name__ == '__main__':
    main()
