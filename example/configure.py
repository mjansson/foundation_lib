#!/usr/bin/env python

"""Ninja build configurator for foundation library example program"""

import sys
import os

sys.path.insert(0, os.path.join('build', 'ninja'))

import generator

project = 'example'

sources = ['main.c']

libs = ['foundation']

includepaths = ['..'] # For foundation library
libpaths = ['..'] # For foundation library

generator = generator.Generator(project = project, variables = [('bundleidentifier', 'com.maniccoder.foundation.$(binname)')])

if generator.target.is_macosx() or generator.target.is_ios() or generator.target.is_android() or generator.target.is_tizen():
  resources = []
  extrasources = []
  if generator.target.is_ios():
    resources = [os.path.join('ios', item) for item in [
      'example.plist', 'Images.xcassets', 'example.xib'
    ]]
    extrasources = [os.path.join('ios', item) for item in [
      'viewcontroller.m'
    ]]
  elif generator.target.is_android():
    resources = [os.path.join('android', item) for item in [
      'AndroidManifest.xml', os.path.join('layout', 'main.xml'), os.path.join('values', 'strings.xml'),
      os.path.join('drawable-ldpi', 'icon.png'), os.path.join('drawable-mdpi', 'icon.png'), os.path.join('drawable-hdpi', 'icon.png'),
      os.path.join('drawable-xhdpi', 'icon.png'), os.path.join('drawable-xxhdpi', 'icon.png'), os.path.join('drawable-xxxhdpi', 'icon.png')
    ]]
    extrasources = [os.path.join('android', 'java', 'com', 'maniccoder', 'foundation', 'example', item) for item in [
      'ExampleActivity.java'
    ]]
  elif generator.target.is_tizen():
    resources = [os.path.join('tizen', item) for item in [
      'tizen-manifest.xml', os.path.join('res', 'tizenapp.png')
    ]]
  generator.app(module = project, sources = sources + extrasources, binname = project, libs = libs, resources = resources, includepaths = includepaths, libpaths = libpaths)
else:
  generator.bin(module = project, sources = sources, binname = project, libs = libs, includepaths = includepaths, libpaths = libpaths)
