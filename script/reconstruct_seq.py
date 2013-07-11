#! /usr/bin/env python

import linecache
import json
from pprint import pprint
import sys

def main( ):

    print "Reading graph...",
    json_data = open( sys.argv[ 1 ] )
    data = json.load( json_data )
    # pprint( data )
    json_data.close( )
    print "done"

    reads = {}
    print "Reading reads.."
    fin = open( sys.argv[ 2 ] )
    num = 0
    for line in fin.readlines( ):
        if line.startswith( ">" ):
            num = int( line[ line.find("> read-") + len("> read-"):-1 ] )
        else:
            read = line[ 0:-1 ]
            reads[ num ] = read
    print "..done"

    readswithoutp = [ ]
    alreadyseenreads = [ False for _ in range( 0, len( reads )  ) ]
    is_already_visited = [ False for _ in range( 0, len( reads ) ) ]

    print "Testing edges...",

    for edge in data["edges"]:
        alreadyseenreads[ edge["to"] ] = True
    print "done"

    print "Looking for first node...",

    for x in range( 0, len( data["edges"] ) + 1 ):
        if x % 500 ==  0:
            print "Edges analyzed: {0}\t Contig found: {1} \r".format(x, len( readswithoutp ) ),
        if not alreadyseenreads[ x ]:
            readswithoutp.append( x)

    if len( readswithoutp ) == 0:
        print "There's some error in the graph"
        sys.exit( -1 )

    print "Number of contig: ", len( readswithoutp )

    current_read = readswithoutp[ 0 ]
    del readswithoutp[ 0 ]
           
    fout = open( sys.argv[ 3 ], 'w' )

    seqnum =0
    print "Reconstructing sequence #{0}..\r".format( seqnum ),
    fout.write( "> sequence {0}\n".format( seqnum ) )
    seqnum += 1
    sequence = reads[ current_read ]

    while True:
        try:
            edge = data[ "edges" ][ current_read ]
            if is_already_visited[ edge["to"] ]:
                fout.write( sequence )
                sequence = ""
                raise Exception( "NULL" )
            is_already_visited[ edge["to"] ] = True
            sequence += reads[ edge["to"] ][ -edge["length"]: ]
            current_read = edge[ "to" ]
            if len( sequence )  > 1000000:
                fout.write( sequence )
                sequence = ""
        except:
            if len( readswithoutp ) == 0:
                break
            else:
                fout.write( sequence )
                fout.write("\n")
                print "Reconstructing sequence #{0}..\r".format( seqnum ),
                fout.write( "> sequence-{0}\n".format( seqnum ) )
                seqnum += 1
                current_read = readswithoutp[ 0 ]
                del readswithoutp[ 0 ]
                sequence = reads[ current_read ]
                           

    # while current_read in edgedict.keys( ) or len( readswithoutp ) > 0:
    #     numsreadsdone += 1
    #     if numsreadsdone % 777 == 0:
    #         print "Reads passed by: {0}\r".format( numsreadsdone ),
    #     if current_read in edgedict.keys( ):
    #         old_read = current_read
    #         current_read = edgedict[ current_read ][ "to" ]
    #         del edgedict[ old_read ]
    #         read = reads[ current_read ]
    #         write_read += read
    #         if len( write_read ) > 1000:
    #             fout.write( write_read )
    #             write_read = ""
    #     else:
    #         if len( write_read ) > 0:
    #             fout.write( write_read )
    #             write_read = ""
    #         current_read = readswithoutp[ 0 ]
    #         readswithoutp.remove( current_read )
    #         fout.write( "\n> sequence {0}\n".format(seqnum) )
    #         print "Assembling sequence #{0}...".format( seqnum )
    #         fout.write( reads[ current_read ] )
    #         seqnum += 1

    #     current_edge = edgedict[ current_read ]
    #     read = reads[ current_read ]
    #     fout.write( read )
    #     last_read = current_read
    #     current_read = edgedict[ current_read ][ "to" ]
    #     del edgedict[ last_read ]
    #     while current_read != None and current_read not in edgedict and len( readswithoutp ) > 0:
    #         current_read = readswithoutp[ 0 ]
    #         readswithoutp.remove( current_read )
    #         fout.write("\n> sequence {0}\n".format( seqnum ) )
    #         # fout.write( linecache.getline( sys.argv[ 2 ], ( current_read+1 )*2 )[ 0:-1 ] )
    #         print "Assembling sequence #{0}..".format( seqnum )
    #         seqnum += 1

    # #print len( readswithoutp )

    fout.write( "\n" )
    fout.close( )

if __name__ == "__main__":
    if len( sys.argv ) < 4 :
        print "Usage: reconstruct_seq.py <graph_json_file> <reads_file> <output_seq>"
    else:
        main() 
