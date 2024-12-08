#include "dragon.h"
#include "elf.h"
#include "knight_errant.h"

Dragon::Dragon(int x, int y) : NPC(DragonType, x, y) {}
Dragon::Dragon(std::istream &is) : NPC(DragonType, is) {}

bool Dragon::accept(std::shared_ptr<NPC> other) {
  if (std::dynamic_pointer_cast<Dragon>(other)) {
    return true;
  }
  if (std::dynamic_pointer_cast<Elf>(other)) {
    return true;
  }
  if (std::dynamic_pointer_cast<KnightErrant>(other)) {
    return true;
  }

  return false;
}

void Dragon::print() { std::cout << *this; }

void Dragon::save(std::ostream &os) {
  os << DragonType << std::endl;
  NPC::save(os);
}

std::ostream &operator<<(std::ostream &os, Dragon &dragon) {
  os << "dragon: " << *static_cast<NPC *>(&dragon) << std::endl;
  return os;
}
