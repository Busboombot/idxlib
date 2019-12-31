
// Read a movement command from the serial port
#if defined (__arm__) && defined (__SAM3X8E__) // Arduino Due compatible
#include "idx_command.h"
#include <Arduino.h>
#include "CRC32.h"
#include "fastset.h"

#define DEBUG_PRINT_ENABLED false
#include "debug.h"

// Wait for a header sync string, then read the entire header. 
int IDXCommandBuffer::run(){
   
    if(ser.available()){
    
        char c =  ser.read();
        
        if (buf_pos < sizeof(command.sync)) {
            // When reading the sync string, ensure that each additional char in 
            // the buffer equals the one in the same position in the sync str. 
            // If not, the characters read so far aren't sync chars, so start over. 
            if ( c == command.sync[buf_pos] ){
                buf_pos++;
                
            } else{
                buf_pos = 0;
            }
       
        } else {
            
            *(char*)(((char*)&command)+buf_pos) = c;
            
            buf_pos++;
            
            if (buf_pos == sizeof(struct command)){
                // This section is slow, about 90us on a Due
                
               fastSet(LOOP_CLEAR_TICK_PIN);
                // Computing the CRC takes about 30us
                uint32_t crc  = CRC32::checksum( (const uint8_t*)&command, 
                                                 sizeof(command) - sizeof(command.crc));
                
                if (crc == command.crc){
                    // Sending the ack and setting up a new command takes about
                    // 55us

                    queue_time += command.segment_time;
                    sendAck(command.seq);
                    
                    if (command.code == IDX_COMMAND_SEGMENT){
                        Segment& segment = segments.head_next();
                        
                        for (int axis = 0; axis < N_AXES; axis++){
                            segment.axes[axis].setParams(command.segment_time, command.v0[axis], command.v1[axis], command.steps[axis]);
                        }
                  
                        segment.seq = command.seq;
                        segment.code = command.code;
                        segment.segment_time = command.segment_time;
                        

                        //SPT("MSG:");SPT(segment.seq); SPT(segments.start); NL;
                        
                    } else if (command.code == IDX_COMMAND_RESET){
                        #if(DEBUG_PRINT_ENABLED)
                        SPT("RESET");SPT(command.seq); NL;
                        #endif
                        /// TBD
                        sendResponseCode(command.seq, IDX_RESPONSE_RESET);
                        
                    } else if (command.code == IDX_COMMAND_LOAD){
                        #if(DEBUG_PRINT_ENABLED)
                        SPT("LOAD");SPT(command.seq); NL;
                        #endif
                        running = false;
                        loading = true;
                        sendResponseCode(command.seq, IDX_RESPONSE_LOADING);
                        
                    } else if (command.code == IDX_COMMAND_RUN){
                        #if(DEBUG_PRINT_ENABLED)
                        SPT("RUN");SPT(command.seq); NL;
                        #endif                        
                        running = true;
                        loading = false;
                        sendResponseCode(command.seq, IDX_RESPONSE_RUNNING);
                        
                    } else if (command.code == IDX_COMMAND_RUNLOAD){
                        #if(DEBUG_PRINT_ENABLED)
                        SPT("RUNLOAD");SPT(command.seq); NL;
                        #endif
                        running = true;
                        loading = true;
                        sendResponseCode(command.seq, IDX_RESPONSE_RUNLOAD);
                    
                    } 
 
                    
                    
                } else {
                    sendNack(command.seq);
                    
                }
           
                buf_pos = 0;
             
                fastClear(LOOP_CLEAR_TICK_PIN);   
            }
            
        }    
    }
    
    return buf_pos;
}


// From Wikipedia: https://en.wikipedia.org/wiki/Fletcher%27s_checksum
uint16_t fletcher16( uint8_t const *data, size_t bytes )
{
    uint16_t sum1 = 0xff, sum2 = 0xff;
    size_t tlen;
    
    while (bytes) {
        tlen = bytes >= 20 ? 20 : bytes;
        bytes -= tlen;
        do {
            sum2 += sum1 += *data++;
        } while (--tlen);
        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);
    }
    /* Second reduction step to reduce sums to 8 bits */
    sum1 = (sum1 & 0xff) + (sum1 >> 8);
    sum2 = (sum2 & 0xff) + (sum2 >> 8);
    return sum2 << 8 | sum1;
}

/*
# For python, a nasty hack based on 
# Wikipedia: https://en.wikipedia.org/wiki/Fletcher%27s_checksum
def fletcher16( data ):

    sum1 = 0xff
    sum2 = 0xff
    tlen = 0
    i = 0
    bytes = len(data)
    
    while (bytes):
        tlen = 20 if bytes >= 20 else bytes
        bytes -= tlen
        while True:
            sum1 += ord(data[i])
            sum2 += sum1
            i += 1
            tlen -= 1
            if tlen == 0:
                break
     
        sum1 = (sum1 & 0xff) + (sum1 >> 8)
        sum2 = (sum2 & 0xff) + (sum2 >> 8)
    
 
    sum1 = (sum1 & 0xff) + (sum1 >> 8)
    sum2 = (sum2 & 0xff) + (sum2 >> 8)
    return sum2 << 8 | sum1;
*/

#endif // _SAM3XA_