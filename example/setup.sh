#!/bin/bash

NRF5SDK_URL="https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sdks/nrf5/binaries/nrf5_sdk_17.1.0_ddde560.zip"
NRF5SDK_PATH="./nRF5_SDK"

wget -O "$NRF5SDK_PATH.zip" "$NRF5SDK_URL"
unzip -q "$NRF5SDK_PATH.zip"
mv nRF5_SDK_* $NRF5SDK_PATH
