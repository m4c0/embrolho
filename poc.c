#pragma leco tool
#pragma leco add_include_dir "MBROLA/Database/"
#pragma leco add_include_dir "MBROLA/Misc/"
#pragma leco add_include_dir "MBROLA/Engine/"
#pragma leco add_include_dir "MBROLA/Parser/"
#pragma leco add_impl impl

#include "database.h"

int main(int argc, char ** argv) {
  Database * dba = init_rename_Database("?", NULL, NULL);
  if (!dba) return 1;

  return 0;
}
