#!/usr/bin/env bash
# Update RELEASE.txt Last release line and Modern release history.
# Usage: update-release-txt.sh <version> <notes-file>
#   version: v2.5 or 2.5
#   notes-file: markdown/text used for the history entry and GitHub Release
set -euo pipefail

if [[ $# -ne 2 ]]; then
    echo "usage: $0 <version> <notes-file>" >&2
    exit 1
fi

VERSION_RAW="$1"
NOTES_FILE="$2"
VERSION="${VERSION_RAW#v}"
DATE="$(date -u +%Y-%m-%d)"

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
RELEASE_TXT="${ROOT}/RELEASE.txt"

if [[ ! -f "${RELEASE_TXT}" ]]; then
    echo "missing ${RELEASE_TXT}" >&2
    exit 1
fi
if [[ ! -f "${NOTES_FILE}" ]]; then
    echo "missing notes file ${NOTES_FILE}" >&2
    exit 1
fi

tmp="$(mktemp)"
trap 'rm -f "${tmp}"' EXIT

# Rewrite Last release: keep any trailing " - P." style marker out of automated lines.
awk -v ver="${VERSION}" -v date="${DATE}" '
  BEGIN { updated = 0 }
  /^Last release:/ {
    print "Last release: " ver " (" date ")"
    updated = 1
    next
  }
  { print }
  END {
    if (!updated) {
      print ""
      print "Last release: " ver " (" date ")"
    }
  }
' "${RELEASE_TXT}" > "${tmp}"

if ! grep -q '^Modern release history$' "${tmp}"; then
    {
        cat "${tmp}"
        echo ""
        echo "Modern release history"
        echo "======================"
        echo ""
    } > "${tmp}.2"
    mv "${tmp}.2" "${tmp}"
fi

entry_file="$(mktemp)"
{
    echo "${VERSION} (${DATE})"
    echo "------------------"
    cat "${NOTES_FILE}"
    echo ""
} > "${entry_file}"

# Insert the new entry immediately after the Modern release history banner.
awk -v entry_file="${entry_file}" '
  BEGIN {
    while ((getline line < entry_file) > 0) {
      entry = entry line "\n"
    }
    close(entry_file)
  }
  {
    print
    if ($0 == "======================") {
      print ""
      printf "%s", entry
      inserted = 1
    }
  }
  END {
    if (!inserted) {
      print ""
      printf "%s", entry
    }
  }
' "${tmp}" > "${RELEASE_TXT}"

rm -f "${entry_file}"
echo "Updated ${RELEASE_TXT} for ${VERSION_RAW}"
