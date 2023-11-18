#include "WAVhdr.h"


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

uint32_t WAVhdr::getVal(uint8_t pos, uint8_t len) {
  uint32_t v = 0;
  for (int c = len - 1; c >= 0; c--) v = (v << 8) | wBuf[pos + c];
  return v;
}


uint8_t *WAVhdr::getBuffer() {
  return wBuf;
}


WAVhdr_t WAVhdr::getData() {
  return wHdr;
}


int WAVhdr::processBuffer() {

  wHdr.audioFormat   = getVal(WAVHDR_POS_AUDIOFORMAT  ,WAVHDR_SIZE_AUDIOFORMAT  );
  wHdr.numChannels   = getVal(WAVHDR_POS_NUMCHANNELS  ,WAVHDR_SIZE_NUMCHANNELS  );
  wHdr.sampleRate    = getVal(WAVHDR_POS_SAMPLERATE   ,WAVHDR_SIZE_SAMPLERATE   );
  wHdr.byteRate      = getVal(WAVHDR_POS_BYTERATE     ,WAVHDR_SIZE_BYTERATE     );
  wHdr.blockAlign    = getVal(WAVHDR_POS_BLOCKALIGN   ,WAVHDR_SIZE_BLOCKALIGN   );
  wHdr.bitsPerSample = getVal(WAVHDR_POS_BITSPERSAMPLE,WAVHDR_SIZE_BITSPERSAMPLE);
  wHdr.chunkSize     = getVal(WAVHDR_POS_CHUNKSIZE    ,WAVHDR_SIZE_CHUNKSIZE    );
  wHdr.dataSize      = getVal(WAVHDR_POS_SUBCHUNK2SIZE,WAVHDR_SIZE_SUBCHUNK2SIZE);
  wHdr.dataPos       = WAVHDR_LEN;

  if ((getVal(WAVHDR_POS_CHUNKID      ,WAVHDR_SIZE_CHUNKID      ) != 0x46464952    ) ||
      (getVal(WAVHDR_POS_FORMAT       ,WAVHDR_SIZE_FORMAT       ) != 0x45564157    ) ||
      (getVal(WAVHDR_POS_SUBCHUNK1ID  ,WAVHDR_SIZE_SUBCHUNK1ID  ) != 0x20746d66    ) ||
      (getVal(WAVHDR_POS_SUBCHUNK1SIZE,WAVHDR_SIZE_SUBCHUNK1SIZE) != 16            ) ||
      (getVal(WAVHDR_POS_SUBCHUNK2ID  ,WAVHDR_SIZE_SUBCHUNK2ID  ) != 0x61746164    ) ||
      (wHdr.audioFormat != WAVHDR_FMT_PCM)) {
    return false;
  }
  if ((wHdr.bitsPerSample != 8   ) ||
      (wHdr.numChannels   != 1   ) ||
      (wHdr.sampleRate    > 48000) ||
      (wHdr.sampleRate    < 8000 )) {
        return false;
      }
  return true;
}


int WAVhdr::processBuffer(readCallBack callBack) {
  uint32_t cType, cLen, cPos;
  int l;

  callBack(wBuf, WAVHDR_MIN);
  cPos = WAVHDR_MIN;

  wHdr.audioFormat   = getVal(WAVHDR_POS_AUDIOFORMAT  ,WAVHDR_SIZE_AUDIOFORMAT  );
  wHdr.numChannels   = getVal(WAVHDR_POS_NUMCHANNELS  ,WAVHDR_SIZE_NUMCHANNELS  );
  wHdr.sampleRate    = getVal(WAVHDR_POS_SAMPLERATE   ,WAVHDR_SIZE_SAMPLERATE   );
  wHdr.byteRate      = getVal(WAVHDR_POS_BYTERATE     ,WAVHDR_SIZE_BYTERATE     );
  wHdr.blockAlign    = getVal(WAVHDR_POS_BLOCKALIGN   ,WAVHDR_SIZE_BLOCKALIGN   );
  wHdr.bitsPerSample = getVal(WAVHDR_POS_BITSPERSAMPLE,WAVHDR_SIZE_BITSPERSAMPLE);
  wHdr.chunkSize     = getVal(WAVHDR_POS_CHUNKSIZE    ,WAVHDR_SIZE_CHUNKSIZE    );

  if ((getVal(WAVHDR_POS_CHUNKID      ,WAVHDR_SIZE_CHUNKID      ) != 0x46464952    ) ||
      (getVal(WAVHDR_POS_FORMAT       ,WAVHDR_SIZE_FORMAT       ) != 0x45564157    ) ||
      (getVal(WAVHDR_POS_SUBCHUNK1ID  ,WAVHDR_SIZE_SUBCHUNK1ID  ) != 0x20746d66    ) ||
      (getVal(WAVHDR_POS_SUBCHUNK1SIZE,WAVHDR_SIZE_SUBCHUNK1SIZE) != 16            ) ||
      (wHdr.audioFormat != WAVHDR_FMT_PCM)) {
    return false;
  }

  while (true) {
    callBack(wBuf, 8);
    cPos += 8;
    cType = getVal(0, 4);
    cLen  = getVal(4, 4);
    wHdr.dataSize = cLen;

    if (cType == 0x61746164 )
      break;
    
    while (cLen) {
      l = cLen > WAVHDR_LEN ? WAVHDR_LEN : cLen;
      cPos += callBack(wBuf, l);
      cLen -= l;
    }
  }
  wHdr.dataPos = cPos;
  
  if ((wHdr.bitsPerSample != 8   ) ||
      (wHdr.numChannels   != 1   ) ||
      (wHdr.sampleRate    > 48000) ||
      (wHdr.sampleRate    < 8000 )) {
        return false;
      }
  return true;
}
