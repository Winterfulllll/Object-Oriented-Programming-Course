#pragma once
#include "npc.h"

struct Elf : public NPC {
  Elf(int x, int y);
  Elf(std::istream &is);

  void print() override;

  bool accept(std::shared_ptr<NPC> visitor) override;

  void save(std::ostream &os) override;

  friend std::ostream &operator<<(std::ostream &os, Elf &elf);
};
