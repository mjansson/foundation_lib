#!/usr/bin/env python

"""PList utility"""

import argparse
import os
import subprocess
import re
import unicodedata

def normalize_char(c):
  try:
    cname = unicodedata.name( unicode(c) )
    cname = cname[:cname.index( ' WITH' )]
    return unicodedata.lookup( cname )
  except ( ValueError, KeyError ):
    return c

def normalize_string(s):
    return ''.join( normalize_char(c) for c in s )

parser = argparse.ArgumentParser( description = 'PList utility for Ninja builds' )
parser.add_argument( 'files',
                     metavar = 'file', type=file, nargs='+',
                     help = 'Source plist file' )
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

if not options.exename:
  options.exename = 'unknown'
if not options.prodname:
  options.prodname = 'unknown'

buildversion = subprocess.check_output( [ 'sw_vers', '-buildVersion' ] ).strip()

lines = []
for f in options.files:
  lines += [ line.strip( '\n\r' ) for line in f ]

#Parse input plist to get package type and signature
bundle_package_type = 'APPL'
bundle_signature = '????'

for i in range( 0, len( lines ) ):
  if 'CFBundlePackageType' in lines[i]:
    match = re.match( '^.*>(.*)<.*$', lines[i+1] )
    if match:
      bundle_package_type = match.group(1)
  if 'CFBundleSignature' in lines[i]:
    match = re.match( '^.*>(.*)<.*$', lines[i+1] )
    if match:
      bundle_signature = match.group(1)

#Write package type and signature to PkgInfo in output path
with open( os.path.join( os.path.dirname( options.output ), 'PkgInfo' ), 'w' ) as pkginfo_file:
  pkginfo_file.write( bundle_package_type + bundle_signature )
  pkginfo_file.close()

#insert os version
for i in range( 0, len( lines ) ):
  if re.match( '^\t*<dict>$', lines[i] ):
    lines.insert( i+1, '\t<key>BuildMachineOSBuild</key>' )
    lines.insert( i+2, '\t<string>' + buildversion + '</string>' )
    break

#replace build variables name
for i in range( 0, len( lines ) ):
  if lines[i].find( '$(EXECUTABLE_NAME)' ) != -1:
    lines[i] = lines[i].replace( '$(EXECUTABLE_NAME)', options.exename )
  if lines[i].find( '$(PRODUCT_NAME)' ) != -1:
    lines[i] = lines[i].replace( '$(PRODUCT_NAME)', options.prodname )
  if lines[i].find( '$(PRODUCT_NAME:rfc1034identifier)' ) != -1:
    lines[i] = lines[i].replace( '$(PRODUCT_NAME:rfc1034identifier)', normalize_string( options.exename ).lower() )

#add supported platform, minimum os version and requirements
for i in range( 0, len( lines ) ):
  if 'CFBundleSignature' in lines[i]:
    lines.insert( i+2,  '\t<key>CFBundleSupportedPlatforms</key>' )
    lines.insert( i+3,  '\t<array>' )
    lines.insert( i+4,  '\t\t<string>iPhoneOS</string>' )
    lines.insert( i+5,  '\t</array>' )
    lines.insert( i+6,  '\t<key>MinimumOSVersion</key>' )
    lines.insert( i+7,  '\t<string>6.0</string>' )
    lines.insert( i+8,  '\t<key>UIDeviceFamily</key>' )
    lines.insert( i+9,  '\t<array>' )
    lines.insert( i+10, '\t\t<integer>1</integer>' )
    lines.insert( i+11, '\t\t<integer>2</integer>' )
    lines.insert( i+12, '\t</array>' )
    break

