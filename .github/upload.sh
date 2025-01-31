#!/bin/bash

RELEASE_NAME="$1"
RELEASE_ID="$2"


# Zip up a package
zip -r "$RELEASE_NAME.zip" artifacts/release/

# remove, use the lil semicolon hack to continue execution if any fail
gh release delete-asset "$RELEASE_ID" "$RELEASE_NAME.zip" -y; gh release delete-asset "$RELEASE_ID" 3ds-donut.3dsx -y; gh release delete-asset "$RELEASE_ID" 3ds-donut.cia -y;

# upload the new releases
gh release upload "$RELEASE_ID" "$RELEASE_NAME.zip"
gh release upload "$RELEASE_ID" artifacts/release/3ds-donut.3dsx
gh release upload "$RELEASE_ID" artifacts/release/3ds-donut.cia

