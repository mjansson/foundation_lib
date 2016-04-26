#!/usr/bin/env python

"""Ninja build configurator for foundation library example program"""

import sys
import os

sys.path.insert(0, os.path.join('build', 'ninja'))

import generator

generator = generator.Generator(project = 'example', variables = [('bundleidentifier', 'com.rampantpixels.foundation.$(binname)')])
target = generator.target
writer = generator.writer
toolchain = generator.toolchain

sources = ['main.c']
includepaths = ['..']
libpaths = ['..']

if target.is_ios() or target.is_android() or target.is_tizen() or target.is_pnacl():
  resources = []
  extrasources = []
  if target.is_ios():
    resources = [os.path.join('ios', item) for item in [
      'example.plist', 'Images.xcassets', 'example.xib'
    ]]
    extrasources = [os.path.join('ios', item) for item in [
      'viewcontroller.m'
    ]]
  elif target.is_android():
    resources = [os.path.join('android', item) for item in [
      'AndroidManifest.xml', os.path.join('layout', 'main.xml'), os.path.join('values', 'strings.xml'),
      os.path.join('drawable-ldpi', 'icon.png'), os.path.join('drawable-mdpi', 'icon.png'), os.path.join('drawable-hdpi', 'icon.png'),
      os.path.join('drawable-xhdpi', 'icon.png'), os.path.join('drawable-xxhdpi', 'icon.png'), os.path.join('drawable-xxxhdpi', 'icon.png')
    ]]
    extrasources = [os.path.join('android', 'java', 'com', 'rampantpixels', 'foundation', 'example', item) for item in [
      'ExampleActivity.java'
    ]]
  elif target.is_tizen():
    resources = [os.path.join('tizen', item) for item in [
      'tizen-manifest.xml', os.path.join('res', 'tizenapp.png')
    ]]
  generator.app(module = 'example', sources = sources + extrasources, binname = 'example', libs = ['foundation'], resources = resources, includepaths = includepaths, libpaths = libpaths)
else:
  generator.bin(module = 'example', sources = sources, binname = 'example', libs = ['foundation'], includepaths = includepaths, libpaths = libpaths)