#add icons and images
for i in range( 0, len( lines ) ):
  if 'UIMainStoryboardFile' in lines[i]:
    #TODO: Detect which icons and launch images that are present
    lines.insert( i+0,  '\t<key>CFBundleIcons</key>' )
    lines.insert( i+1,  '\t<dict>' )
    lines.insert( i+2,  '\t\t<key>CFBundlePrimaryIcon</key>' )
    lines.insert( i+3,  '\t\t<dict>' )
    lines.insert( i+4,  '\t\t\t<key>CFBundleIconFiles</key>' )
    lines.insert( i+5,  '\t\t\t<array>' )
    lines.insert( i+6,  '\t\t\t\t<string>AppIcon29x29</string>' )
    lines.insert( i+7,  '\t\t\t\t<string>AppIcon40x40</string>' )
    lines.insert( i+8,  '\t\t\t\t<string>AppIcon57x57</string>' )
    lines.insert( i+9,  '\t\t\t\t<string>AppIcon60x60</string>' )
    lines.insert( i+10,  '\t\t\t</array>' )
    lines.insert( i+11,  '\t\t</dict>' )
    lines.insert( i+12,  '\t</dict>' )
    lines.insert( i+13,  '\t<key>CFBundleIcons~ipad</key>' )
    lines.insert( i+14,  '\t<dict>' )
    lines.insert( i+15,  '\t\t<key>CFBundlePrimaryIcon</key>' )
    lines.insert( i+16,  '\t\t<dict>' )
    lines.insert( i+17,  '\t\t <key>CFBundleIconFiles</key>' )
    lines.insert( i+18,  '\t\t <array>' )
    lines.insert( i+19,  '\t\t\t <string>AppIcon29x29</string>' )
    lines.insert( i+20,  '\t\t\t <string>AppIcon40x40</string>' )
    lines.insert( i+21,  '\t\t\t <string>AppIcon57x57</string>' )
    lines.insert( i+22,  '\t\t\t <string>AppIcon60x60</string>' )
    lines.insert( i+23,  '\t\t\t <string>AppIcon50x50</string>' )
    lines.insert( i+24,  '\t\t\t <string>AppIcon72x72</string>' )
    lines.insert( i+25,  '\t\t\t <string>AppIcon76x76</string>' )
    lines.insert( i+26,  '\t\t </array>' )
    lines.insert( i+27,  '\t </dict>' )
    lines.insert( i+28,  '\t</dict>' )
    lines.insert( i+29,  '\t<key>UILaunchImageFile</key>' )
    lines.insert( i+30,  '\t<string>LaunchImage</string>' )
    lines.insert( i+31,  '\t<key>UILaunchImages</key>' )
    lines.insert( i+32,  '\t<array>' )
    lines.insert( i+33,  '\t\t<dict>' )
    lines.insert( i+34,  '\t\t\t<key>UILaunchImageMinimumOSVersion</key>' )
    lines.insert( i+35,  '\t\t\t<string>7.0</string>' )
    lines.insert( i+36,  '\t\t\t<key>UILaunchImageName</key>' )
    lines.insert( i+37,  '\t\t\t<string>LaunchImage-700</string>' )
    lines.insert( i+38,  '\t\t\t<key>UILaunchImageOrientation</key>' )
    lines.insert( i+39,  '\t\t\t<string>Portrait</string>' )
    lines.insert( i+40,  '\t\t\t<key>UILaunchImageSize</key>' )
    lines.insert( i+41,  '\t\t\t<string>{320, 480}</string>' )
    lines.insert( i+42,  '\t\t</dict>' )
    lines.insert( i+43,  '\t\t<dict>' )
    lines.insert( i+44,  '\t\t\t<key>UILaunchImageMinimumOSVersion</key>' )
    lines.insert( i+45,  '\t\t\t<string>7.0</string>' )
    lines.insert( i+46,  '\t\t\t<key>UILaunchImageName</key>' )
    lines.insert( i+47,  '\t\t\t<string>LaunchImage-700-568h</string>' )
    lines.insert( i+48,  '\t\t\t<key>UILaunchImageOrientation</key>' )
    lines.insert( i+49,  '\t\t\t<string>Portrait</string>' )
    lines.insert( i+50,  '\t\t\t<key>UILaunchImageSize</key>' )
    lines.insert( i+51,  '\t\t\t<string>{320, 568}</string>' )
    lines.insert( i+52,  '\t\t</dict>' )
    lines.insert( i+53,  '\t\t<dict>' )
    lines.insert( i+54,  '\t\t\t<key>UILaunchImageMinimumOSVersion</key>' )
    lines.insert( i+55,  '\t\t\t<string>7.0</string>' )
    lines.insert( i+56,  '\t\t\t<key>UILaunchImageName</key>' )
    lines.insert( i+57,  '\t\t\t<string>LaunchImage-700-Portrait</string>' )
    lines.insert( i+58,  '\t\t\t<key>UILaunchImageOrientation</key>' )
    lines.insert( i+59,  '\t\t\t<string>Portrait</string>' )
    lines.insert( i+60,  '\t\t\t<key>UILaunchImageSize</key>' )
    lines.insert( i+61,  '\t\t\t<string>{768, 1024}</string>' )
    lines.insert( i+62,  '\t\t</dict>' )
    lines.insert( i+63,  '\t\t<dict>' )
    lines.insert( i+64,  '\t\t\t<key>UILaunchImageMinimumOSVersion</key>' )
    lines.insert( i+65,  '\t\t\t<string>7.0</string>' )
    lines.insert( i+66,  '\t\t\t<key>UILaunchImageName</key>' )
    lines.insert( i+67,  '\t\t\t<string>LaunchImage-700-Landscape</string>' )
    lines.insert( i+68,  '\t\t\t<key>UILaunchImageOrientation</key>' )
    lines.insert( i+69,  '\t\t\t<string>Landscape</string>' )
    lines.insert( i+70,  '\t\t\t<key>UILaunchImageSize</key>' )
    lines.insert( i+71,  '\t\t\t<string>{768, 1024}</string>' )
    lines.insert( i+72,  '\t\t</dict>' )
    lines.insert( i+73,  '\t</array>' )
    break

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
with open( options.output, 'w' ) as plist_file:
  for line in lines:
    #print lines[i]
    plist_file.write( line + '\n' )
  plist_file.close()

#run plutil -convert binary1
sdk = 'iphoneos'
platformpath = subprocess.check_output( [ 'xcrun', '--sdk', sdk, '--show-sdk-platform-path' ] ).strip()
localpath = platformpath + "/Developer/usr/bin:/Applications/Xcode.app/Contents/Developer/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin"
plutil = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'plutil' ] ).strip()
os.system( plutil + ' -convert binary1 ' + options.output )

