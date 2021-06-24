#include "wrappers/buffered_io.h"
#include "../../include/make_unique.h"

bool MIStream::pipeTo(MOStream* ostream) {
    auto br = std::make_unique<BufferedReader>(this);
    auto bw = std::make_unique<BufferedWriter>(ostream);

    bool error = false;

    do {
        auto buffer = br->read();
        
        if(buffer->length() != 0) {
            //Serial.printf("FSTEST: Bytes read into buffred reader: %d\n",buffer->length());
            int written = bw->write(buffer);
            //Serial.printf("FSTEST: Bytes written into buffred writer: %d\n",written);
            error = buffer->length() != written;
        }
    } while (!br->eof());

    this->close();
    ostream->close();

    return error;

}

