#pragma once
#include "npc.h"

struct Elf : public NPC {
  Elf(int x, int y);
  Elf(std::istream &is);

  void print() override;

  bool accept(Visitor &visitor) override;

  bool fight(std::shared_ptr<KnightErrant> other) override;
  bool fight(std::shared_ptr<Elf> other) override;
  bool fight(std::shared_ptr<Dragon> other) override;

  void save(std::ostream &os) override;

  friend std::ostream &operator<<(std::ostream &os, Elf &elf);
};
