#ifndef WAVHDR_H
#define WAVHDR_H

#include "Arduino.h"


// Offset  Size  Name             Description

// The canonical WAVE format starts with the RIFF header:

// 0         4   ChunkID          Contains the letters "RIFF" in ASCII form
                               // (0x52494646 big-endian form).
// 4         4   ChunkSize        36 + SubChunk2Size, or more precisely:
                               // 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
                               // This is the size of the rest of the chunk 
                               // following this number.  This is the size of the 
                               // entire file in bytes minus 8 bytes for the
                               // two fields not included in this count:
                               // ChunkID and ChunkSize.
// 8         4   Format           Contains the letters "WAVE"
                               // (0x57415645 big-endian form).

// The "WAVE" format consists of two subchunks: "fmt " and "data":
// The "fmt " subchunk describes the sound data's format:

// 12        4   Subchunk1ID      Contains the letters "fmt "
                               // (0x666d7420 big-endian form).
// 16        4   Subchunk1Size    16 for PCM.  This is the size of the
                               // rest of the Subchunk which follows this number.
// 20        2   AudioFormat      PCM = 1 (i.e. Linear quantization)
                               // Values other than 1 indicate some 
                               // form of compression.
// 22        2   NumChannels      Mono = 1, Stereo = 2, etc.
// 24        4   SampleRate       8000, 44100, etc.
// 28        4   ByteRate         == SampleRate * NumChannels * BitsPerSample/8
// 32        2   BlockAlign       == NumChannels * BitsPerSample/8
                               // The number of bytes for one sample including
                               // all channels. I wonder what happens when
                               // this number isn't an integer?
// 34        2   BitsPerSample    8 bits = 8, 16 bits = 16, etc.
          // 2   ExtraParamSize   if PCM, then doesn't exist
          // X   ExtraParams      space for extra parameters

// The "data" subchunk contains the size of the data and the actual sound:

// 36        4   Subchunk2ID      Contains the letters "data"
                               // (0x64617461 big-endian form).
// 40        4   Subchunk2Size    == NumSamples * NumChannels * BitsPerSample/8
                               // This is the number of bytes in the data.
                               // You can also think of this as the size
                               // of the read of the subchunk following this 
                               // number.


#define WAVHDR_LEN 44

#define WAVHDR_POS_CHUNKID        0  
#define WAVHDR_POS_CHUNKSIZE      4  
#define WAVHDR_POS_FORMAT         8  
#define WAVHDR_POS_SUBCHUNK1ID    12 
#define WAVHDR_POS_SUBCHUNK1SIZE  16 
#define WAVHDR_POS_AUDIOFORMAT    20 
#define WAVHDR_POS_NUMCHANNELS    22 
#define WAVHDR_POS_SAMPLERATE     24 
#define WAVHDR_POS_BYTERATE       28 
#define WAVHDR_POS_BLOCKALIGN     32 
#define WAVHDR_POS_BITSPERSAMPLE  34 
#define WAVHDR_POS_SUBCHUNK2ID    36 
#define WAVHDR_POS_SUBCHUNK2SIZE  40 

#define WAVHDR_SIZE_CHUNKID        4 
#define WAVHDR_SIZE_CHUNKSIZE      4 
#define WAVHDR_SIZE_FORMAT         4 
#define WAVHDR_SIZE_SUBCHUNK1ID    4 
#define WAVHDR_SIZE_SUBCHUNK1SIZE  4 
#define WAVHDR_SIZE_AUDIOFORMAT    2 
#define WAVHDR_SIZE_NUMCHANNELS    2 
#define WAVHDR_SIZE_SAMPLERATE     4 
#define WAVHDR_SIZE_BYTERATE       4 
#define WAVHDR_SIZE_BLOCKALIGN     2 
#define WAVHDR_SIZE_BITSPERSAMPLE  2 
#define WAVHDR_SIZE_SUBCHUNK2ID    4 
#define WAVHDR_SIZE_SUBCHUNK2SIZE  4 


typedef struct WAVhdr_t {
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
};


class WAVhdr {
  private:
    uint8_t wBuf[WAVHDR_LEN];

    uint32_t getVal(uint8_t pos, uint8_t len);
  
  public:
    uint8_t *getBuffer();
    int processBuffer();
};




#endif
