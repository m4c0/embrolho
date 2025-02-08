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

Database * embrolho_init_db() {
  return init_rename_Database("en1", NULL, NULL);
}
Fifo * embrolho_init_fifo() {
  return init_Fifo(FIFO_SIZE);
}
Input * embrolho_init_input(Fifo * fifo) {
  Input * input = (Input *)MBR_malloc(sizeof(Input));
  input->self = fifo;
  input->readline_Input = readline_i;
  input->close_Input = close_i;
  input->reset_Input = reset_i;
  return input;
}
Parser * embrolho_init_parserinput(Input * input, Database * db) {
  float pitch = (float)Freq(db) / (float)MBRPeriod(db);
  float freq_ratio = 1.0;
  float time_ratio = 1.0;
  char * comment_symbol = ";";

  return init_ParserInput(input, sil_phon(db), pitch, time_ratio, freq_ratio, comment_symbol, NULL);
}
Mbrola * embrolho_init_mbrola(Parser * parser, Database * dba) {
  Mbrola * brola = init_Mbrola(dba);
  set_parser_Mbrola(brola, parser);
  return brola;
}

void embrolho_close_mbrola(Mbrola * brola) { close_Mbrola(brola); }
void embrolho_close_parser(Parser * parser) { parser->close_Parser(parser); }
void embrolho_close_input(Input * input) { input->close_Input(input); }
void embrolho_close_fifo(Fifo * fifo) { close_Fifo(fifo); }
void embrolho_close_database(Database * dba) { dba->close_Database(dba); }

int embrolho_write_fifo(Fifo * fifo, char * msg) {
  return write_Fifo(fifo, msg);
}
int embrolho_read_mbrola(Mbrola * brola, short * buffer, int n) {
  return readtype_Mbrola(brola, buffer, n, LIN16);
}

const char * embrolho_errbuffer() {
  return errbuffer;
}
