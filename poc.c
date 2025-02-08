#pragma leco tool
#pragma leco add_include_dir "MBROLA/Database/"
#pragma leco add_include_dir "MBROLA/Misc/"
#pragma leco add_include_dir "MBROLA/Engine/"
#pragma leco add_include_dir "MBROLA/Parser/"
#pragma leco add_impl impl

#define LIBRARY

#include "database.h"
#include "fifo.h"
#include "input.h"
#include "parser.h"
#include "parser_input.h"
#include "mbrola.h"

static long readline_i(Input * in, char * line, int size) {
  return readline_Fifo((Fifo *)in->self, line, size);
}
static void reset_i(Input * in) {
  reset_Fifo((Fifo *)in->self);
}
static void close_i(Input * in) {
  MBR_free(in);
}

int main(int argc, char ** argv) {
  Database * dba = init_rename_Database("en1", NULL, NULL);
  if (!dba) return 1;

  Fifo * fifo = init_Fifo(FIFO_SIZE);

  Input * input = (Input *)MBR_malloc(sizeof(Input));
  input->self = fifo;
  input->readline_Input = readline_i;
  input->close_Input = close_i;
  input->reset_Input = reset_i;

  float pitch = (float)Freq(dba) / (float)MBRPeriod(dba);
  float freq_ratio = 1.0;
  float time_ratio = 1.0;
  char * comment_symbol = ";";

  Parser * parser = init_ParserInput(input, sil_phon(dba), pitch, time_ratio, freq_ratio, comment_symbol, NULL);
  Mbrola * brola = init_Mbrola(dba);
  set_parser_Mbrola(brola, parser);

  if (write_Fifo(fifo, "_   51 \n") < 0) return 2;
  if (write_Fifo(fifo, "p   62 \n") < 0) return 2;
  if (write_Fifo(fifo, "aU 127 \n") < 0) return 2;
  if (write_Fifo(fifo, "@  110 \n") < 0) return 2;
  if (write_Fifo(fifo, "_    9 \n") < 0) return 2;
  if (write_Fifo(fifo, "#      \n") < 0) return 2;

  FILE * output = fopen("out/res.ulaw", "wb");
  short buffer[16000];
  int i;
  while ((i = readtype_Mbrola(brola, buffer, 16000, LIN16)) == 16000)
    fwrite(buffer, 2, i, output);

  if (i < 0) {
    fprintf(stderr, "error: %s\n", errbuffer);
    return 3;
  }
  if (i != 0) fwrite(buffer, 2, i, output);
  fclose(output);

  close_Mbrola(brola);
  parser->close_Parser(parser);
  input->close_Input(input);
  close_Fifo(fifo);
  dba->close_Database(dba);
  return 0;
}
