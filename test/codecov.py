#!/usr/bin/env python

import sys
import argparse
import os
import subprocess
import json
import hashlib
import requests
from coverage import parse_gcov

def gitcmd( *arguments ):
  gitcmd = 'git'
  if sys.platform.startswith('win'):
    gitcmd = 'git.exe'
  return subprocess.check_output( ['git'] + list( arguments ) )

def gitlog( format ):
  return gitcmd( '--no-pager', 'log', '-1', '--pretty=format:%s' % format )

def gitinfo():
  return {
    'branch' : gitcmd( 'rev-parse', '--abbrev-ref', 'HEAD' ).rstrip(),
    'remotes' : [ { 'name': remote.split()[0], 'url': remote.split()[1] } for remote in gitcmd( 'remote', '-v' ).split( '\n' ) if '(fetch)' in remote ],
    'head' : {
      'id' : gitlog( '%H' ),
      'message' : gitlog( '%s' ),
      'author_name' : gitlog( '%aN' ),
      'author_email' : gitlog( '%ae' ),
      'committer_name' : gitlog( '%cN' ),
      'committer_email' : gitlog( '%ce' )
    }
  }

def read_prefs( filename ):
  if not os.path.isfile( filename ):
    return
  file = open( filename, 'r' )
  prefs = json.load( file )
  file.close()
  return prefs

def merge_reports( basereport, addreport ):
  baselen = len(basereport)
  addlen = len(addreport)
  for line in range( 0, addlen ):
    if line >= baselen:
      basereport.append( addreport[line] )
    elif basereport[line] == None:
      basereport[line] = addreport[line]
    elif basereport[line] == 0 and addreport[line] != None:
      basereport[line] = addreport[line]
    elif addreport[line] != None:
      basereport[line] += addreport[line]
  return basereport

def post_report(prefs, coverage):
  info = gitinfo()
  response = requests.post('https://codecov.io/upload/v2?token=' + prefs['token'] + '&commit=' + info['head']['id'] + '&branch=' + info['branch'] + '&job=' + prefs['job'], data = json.dumps(coverage))
  try:
    result = response.json()
  except ValueError:
    result = { 'error': 'Failure to submit data. '
               'Response [%(status)s]: %(text)s' % {
               'status': response.status_code,
               'text': response.text } }
  return result

argparser = argparse.ArgumentParser( description = 'Ninja build generator' )
argparser.add_argument( '-o', '--objectdir', required = True,
                     help = 'Object directory' )
argparser.add_argument( '-s', '--sourcedir', required = True,
                     help = 'Source directory' )
argparser.add_argument( '-m,', '--merge', action = 'append',
                     help = 'Merge in previous report',
                     default = [] )
argparser.add_argument( '-p', '--post', action='store_true',
                     help = 'Post results to codecov.io',
                     default = False )
options = argparser.parse_args()

infiles = {}
for file in os.listdir( options.objectdir ):
  basefile = os.path.splitext(file)[0]
  if file.endswith(".gcda"):
    if not basefile in infiles:
      infiles[basefile] = ( False, False )
    infiles[basefile] = ( True, infiles[basefile][1] )
  elif file.endswith(".gcno"):
    if not basefile in infiles:
      infiles[basefile] = ( False, False )
    infiles[basefile] = ( infiles[basefile][0], True )

sourcefiles = {}
for file in os.listdir( options.sourcedir ):
  if file.endswith(".c") or file.endswith(".m"):
    sourcefiles[file] = []

for key, pair in infiles.iteritems():
  if pair[0] and pair[1]:
    if sys.platform == "darwin":
      subprocess.check_output( [ "gcov", "-gcda=" + os.path.join( options.objectdir, key + ".gcda" ), "-gcno=" + os.path.join( options.objectdir, key + ".gcno" ), "source" ] )
    else:
      sourcefile = os.path.basename(key).rsplit('-', 1)[0]
      subprocess.check_output(["llvm-cov", "gcov", "-o", os.path.join(options.objectdir, key + ".o"), os.path.join(options.sourcedir, sourcefile + ".c")])
      #subprocess.check_output( [ "gcov", "-o", options.objectdir, "-s", options.sourcedir, key + ".o" ] )
    for file in os.listdir('.'):
      if file.endswith(".gcov"):
        sourcefile = os.path.splitext(file)[0]
        if sourcefile in sourcefiles:
          results = parse_gcov( file )
          sourcefiles[sourcefile] = merge_reports( sourcefiles[sourcefile], results )
        os.remove( file )

for reportfile in options.merge:
  with open( reportfile, 'r' ) as infile:
    report = json.load( infile )
    infile.close()
    sourcelist = report['source_files']
    for sourcefile in sourcelist:
      filename = os.path.basename( sourcefile['name'] )
      if not filename in sourcefiles:
        sourcefiles[filename] = []
      results = sourcefile['coverage']
      sourcefiles[filename] = merge_reports( sourcefiles[filename], results )

coverage = {}
for sourcefile, report in sourcefiles.iteritems():
  if len(report) > 0:
    coverage[os.path.join( options.sourcedir, sourcefile )] = [None] + report

prefs = read_prefs( 'codecov.json' )

codecov = {}
codecov['coverage'] = coverage

if options.post:
  result = post_report(prefs, codecov)
  print result
else:
  with open( 'codecovreport.json', 'w' ) as outfile:
    json.dump( codecov, outfile )
