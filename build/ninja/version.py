#!/usr/bin/env python

"""Version utility"""

import subprocess
import os
import sys

def generate_version_string(libname):
  gitcmd = 'git'
  if sys.platform.startswith('win'):
    gitcmd = 'git.exe'
  try:
	git_version = subprocess.check_output( [ gitcmd, 'describe', '--long' ] ).strip()
  except Exception:
    return
  tokens = git_version.split('-')
  version_numbers = tokens[0].split('.')

  source = """/* ****** AUTOMATICALLY GENERATED, DO NOT EDIT ******
   This file is generated from the git describe command.
   Run the configure script to regeneerate this file */

#include <foundation/foundation.h>

version_t """ + libname + """_version( void )
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

def generate_version(libname, output_path):
  generated = generate_version_string(libname)
  if generated == None:
	return
  previous = read_version_string(output_path)

  if generated != previous:
    write_version_string(output_path, generated)

if __name__ == "__main__":
  generate_version(sys.argv[1], sys.argv[2])
