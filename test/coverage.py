
import re

def parse_gcov( gcovfile ):
  """Parses a .gcov file and returns a report array of line coverage
  """

  report = []
  ignore_block = False

  with open( gcovfile ) as gcov:
    for line in gcov:
      tokens = line.split(':')
      if len(tokens) < 2:
        continue

      count = tokens[0].strip()
      line = int( tokens[1].strip() )
      source = tokens[2]

      if line == 0:
       continue

      if re.search( r'\bLCOV_EXCL_START\b', source ):
        ignore_block = ignore_block + 1
      elif re.search(r'\bLCOV_EXCL_END\b', source ):
        ignore_block = ignore_block - 1

      if count == '-':
        report.append( None )
      elif count == '#####':
        if ( ignore_block > 0 or
           source.strip().startswith( ( 'inline', 'static' ) ) or
           source.strip() == '}' or
           re.search( r'\bLCOV_EXCL_LINE\b', source ) ):
          report.append( None )
        else:
          report.append( 0 )
      elif count == '=====':
        report.append( 0 )
      else:
        report.append( int( count ) )
  return report
