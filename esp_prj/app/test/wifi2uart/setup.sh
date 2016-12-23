#!/usr/bin/env bash

echo "upload start"

esp file write ./config.lua
esp file write ./esutil.lua
esp file write ./wifi_setup.lua
esp file write ./init.lua

echo "upload complete"

esp file list