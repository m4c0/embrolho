#pragma leco app
import print;
import siaudio;

import embrolho;

int main(int argc, char ** argv) try {
  embrolho::t emb {};

  // These are five reasons
  // ARPA:  DH IY Z . AA R . F AY V . R IY Z AH N Z .
  // SAMPA: D i: z _ A: r _ f aI v _ r i: z V n z _
  constexpr const auto pho = R"(
_  30
D  30
i: 50
z  30
_  30
A: 50
r  30
_  30
f  30
aI 50
v  30
_  30
r  30
i: 50
z  30
V  30
n  30
z  30
_  30
#
  )";
  emb.write_pho(pho);

  bool playing = true;

  siaudio::rate(16000);
  siaudio::filler([&](auto data, auto smps) {
    for (auto x = 0; x < smps; x++) data[x] = 0;
    if (!playing) return;

    short buffer[16000];
    auto i = emb.read(buffer, smps);
    if (i == 0) playing = false;
    else for (auto x = 0; x < i; x++) {
      float s = buffer[x];
      data[x] = s / ((2 << 16) - 1);
    }
  });

  while (playing);

  return 0;
} catch (...) {
  return 1;
}
