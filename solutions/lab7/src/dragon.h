#pragma once
#include "npc.h"

struct Dragon : public NPC {
  Dragon(int x, int y);
  Dragon(std::istream &is);

  void print() override;

  bool accept(std::shared_ptr<NPC> visitor) override;

  void save(std::ostream &os) override;

  friend std::ostream &operator<<(std::ostream &os, Dragon &dragon);
};
