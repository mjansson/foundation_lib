#!/usr/bin/env python

"""Ninja toolchain abstraction for Microsoft compiler suite"""


class MSVCToolchain(object):
  def __init__( self ):
  	pass

def create( toolchain ):
  return MSVCToolchain()
