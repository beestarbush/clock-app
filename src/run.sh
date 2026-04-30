#!/bin/bash

cd build
QT_LOGGING_RULES="WebSocketService.debug=false;EnvironmentService.debug=false" ./clock-app -platform xcb
