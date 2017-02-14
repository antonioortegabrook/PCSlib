#! /bin/bash
# A simple script to build all the externals in Deployment configuration

xcodebuild -workspace PCSlib-workspace/PCSlib.xcworkspace -scheme max-external-all clean
xcodebuild -workspace PCSlib-workspace/PCSlib.xcworkspace -scheme max-external-all -configuration Deployment

