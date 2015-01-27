#!/usr/bin/python

"""Ninja platform abstraction"""

import sys

def supported_platforms():
  return [ 'windows', 'linux', 'macosx', 'ios', 'android', 'raspberrypi', 'pnacl' ]

class Platform(object):
  def __init__(self, platform):
    self.platform = platform
    if self.platform is None:
      self.platform = sys.platform
    if self.platform.startswith('linux'):
      self.platform = 'linux' 
    elif self.platform.startswith('darwin'):
      self.platform = 'macosx' 
    elif self.platform.startswith('macos'):
      self.platform = 'macosx' 
    elif self.platform.startswith('win'):
      self.platform = 'windows' 
    elif self.platform.startswith('ios'):
      self.platform = 'ios' 
    elif self.platform.startswith('android'):
      self.platform = 'android' 
    elif self.platform.startswith('raspberry'):
      self.platform = 'raspberrypi' 
    elif self.platform.startswith('pnacl'):
      self.platform = 'pnacl' 

  def platform(self):
    return self.platform

  def is_linux(self):
    return self.platform == 'linux'

  def is_windows(self):
    return self.platform == 'windows'

  def is_macosx(self):
    return self.platform == 'macosx'

  def is_ios(self):
    return self.platform == 'ios'

  def is_android(self):
    return self.platform == 'android'

  def is_raspberrypi(self):
    return self.platform == 'raspberrypi'

  def is_pnacl(self):
    return self.platform == 'pnacl'
