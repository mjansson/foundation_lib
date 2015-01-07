#!/usr/bin/env python

"""PList utility"""

import argparse
import os
import subprocess

parser = argparse.ArgumentParser( description = 'PList utility for Ninja builds' )
parser.add_argument( '--source',
                     help = 'Source plist file',
                     default = [] )
parser.add_argument( '--exename',
                     help = 'Executable name',
                     default = [] )
parser.add_argument( '--prodname',
                     help = 'Product name',
                     default = [] )
parser.add_argument( '--output',
                     help = 'Output path',
                     default = [] )
options = parser.parse_args()

buildversion = subprocess.check_output( [ 'sw_vers', '-buildVersion' ] ).strip()

#read source plist

#parse CFBundlePackageType and CFBundleSignature for PkgInfo

#insert os version
#<key>BuildMachineOSBuild</key>
#<string>14B25</string>

#replace executable name
#$(EXECUTABLE_NAME)

#replace product name
#$(PRODUCT_NAME)
#$(PRODUCT_NAME:rfc1034identifier)

#add supported platform
#<key>CFBundleSupportedPlatforms</key>
#<array>
#  <string>iPhoneOS</string>
#</array>

#add requirements
#<key>MinimumOSVersion</key>
#<string>6.0</string>
#<key>UIDeviceFamily</key>
#<array>
#  <integer>1</integer>
#  <integer>2</integer>
#</array>

#add icons and images
#<key>CFBundleIcons</key>
#<dict>
#  <key>CFBundlePrimaryIcon</key>
#  <dict>
#    <key>CFBundleIconFiles</key>
#    <array>
#      <string>AppIcon29x29</string>
#      <string>AppIcon40x40</string>
#      <string>AppIcon57x57</string>
#      <string>AppIcon60x60</string>
#    </array>
#  </dict>
#</dict>
#<key>CFBundleIcons~ipad</key>
#<dict>
#  <key>CFBundlePrimaryIcon</key>
#  <dict>
#   <key>CFBundleIconFiles</key>
#   <array>
#     <string>AppIcon29x29</string>
#     <string>AppIcon40x40</string>
#     <string>AppIcon57x57</string>
#     <string>AppIcon60x60</string>
#     <string>AppIcon50x50</string>
#     <string>AppIcon72x72</string>
#     <string>AppIcon76x76</string>
#   </array>
# </dict>
#</dict>
#<key>UILaunchImageFile</key>
#<string>LaunchImage</string>
#<key>UILaunchImages</key>
#<array>
#  <dict>
#    <key>UILaunchImageMinimumOSVersion</key>
#    <string>7.0</string>
#    <key>UILaunchImageName</key>
#    <string>LaunchImage-700</string>
#    <key>UILaunchImageOrientation</key>
#    <string>Portrait</string>
#    <key>UILaunchImageSize</key>
#    <string>{320, 480}</string>
#  </dict>
#  <dict>
#    <key>UILaunchImageMinimumOSVersion</key>
#    <string>7.0</string>
#    <key>UILaunchImageName</key>
#    <string>LaunchImage-700-568h</string>
#    <key>UILaunchImageOrientation</key>
#    <string>Portrait</string>
#    <key>UILaunchImageSize</key>
#    <string>{320, 568}</string>
#  </dict>
#  <dict>
#    <key>UILaunchImageMinimumOSVersion</key>
#    <string>7.0</string>
#    <key>UILaunchImageName</key>
#    <string>LaunchImage-700-Portrait</string>
#    <key>UILaunchImageOrientation</key>
#    <string>Portrait</string>
#    <key>UILaunchImageSize</key>
#    <string>{768, 1024}</string>
#  </dict>
#  <dict>
#    <key>UILaunchImageMinimumOSVersion</key>
#    <string>7.0</string>
#    <key>UILaunchImageName</key>
#    <string>LaunchImage-700-Landscape</string>
#    <key>UILaunchImageOrientation</key>
#    <string>Landscape</string>
#    <key>UILaunchImageSize</key>
#    <string>{768, 1024}</string>
#  </dict>
#</array>

#add build info
#<key>DTCompiler</key>
#<string>com.apple.compilers.llvm.clang.1_0</string>
#<key>DTPlatformBuild</key>
#<string>12B411</string>
#<key>DTPlatformName</key>
#<string>iphoneos</string>
#<key>DTPlatformVersion</key>
#<string>8.1</string>
#<key>DTSDKBuild</key>
#<string>12B411</string>
#<key>DTSDKName</key>
#<string>iphoneos8.1</string>
#<key>DTXcode</key>
#<string>0611</string>
#<key>DTXcodeBuild</key>
#<string>6A2008a</string>

#write final Info.plist in output path

#run plutil -convert binary1

#write PkgInfo in output path
