#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"
rm -rf test-results/screenshots/*
npm run build
npm test
npm run test:e2e
