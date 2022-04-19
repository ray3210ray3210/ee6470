#include <cassert>
#include <cstdio>
#include <cstdlib>
using namespace std;

#include "Testbench.h"

unsigned char header[54] = {
    0x42,          // identity : B
    0x4d,          // identity : M
    0,    0, 0, 0, // file size
    0,    0,       // reserved1
    0,    0,       // reserved2
    54,   0, 0, 0, // RGB data offset
    40,   0, 0, 0, // struct BITMAPINFOHEADER size
    0,    0, 0, 0, // bmp width
    0,    0, 0, 0, // bmp height
    1,    0,       // planes
    24,   0,       // bit per pixel
    0,    0, 0, 0, // compression
    0,    0, 0, 0, // data size
    0,    0, 0, 0, // h resolution
    0,    0, 0, 0, // v resolution
    0,    0, 0, 0, // used colors
    0,    0, 0, 0  // important colors
};

Testbench::Testbench(sc_module_name n)
    : sc_module(n), initiator("initiator"), output_rgb_raw_data_offset(54) {
  SC_THREAD(do_sort);
}

Testbench::~Testbench() = default;

int Testbench::read_bmp(string infile_name) {
  //----------merge sort-------------------------------------------
  FILE *fp_s = NULL; // source file handler

  fp_s = fopen(infile_name.c_str(), "r");
  if (fp_s == NULL) {
    printf("fopen %s error\n", infile_name.c_str());
    return -1;
  }

  //store in stimulus array
	for( int i = 0; i != SIZE; i++ )
	{
	  int value;
		assert(fscanf(fp_s, "%u\n", &value));
    stimulus_array[i]= value;        
		// if(i==0)
		// pre_sample_time = sc_time_stamp();
	}

	fclose( fp_s );
  return 0;
  //----------0419-------------------------------------------------
/*
  FILE *fp_s = NULL; // source file handler
  fp_s = fopen(infile_name.c_str(), "rb");
  if (fp_s == NULL) {
    printf("fopen %s error\n", infile_name.c_str());
    return -1;
  }
  // move offset to 10 to find rgb raw data offset
  fseek(fp_s, 10, SEEK_SET);
  assert(fread(&input_rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s));

  // move offset to 18 to get width & height;
  fseek(fp_s, 18, SEEK_SET);
  assert(fread(&width, sizeof(unsigned int), 1, fp_s));
  assert(fread(&height, sizeof(unsigned int), 1, fp_s));

  // get bit per pixel
  fseek(fp_s, 28, SEEK_SET);
  assert(fread(&bits_per_pixel, sizeof(unsigned short), 1, fp_s));
  bytes_per_pixel = bits_per_pixel / 8;

  // move offset to input_rgb_raw_data_offset to get RGB raw data
  fseek(fp_s, input_rgb_raw_data_offset, SEEK_SET);

  source_bitmap =
      (unsigned char *)malloc((size_t)width * height * bytes_per_pixel);
  if (source_bitmap == NULL) {
    printf("malloc images_s error\n");
    return -1;
  }

  target_bitmap =
      (unsigned char *)malloc((size_t)width * height * bytes_per_pixel);
  if (target_bitmap == NULL) {
    printf("malloc target_bitmap error\n");
    return -1;
  }

  assert(fread(source_bitmap, sizeof(unsigned char),
               (size_t)(long)width * height * bytes_per_pixel, fp_s));
  fclose(fp_s);

  unsigned int file_size; // file size
  // file size
  file_size = width * height * bytes_per_pixel + output_rgb_raw_data_offset;
  header[2] = (unsigned char)(file_size & 0x000000ff);
  header[3] = (file_size >> 8) & 0x000000ff;
  header[4] = (file_size >> 16) & 0x000000ff;
  header[5] = (file_size >> 24) & 0x000000ff;

  // width
  header[18] = width & 0x000000ff;
  header[19] = (width >> 8) & 0x000000ff;
  header[20] = (width >> 16) & 0x000000ff;
  header[21] = (width >> 24) & 0x000000ff;

  // height
  header[22] = height & 0x000000ff;
  header[23] = (height >> 8) & 0x000000ff;
  header[24] = (height >> 16) & 0x000000ff;
  header[25] = (height >> 24) & 0x000000ff;

  // bit per pixel
  header[28] = bits_per_pixel;

  return 0;*/
}

int Testbench::write_bmp(string outfile_name) {
  //----------merge sort-------------------------------------------
  FILE *fp_t = NULL; // target file handler

  fp_t = fopen(outfile_name.c_str(), "wb");
  if (fp_t == NULL) {
    printf("fopen %s error\n", outfile_name.c_str());
    return -1;
  }

	// write value to response file
  for( int i = 0; i != SIZE; i++ ) {
    fprintf( fp_t, "%u\n", response_array[i] );
  }

  fclose( fp_t );
  return 0;

  //----------0419-------------------------------------------------

  /*FILE *fp_t = NULL; // target file handler

  fp_t = fopen(outfile_name.c_str(), "wb");
  if (fp_t == NULL) {
    printf("fopen %s error\n", outfile_name.c_str());
    return -1;
  }

  // write header
  fwrite(header, sizeof(unsigned char), output_rgb_raw_data_offset, fp_t);

  // write image
  fwrite(target_bitmap, sizeof(unsigned char),
         (size_t)(long)width * height * bytes_per_pixel, fp_t);

  fclose(fp_t);
  return 0;*/
}

void Testbench::do_sort() {
  //----------merge sort-------------------------------------------
  
  int i, j;           // for loop counter
  unsigned char R;    // color of R, G, B
  word data;
  unsigned char mask[4];

  wait(5 * CLOCK_PERIOD, SC_NS);
  for( i = 0; i != SIZE; i++ ) {
    R =stimulus_array[i];
    data.uc[0] = R;
    mask[0] = 0xff;
    mask[1] = 0;
    mask[2] = 0;
    mask[3] = 0;
    initiator.write_to_socket(MERGE_MM_BASE + MERGE_SORT_R_ADDR, mask,
                              data.uc, 4);
    cout << "intput : Now at " << sc_time_stamp() << endl; //print current sc_time

    wait(1 * CLOCK_PERIOD, SC_NS);
  }

  for( j = 0; j != SIZE; j++) {
  bool done=false;
      int output_num=0;
      while(!done){
        initiator.read_from_socket(MERGE_MM_BASE + MERGE_SORT_CHECK_ADDR, mask, data.uc, 4);
        output_num = data.sint;
        if(output_num>0) done=true;
      }
      wait(10 * CLOCK_PERIOD, SC_NS);

      initiator.read_from_socket(MERGE_MM_BASE + MERGE_SORT_RESULT_ADDR, mask,
                                 data.uc, 4);
      //target_bitmap = data.sint;
      response_array[j] = data.sint;
      // debug
      cout << "output : Now at " << sc_time_stamp() << endl; //print current sc_time
  }
  //  initiator.write_to_socket(RAM_MM_BASE, mask, target_bitmap,
  //                           CHAR_SIZE *SIZE);
  //  initiator.write_to_socket(RAM_MM_BASE + output_rgb_raw_data_offset, mask,
  //                           target_bitmap, bytes_per_pixel * height * width);
  sc_stop();
  //----------0419-------------------------------------------------
}
