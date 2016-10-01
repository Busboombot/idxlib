
// Read a movement command from the serial port

#include "idx_command.h"
#include <Arduino.h>
#include "CRC32.h"




// Wait for a header sync string, then read the entire header. 
int IDXCommandBuffer::run(){
    
    if(ser.available()){
        
        char c =  ser.read();
        
        if (buf_pos < sizeof(last_command->sync)) {
           
            // When reading the sync string, ensure that each additional char in 
            // the buffer equals the one in the same position in the sync str. 
            // If not, the characters read so far aren't sync chars, so start over. 
            if ( c == last_command->sync[buf_pos] ){
                buf_pos++;
                
            } else{
                buf_pos = 0;
            }
            Serial.print(c);
       
        } else {
            
            *(char*)(((char*)last_command)+buf_pos) = c;
            
            buf_pos++;
            
            //Serial.print(buf_pos-1);Serial.print(" ");
            //Serial.print((uint8_t)c);Serial.print(" ");
            //Serial.println(*(uint8_t*)(((uint8_t*)last_command)+buf_pos-1) );
            
            if (buf_pos == sizeof(struct command)){
                
                uint32_t crc  = CRC32::checksum( (const uint8_t*)last_command, 
                                             sizeof(*last_command) - sizeof(last_command->crc));
                 
                
                Serial.println(crc);
                if (crc == last_command->crc){
                    Serial.println("ACK");
                    commands.add(last_command);
                    sendResponse(*last_command, IDX_COMMAND_ACK );
                    last_command = new command();
                } else {
                    Serial.println("nack");
                    sendResponse(*last_command, IDX_COMMAND_NACK );
                }
           
                buf_pos = 0;
            }
        }
    }
    
    return buf_pos;
}

void IDXCommandBuffer::send(struct command & command){     
    
    uint32_t crc  = CRC32::checksum( (const uint8_t*)&command, 
                                 sizeof(*last_command) - sizeof(last_command->crc));
          
    ser.write( (uint8_t*)&command, sizeof(struct command));
}

void IDXCommandBuffer::sendResponse(struct command & command, int code){ 
    response.seq = command.seq;
    response.code = code;         
    send(response);
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
# Nasty hack based on 
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
