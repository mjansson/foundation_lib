#!/usr/bin/env python

"""Ninja toolchain abstraction for XCode toolchain"""

import os
import subprocess

import toolchain

def make_target(toolchain, host, target):
  return XCode(toolchain, host, target)

class XCode(object):
  def __init__(self, toolchain, host, target):
    self.toolchain = toolchain
    self.host = host
    self.target = target

  def initialize_toolchain(self):
    self.organisation = ''
    self.bundleidentifier = ''
    self.provisioning = ''
    self.deploymenttarget = ''

  def build_toolchain(self):
    if self.target.is_macosx():
      sdk = 'macosx'
      deploytarget = 'MACOSX_DEPLOYMENT_TARGET=' + self.deploymenttarget
    elif self.target.is_ios():
      sdk = 'iphoneos'
      deploytarget = 'IPHONEOS_DEPLOYMENT_TARGET=' + self.deploymenttarget

    platformpath = subprocess.check_output( [ 'xcrun', '--sdk', sdk, '--show-sdk-platform-path' ] ).strip()
    localpath = platformpath + "/Developer/usr/bin:/Applications/Xcode.app/Contents/Developer/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin"

    self.plist = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'plutil' ] ).strip()
    self.xcassets = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'actool' ] ).strip()
    self.xib = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'ibtool' ] ).strip()
    self.dsymutil = "PATH=" + localpath + " " + subprocess.check_output( [ 'xcrun', '--sdk', sdk, '-f', 'dsymutil' ] ).strip()

    self.plistcmd = 'build/ninja/plist.py --exename $exename --prodname $prodname --bundle $bundleidentifier --target $target --deploymenttarget $deploymenttarget --output $outpath $in'
    if self.target.is_macosx():
      self.xcassetscmd = 'mkdir -p $outpath && $xcassets --output-format human-readable-text --output-partial-info-plist $outplist' \
                         ' --app-icon AppIcon --launch-image LaunchImage --platform macosx --minimum-deployment-target ' + self.deploymenttarget + \
                         ' --target-device mac --compress-pngs --compile $outpath $in >/dev/null'
      self.xibcmd = '$xib --target-device mac --module $module --minimum-deployment-target ' + self.deploymenttarget + \
                    ' --output-partial-info-plist $outplist --auto-activate-custom-fonts' \
                    ' --output-format human-readable-text --compile $outpath $in'
    elif self.target.is_ios():
      self.xcassetscmd = 'mkdir -p $outpath && $xcassets --output-format human-readable-text --output-partial-info-plist $outplist' \
                         ' --app-icon AppIcon --launch-image LaunchImage --platform iphoneos --minimum-deployment-target ' + self.deploymenttarget + \
                         ' --target-device iphone --target-device ipad --compress-pngs --compile $outpath $in >/dev/null'
      self.xibcmd = '$xib --target-device iphone --target-device ipad --module $module --minimum-deployment-target ' + self.deploymenttarget + \
                    ' --output-partial-info-plist $outplist --auto-activate-custom-fonts' \
                    ' --output-format human-readable-text --compile $outpath $in &> /dev/null '
    self.dsymutilcmd = '$dsymutil $in -o $outpath'
    self.codesigncmd = 'build/ninja/codesign.py --target $target --prefs codesign.json --builddir $builddir --binname $binname --config $config $outpath'

  def parse_prefs(self, prefs):
    if self.target.is_ios() and 'ios' in prefs:
      iosprefs = prefs['ios']
      if 'deploymenttarget' in iosprefs:
        self.deploymenttarget = iosprefs['deploymenttarget']
      if 'organisation' in iosprefs:
        self.organisation = iosprefs['organisation']
      if 'bundleidentifier' in iosprefs:
        self.bundleidentifier = iosprefs['bundleidentifier']
      if 'provisioning' in iosprefs:
        self.provisioning = iosprefs['provisioning']
    elif self.target.is_macosx() and 'macosx' in prefs:
      macosxprefs = prefs['macosx']
      if 'deploymenttarget' in macosxprefs:
        self.deploymenttarget = macosxprefs['deploymenttarget']
      if 'organisation' in macosxprefs:
        self.organisation = macosxprefs['organisation']
      if 'bundleidentifier' in macosxprefs:
        self.bundleidentifier = macosxprefs['bundleidentifier']
      if 'provisioning' in macosxprefs:
        self.provisioning = macosxprefs['provisioning']

  def write_variables(self, writer):
    pass

  def write_rules(self, writer):
    writer.rule( 'dsymutil', command = self.dsymutilcmd, description = 'DSYMUTIL $outpath' )
    writer.rule( 'plist', command = self.plistcmd, description = 'PLIST $outpath' )
    writer.rule( 'xcassets', command = self.xcassetscmd, description = 'XCASSETS $outpath' )
    writer.rule( 'xib', command = self.xibcmd, description = 'XIB $outpath' )
    writer.rule( 'codesign', command = self.codesigncmd, description = 'CODESIGN $outpath' )

  def apk(self, toolchain, writer, module, archbins, outpath, binname, basepath, config, implicit_deps, resources, codesign):
    #Outputs
    builtbin = []
    builtres = []
    builtsym = []

    #Paths
    builddir = os.path.join('$buildpath', config, 'app', binname)
    dsympath = binpath + '.dSYM'

    #Extract debug symbols from universal binary
    dsymcontentpath = os.path.join( dsympath, 'Contents' )
    builtsym = writer.build( [ os.path.join( dsymcontentpath, 'Resources', 'DWARF', binname ), os.path.join( dsymcontentpath, 'Resources', 'DWARF' ), os.path.join( dsymcontentpath, 'Resources' ), os.path.join( dsymcontentpath, 'Info.plist' ), dsymcontentpath, dsympath ], 'dsymutil', unibinary, variables = [ ( 'outpath', dsympath ) ] )

    #Copy final universal binary
    if self.target.is_ios():
      builtbin = self.build_copy( writer, os.path.join( binpath, self.binprefix + binname + self.binext ), unibinary )
    else:
      builtbin = self.build_copy( writer, os.path.join( binpath, 'Contents', 'MacOS', self.binprefix + binname + self.binext ), unibinary, os.path.join( binpath, 'Contents' ), 'MacOS' )

    #Build resources
    if resources:
      has_resources = False

      #Lists of input plists and partial plist files produced by resources
      plists = []
      assetsplists = []
      xibplists = []

      #All resource output files
      outfiles = []

      #First build everything except plist inputs
      for resource in resources:
        if resource.endswith( '.xcassets' ):
          if self.target.is_macosx():
            assetsvars = [ ( 'outpath', os.path.join( binpath, 'Contents', 'Resources' ) ) ]
          else:
            assetsvars = [ ( 'outpath', binpath ) ]
          outplist = os.path.join( builddir, os.path.splitext( os.path.basename( resource ) )[0] + '-xcassets.plist' )
          assetsvars += [ ( 'outplist', outplist ) ]
          outfiles = [ outplist ]
          if self.target.is_macosx():
            outfiles += [ os.path.join( binpath, 'Contents', 'Resources', 'AppIcon.icns' ) ]
          elif self.target.is_ios():
            pass #TODO: Need to list all icon and launch image files here
          assetsplists += writer.build( outfiles, 'xcassets', os.path.join( basepath, module, resource ), variables = assetsvars )
          has_resources = True
        elif resource.endswith( '.xib' ):
          xibmodule = binname.replace( '-', '_' ).replace( '.', '_' )
          if self.target.is_macosx():
            nibpath = os.path.join( binpath, 'Contents', 'Resources', os.path.splitext( os.path.basename( resource ) )[0] + '.nib' )
          else:
            nibpath = os.path.join( binpath, os.path.splitext( os.path.basename( resource ) )[0] + '.nib' )
          plistpath = os.path.join( builddir, os.path.splitext( os.path.basename( resource ) )[0] + '-xib.plist' )
          xibplists += [ plistpath ]
          outfiles = []
          if self.target.is_ios():
            outfiles += [ os.path.join( nibpath, 'objects.nib' ), os.path.join( nibpath, 'objects-8.0+.nib' ), os.path.join( nibpath, 'runtime.nib' ) ]
          outfiles += [ nibpath, plistpath ]
          builtres += writer.build( outfiles, 'xib', os.path.join( basepath, module, resource ), variables = [ ( 'outpath', nibpath ), ( 'outplist', plistpath ), ( 'module', xibmodule ) ] )
          has_resources = True
        elif resource.endswith( '.plist' ):
          plists += [ os.path.join( basepath, module, resource ) ]

      #Extra output files/directories
      outfiles = []
      if has_resources and self.target.is_macosx():
        outfiles += [ os.path.join( binpath, 'Contents', 'Resources' ) ]

      #Now build input plists appending partial plists created by previous resources
      if self.target.is_macosx():
        plistpath = os.path.join( binpath, 'Contents', 'Info.plist' )
        pkginfopath = os.path.join( binpath, 'Contents', 'PkgInfo' )
      else:
        plistpath = os.path.join( binpath, 'Info.plist' )
        pkginfopath = os.path.join( binpath, 'PkgInfo' )
      plistvars = [ ( 'exename', binname ), ( 'prodname', binname ), ( 'outpath', plistpath ) ]
      bundleidentifier = self.make_bundleidentifier( binname )
      if bundleidentifier != '':
        plistvars += [ ( 'bundleidentifier', bundleidentifier ) ]
      outfiles += [ plistpath, pkginfopath ]
      builtres += writer.build( outfiles, 'plist', plists + assetsplists + xibplists, implicit = [ os.path.join( 'build', 'ninja', 'plist.py' ) ], variables = plistvars )

    #Do code signing (might modify binary, but does not matter, nothing should have final binary as input anyway)
    if codesign:
      codesignvars = [ ( 'builddir', builddir ), ( 'binname', binname ), ( 'outpath', binpath ), ( 'config', config ) ]
      if self.target.is_ios():
        if self.ios_provisioning != '':
          codesignvars += [ ( 'provisioning', self.ios_provisioning ) ]
        writer.build( [ os.path.join( binpath, '_CodeSignature', 'CodeResources' ), os.path.join( binpath, '_CodeSignature' ), binpath ], 'codesign', builtbin, implicit = builtres + [ os.path.join( 'build', 'ninja', 'codesign.py' ) ], variables = codesignvars )
      elif self.target.is_macosx():
        if self.macosx_provisioning != '':
          codesignvars += [ ( 'provisioning', self.macosx_provisioning ) ]
        writer.build( [ os.path.join( binpath, 'Contents', '_CodeSignature', 'CodeResources' ), os.path.join( binpath, 'Contents', '_CodeSignature' ), os.path.join( binpath, 'Contents' ), binpath ], 'codesign', builtbin, implicit = builtres + [ os.path.join( 'build', 'ninja', 'codesign.py' ) ], variables = codesignvars )

    return builtbin + builtsym + builtres
