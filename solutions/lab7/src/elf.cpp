#include "elf.h"
#include "dragon.h"
#include "knight_errant.h"


Elf::Elf(int x, int y) : NPC(ElfType, x, y) {}
Elf::Elf(std::istream &is) : NPC(ElfType, is) {}

bool Elf::accept(std::shared_ptr<NPC> other) {
  if (std::dynamic_pointer_cast<KnightErrant>(other)) {
    return true;
  }
  return false;
}

void Elf::print() { std::cout << *this; }

void Elf::save(std::ostream &os) {
  os << ElfType << std::endl;
  NPC::save(os);
}

std::ostream &operator<<(std::ostream &os, Elf &elf) {
  os << "elf: " << *static_cast<NPC *>(&elf) << std::endl;
  return os;
}
