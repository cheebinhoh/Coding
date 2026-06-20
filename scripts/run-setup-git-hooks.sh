#!/bin/sh
#
# Copyright © 2026 Chee Bin HOH. All rights reserved.
#

set -eu

HOOK_PATH=".git/hooks/pre-commit"

if [ -e "$HOOK_PATH" ]; then
  echo "Error: $HOOK_PATH already exists." >&2
  exit 1
fi

mkdir -p "$(dirname "$HOOK_PATH")"

cat > "$HOOK_PATH" <<'EOF'
#!/bin/sh
#
# Copyright © 2026 Chee Bin HOH. All rights reserved.
#

./scripts/run-tab-check-and-clang-format.sh
EOF

chmod +x "$HOOK_PATH"

echo "Installed pre-commit hook at $HOOK_PATH"
