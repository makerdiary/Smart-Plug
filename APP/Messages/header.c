/**
*
* @file header.c
*
* @brief Common message header
*
* This is a message header that has the basics:
* - MAC Address
* - Protocol Version
* - Flags
* - Sequence Number
*
* Of course you will probably want to change this header format for your own use.
*
* $Rev: 598 $
* $Author: dsmith $
* $Date: 2010-06-14 16:08:43 -0700 (Mon, 14 Jun 2010) $
*
* YOU ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS?WITHOUT WARRANTY 
* OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, 
* TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL TEXAS INSTRUMENTS 
* OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, 
* BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
* INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY 
* CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/
#include "header.h"
#include "dprint_macros.h"

/** uncomment to display more information */
//#define VERBOSE_HEADER

void printHeader(struct header* h)
{
  printf("#%u: V/F=%02X/%02X, mac=", h->sequence, h->version, h->flags);
  for (int j = 0; j<8; j++)
  {
    printf("%02X", h->mac[j]);
    if (j != 7) printf("-");
  }
}

/** Serializes the Header into memory starting at destination.
Number of bytes streamed is equal to getSizeOfHeader() 
@param h the header to serialize
@param destination points to a region of memory at least getSizeOfHeader() bytes in size
*/
void serializeHeader(struct header* h, unsigned char* destination)
{
  unsigned char* destinationPtr = destination;
  *destinationPtr++ = h->sequence & 0xFF;  //LSB first
  *destinationPtr++ = h->sequence >> 8;
  *destinationPtr++ = h->version; 
  *destinationPtr++ = h->flags;
  for (int i = 0; i<8; i++)
    *destinationPtr++ = h->mac[i];
}

/** Gets the size of the header.
@return the size of the header, in bytes 
*/
unsigned int getSizeOfHeader(struct header* h) { return 12; }

/* Deserializes the stream of bytes into a struct header
@param sourcePtr points to a sequence of bytes that is a serialized header, getSizeOfHeader() bytes long
@return a header structure created from the bytes
*/
struct header deserializeHeader(unsigned char* source)
{
  unsigned char* sourcePtr = source;
  struct header h;
  unsigned char lsb = *sourcePtr++;
  unsigned char msb = *sourcePtr++;
  h.sequence = lsb + 0x0100 * msb;
  h.version = *sourcePtr++;  //version
  h.flags = *sourcePtr++;  //flags
  for (int i=0; i<8; i++)
  {
    h.mac[i] = *sourcePtr++;
  }
#ifdef VERBOSE_HEADER  
  printHeader(h);
  printf("\r\n");
  for (int i=0; i<12; i++)
    printf("%02X ", source[i]);
  printf("\r\nHeader Parsed %u bytes\r\n", sourcePtr-source);
#endif  
  return h;
}