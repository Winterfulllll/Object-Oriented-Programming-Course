#include "knight_errant.h"
#include "dragon.h"
#include "elf.h"


KnightErrant::KnightErrant(int x, int y) : NPC(KnightErrantType, x, y) {}
KnightErrant::KnightErrant(std::istream &is) : NPC(KnightErrantType, is) {}

bool KnightErrant::accept(std::shared_ptr<NPC> other) {
  if (std::dynamic_pointer_cast<Dragon>(other)) {
    return true;
  }
  return false;
}

void KnightErrant::print() { std::cout << *this; }

void KnightErrant::save(std::ostream &os) {
  os << KnightErrantType << std::endl;
  NPC::save(os);
}

std::ostream &operator<<(std::ostream &os, KnightErrant &knight_errant) {
  os << "knight_errant: " << *static_cast<NPC *>(&knight_errant) << std::endl;
  return os;
}
