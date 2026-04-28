#!/usr/bin/env bash
# 사용법: ./scripts/seal-secret.sh <app> <key> <value>
# 예시:   ./scripts/seal-secret.sh web-main AUTH_SECRET "mysecret"
#
# 사전 조건:
#   - kubeseal CLI 설치: brew install kubeseal
#   - Sealed Secrets 컨트롤러가 kube-system에 배포되어 있어야 함
#   - kubectl context가 올바른 클러스터를 가리켜야 함

set -euo pipefail

APP="${1:-}"
KEY="${2:-}"
VALUE="${3:-}"
NAMESPACE="platform"

if [[ -z "$APP" || -z "$KEY" || -z "$VALUE" ]]; then
  echo "사용법: $0 <app> <key> <value>"
  echo "  app:   web-main | web-admin | web-partner"
  echo "  key:   시크릿 키 (예: AUTH_SECRET)"
  echo "  value: 평문 값"
  exit 1
fi

SECRET_NAME="${APP}-secret"
TARGET_FILE="infra/k8s/${APP}/sealed-secret.yaml"

if [[ ! -f "$TARGET_FILE" ]]; then
  echo "오류: $TARGET_FILE 파일을 찾을 수 없습니다."
  exit 1
fi

SEALED=$(echo -n "$VALUE" | kubeseal \
  --raw \
  --name "$SECRET_NAME" \
  --namespace "$NAMESPACE" \
  --controller-name sealed-secrets-controller \
  --controller-namespace kube-system \
  --from-file /dev/stdin)

echo ""
echo "암호화 완료. 아래 값을 $TARGET_FILE 의 $KEY 필드에 붙여넣으세요:"
echo ""
echo "    $KEY: $SEALED"
echo ""
