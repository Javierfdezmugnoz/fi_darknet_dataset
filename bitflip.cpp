#include <stdio.h>      /* fopen, fread, fseek, fclose */
#include <stdlib.h>     /* atoi */
#include <iostream>     /* cout */
#include <bitset>       /* bitset */

int main(int argc, char *argv[]) {

    std::cout << "Reading " << argv[1] << '\n';
    FILE *ifp = fopen(argv[1], "rb");

    // header
    // copied from https://github.com/pjreddie/darknet/blob/a3714d0a2bf92c3dea84c4bea65b2b0c64dbc6b1/src/parser.c#L1218
    int major;
    int minor;
    int revision;
    fread(&major, sizeof(int), 1, ifp);
    fread(&minor, sizeof(int), 1, ifp);
    fread(&revision, sizeof(int), 1, ifp);
    if ((major * 10 + minor) >= 2 && major < 1000 && minor < 1000) {
        fseek(ifp, sizeof(size_t), SEEK_CUR);
    } else {
        fseek(ifp, sizeof(int), SEEK_CUR);
    }
    //std::cout << major << '\n';
    //std::cout << minor << '\n';
    //std::cout << revision << '\n';

    // copy header for output file
    auto header_end = ftell(ifp);
    fseek(ifp, 0L, SEEK_SET);
    char *header = (char *)malloc(header_end * sizeof(char));
    fread(header, sizeof(char), header_end, ifp);

    // read weights into buffer for convenience
    fseek(ifp, 0L, SEEK_END);
    auto file_end = ftell(ifp);
    printf("header file end here: %s\n",header);
    printf("header file end here: %li\n",file_end);
    const size_t n_weights = (file_end - header_end) / sizeof(float);
    std::cout << n_weights << " weights (" << n_weights * sizeof(float) << " bytes)" << '\n';
    fseek(ifp, header_end, SEEK_SET);
    float *buf = (float *)malloc(n_weights * sizeof(float));
    fread(buf, sizeof(float), n_weights, ifp);
    fclose(ifp);

    const auto weight_idx = atoi(argv[2]);
    const auto bit_idx = atoi(argv[3]);

    // flip bit
    union { uint32_t i; float f; } weight;
    weight.f = buf[weight_idx];
    std::bitset<32> bs(weight.i); // only for logging purposes
    std::cout << "Weight " << weight_idx << " [float] = " << weight.f << '\n';
    std::cout << "Weight " << weight_idx << " [bits]  = " << bs << '\n';
    std::cout << "Flipping bit " << bit_idx << '\n';
    weight.i ^= 1U << bit_idx;
    bs.flip(bit_idx);
    std::cout << "Weight " << weight_idx << " [bits]  = " << bs << '\n';
    std::cout << "Weight " << weight_idx << " [float] = " << weight.f << '\n';

    // put modified weight back to the buffer
    buf[weight_idx] = weight.f;

    // write modified weights to output file
    std::cout << "Writing " << argv[4] << '\n';
    FILE *ofp = fopen(argv[4], "wb");
    fwrite(header, sizeof(char), header_end, ofp);
    fwrite(buf, sizeof(float), n_weights, ofp);
    fclose(ofp);

    // free buffers
    free(header);
    free(buf);
}
