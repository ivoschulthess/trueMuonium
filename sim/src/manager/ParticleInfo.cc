#include "ParticleInfo.hh"


std::ofstream& operator<<(std::ofstream& stream, const ParticleInfo& particle) {
  // Output format for file and std::cout
  stream << particle.ID << " " << particle.Ene << " " << particle.Time << " " << particle.Mom.x() << " " << particle.Mom.y() << " " << particle.Mom.z() << " " << particle.Pos.x() << " " << particle.Pos.y() << " " << particle.Pos.z() << std::endl;
  return stream;
}
