#!/usr/bin/env bash

set -Eeuo pipefail

readonly SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
readonly SOURCE_DIR="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

if [[ $# -ne 1 ]]; then
  echo "用法: $0 <部署目录>" >&2
  echo "示例: $0 /data/app/gt4_app" >&2
  exit 1
fi

readonly DEPLOY_SOURCE_DIR="$1"

if [[ "${DEPLOY_SOURCE_DIR}" != /* ]]; then
  echo "错误: 部署目录必须为绝对路径: ${DEPLOY_SOURCE_DIR}" >&2
  exit 1
fi

if ! command -v rsync >/dev/null 2>&1; then
    echo "错误: 未找到 rsync，请先安装 rsync。" >&2
    exit 1
fi

if [[ ! -f "${SOURCE_DIR}/CMakeLists.txt" ]]; then
    echo "错误: 无法从脚本位置识别 gt4_app 源码目录: ${SOURCE_DIR}" >&2
    exit 1
fi

mkdir -p "${DEPLOY_SOURCE_DIR}"

rsync -a \
    --exclude='/build/' \
    --exclude='/bin/' \
    --exclude='/.git/' \
    --exclude='/.vscode/' \
    --exclude='/.idea/' \
    --exclude='*.tmp' \
    --exclude='*.log' \
    "${SOURCE_DIR}/" "${DEPLOY_SOURCE_DIR}/"

cat <<EOF
源码已同步到: ${DEPLOY_SOURCE_DIR}

请在目标服务器执行干净构建：
  cd ${DEPLOY_SOURCE_DIR}
  cmake --preset linux-debug
  cmake --build build -j

重新配置后，CommL3GaussDB 会从以下目录链接并写入运行时库路径：
  ${DEPLOY_SOURCE_DIR}/third_party/gauss_sdk/lib
EOF