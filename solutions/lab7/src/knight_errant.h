#pragma once
#include "npc.h"

struct KnightErrant : public NPC {
  KnightErrant(int x, int y);
  KnightErrant(std::istream &is);

  void print() override;

  bool accept(std::shared_ptr<NPC> visitor) override;

  void save(std::ostream &os) override;

  friend std::ostream &operator<<(std::ostream &os,
                                  KnightErrant &knight_errant);
};
