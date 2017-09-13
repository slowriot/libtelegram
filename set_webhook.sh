#!/bin/bash
# Simple script to tell telegram to set a webhook at a particular place

token="$1"
url="$2"
if [ -z "$token" ]; then
  echo "Usage: $0 token [url]" >&2
  echo "Omit url argument to unset instead of setting." >&2
  exit 1
fi

endpoint="https://api.telegram.org"
api="$endpoint/bot$token"

if [ -z "$url" ]; then
  result=$(wget -q "$api/deleteWebhook" -O -)
else
  result=$(wget -q "$api/setWebhook?url=$url" -O -)
fi

# pretty-print the result only if jq is available
jq_path=$(which jq)
if [ -x "$jq_path" ]; then
  jq . <<< "$result"
else
  echo "$result"
fi
