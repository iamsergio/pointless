#!/bin/bash

# Runs app and gives you a console

xcrun devicectl device process launch --device ${IPHONE_DEVICE_ID}  --console com.iamsergio.pointless
