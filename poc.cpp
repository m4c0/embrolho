#pragma leco tool
#pragma leco add_impl api
import print;
import siaudio;

struct db;
struct fifo;
struct input;
struct mbrola;
struct parser;

extern "C" {
  db * embrolho_init_db();
  fifo * embrolho_init_fifo();
  input * embrolho_init_input(fifo *);
  parser * embrolho_init_parserinput(input *, db *);
  mbrola * embrolho_init_mbrola(parser *, db *);

  void embrolho_close_mbrola(mbrola *);
  void embrolho_close_parser(parser *);
  void embrolho_close_input(input *);
  void embrolho_close_fifo(fifo *);
  void embrolho_close_database(db *);

  int embrolho_write_fifo(fifo *, const char *);
  int embrolho_read_mbrola(mbrola *, short * buf, int n);

  const char * embrolho_errbuffer();
}

int main(int argc, char ** argv) {
  auto dba = embrolho_init_db();
  if (!dba) return 1;

  auto fifo = embrolho_init_fifo();
  auto input = embrolho_init_input(fifo);
  auto parser = embrolho_init_parserinput(input, dba);
  auto brola = embrolho_init_mbrola(parser, dba);

  constexpr const auto pho = R"(
    _   51
    p   62
    aU 127
    @  110
    _    9
    #
  )";
  if (embrolho_write_fifo(fifo, pho) < 0) return 2;

  bool playing = true;

  siaudio::rate(16000);
  siaudio::filler([&](auto data, auto smps) {
    for (auto x = 0; x < smps; x++) data[x] = 0;
    if (!playing) return;

    short buffer[16000];
    auto i = embrolho_read_mbrola(brola, buffer, smps);
    if (i < 0) putln("error: %s\n", embrolho_errbuffer());
    if (i <= 0) playing = false;
    else for (auto x = 0; x < smps; x++) {
      float s = buffer[x];
      data[x] = x > i ? 0 : s / ((2 << 16) - 1);
    }
  });

  while (playing);

  embrolho_close_mbrola(brola);
  embrolho_close_parser(parser);
  embrolho_close_input(input);
  embrolho_close_fifo(fifo);
  embrolho_close_database(dba);
  return 0;
}
