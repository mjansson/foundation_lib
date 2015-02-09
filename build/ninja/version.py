#!/usr/bin/env python

"""Version utility"""

import subprocess
import os

def generate_version_string():
  git_version = subprocess.check_output( [ 'git', 'describe' ] ).strip()
  tokens = git_version.split('-')
  version_numbers = tokens[0].split('.')

  source = """/* version.c  -  Foundation library  -  Public Domain  -  2015 Mattias Jansson / Rampant Pixels
* 
* This library provides a cross-platform foundation library in C11 providing basic support data types and
* functions to write applications and games in a platform-independent fashion. The latest source code is
* always available at
*
* https://github.com/rampantpixels/foundation_lib
*
* This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
*
*/

#include <foundation/platform.h>
#include <foundation/types.h>

version_t foundation_version( void )
{
"""
  source += "	return version_make( " + version_numbers[0] + ", " + version_numbers[1] + ", " + version_numbers[2] + ", " + tokens[1] + ", 0x" + tokens[2][1:] + " );\n}\n"
  return source

def read_version_string(input_path):
  try:
    file = open( os.path.join( input_path, 'version.c' ), "r" )
    str = file.read()
    file.close()
  except IOError:
    str = ""
  return str

def write_version_string(output_path, str):
  file = open( os.path.join( output_path, 'version.c' ), "w" )
  file.write( str )
  file.close

def generate_version(output_path):
  generated = generate_version_string()
  previous = read_version_string(output_path)

  if generated != previous:
    write_version_string(output_path, generated)
