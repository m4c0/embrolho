export module embrolho;
import hai;

namespace embrolho::handles {
  struct db;
  struct fifo;
  struct input;
  struct mbrola;
  struct parser;
}

extern "C" {
  using namespace embrolho::handles;

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

namespace embrolho {
  export struct error {};
  export const char * errbuffer() { return embrolho_errbuffer(); }

  struct deleter {
    void operator()(handles::db     * h) { embrolho_close_database(h); }
    void operator()(handles::fifo   * h) { embrolho_close_fifo(h);     }
    void operator()(handles::input  * h) { embrolho_close_input(h);    }
    void operator()(handles::parser * h) { embrolho_close_parser(h);   }
    void operator()(handles::mbrola * h) { embrolho_close_mbrola(h);   }
  };

  export class t {
    [[nodiscard]] static handles::db * init_db() {
      auto res = embrolho_init_db();
      if (!res) throw error {};
      return res;
    }

    hai::holder<db,     deleter> m_db     { init_db() };
    hai::holder<fifo,   deleter> m_fifo   { embrolho_init_fifo() };
    hai::holder<input,  deleter> m_input  { embrolho_init_input(*m_fifo) };
    hai::holder<parser, deleter> m_parser { embrolho_init_parserinput(*m_input, *m_db) };
    hai::holder<mbrola, deleter> m_brola  { embrolho_init_mbrola(*m_parser, *m_db) };

  public:
    void write_pho(const char * pho) {
      if (embrolho_write_fifo(*m_fifo, pho) < 0) throw error {};
    }
    [[nodiscard]] unsigned read(short * buf, unsigned n) {
      auto i = embrolho_read_mbrola(*m_brola, buf, n);
      if (i < 0) throw error {};
      return i;
    }
  };
}
